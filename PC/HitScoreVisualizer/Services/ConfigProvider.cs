using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using HitScoreVisualizer.Models;
using HitScoreVisualizer.Settings;
using IPA.Utilities;
using Newtonsoft.Json;
using SiraUtil.Tools;
using Zenject;
using Version = Hive.Versioning.Version;

namespace HitScoreVisualizer.Services
{
	public class ConfigProvider : IInitializable
	{
		private readonly SiraLog _siraLog;
		private readonly HSVConfig _hsvConfig;

		private readonly string _hsvConfigsFolderPath;
		private readonly JsonSerializerSettings _jsonSerializerSettings;

		private readonly Dictionary<Version, Func<Configuration, bool>> _migrationActions;

		private readonly Version _minimumMigratableVersion;
		private readonly Version _maximumMigrationNeededVersion;

		private Configuration? _currentConfig;

		internal string? CurrentConfigPath => _hsvConfig.ConfigFilePath;

		internal ConfigProvider(SiraLog siraLog, HSVConfig hsvConfig)
		{
			_siraLog = siraLog;
			_hsvConfig = hsvConfig;

			_jsonSerializerSettings = new JsonSerializerSettings {DefaultValueHandling = DefaultValueHandling.IgnoreAndPopulate, Formatting = Formatting.Indented};
			_hsvConfigsFolderPath = Path.Combine(UnityGame.UserDataPath, nameof(HitScoreVisualizer));

			_migrationActions = new Dictionary<Version, Func<Configuration, bool>>
			{
				{new Version(2, 0, 0), RunMigration2_0_0},
				{new Version(2, 1, 0), RunMigration2_1_0},
				{new Version(2, 2, 3), RunMigration2_2_3}
			};

			_minimumMigratableVersion = _migrationActions.Keys.Min();
			_maximumMigrationNeededVersion = _migrationActions.Keys.Max();
		}

		public async void Initialize()
		{
			if (CreateHsvConfigsFolderIfYeetedByPlayer())
			{
				await SaveConfig(Path.Combine(_hsvConfigsFolderPath, "HitScoreVisualizerConfig (default).json"), Configuration.Default).ConfigureAwait(false);

				var oldHsvConfigPath = Path.Combine(UnityGame.UserDataPath, "HitScoreVisualizerConfig.json");
				if (File.Exists(oldHsvConfigPath))
				{
					try
					{
						var destinationHsvConfigPath = Path.Combine(_hsvConfigsFolderPath, "HitScoreVisualizerConfig (imported).json");
						File.Move(oldHsvConfigPath, destinationHsvConfigPath);

						_hsvConfig.ConfigFilePath = destinationHsvConfigPath;
					}
					catch (Exception e)
					{
						_siraLog.Warning(e);
					}
				}
			}

			if (_hsvConfig.ConfigFilePath == null)
			{
				return;
			}

			var fullPath = Path.Combine(_hsvConfigsFolderPath, _hsvConfig.ConfigFilePath);
			if (!File.Exists(fullPath))
			{
				_hsvConfig.ConfigFilePath = null;
				return;
			}

			var userConfig = await LoadConfig(_hsvConfig.ConfigFilePath).ConfigureAwait(false);
			if (userConfig == null)
			{
				_siraLog.Warning($"Couldn't load userConfig at {fullPath}");
				return;
			}

			var configFileInfo = new ConfigFileInfo(Path.GetFileNameWithoutExtension(_hsvConfig.ConfigFilePath), _hsvConfig.ConfigFilePath)
			{
				Configuration = userConfig, State = GetConfigState(userConfig, Path.GetFileNameWithoutExtension(_hsvConfig.ConfigFilePath))
			};

			await SelectUserConfig(configFileInfo).ConfigureAwait(false);
		}

		public Configuration? GetCurrentConfig()
		{
			return _currentConfig;
		}

		internal async Task<IEnumerable<ConfigFileInfo>> ListAvailableConfigs()
		{
			var configFileInfoList = Directory
				.EnumerateFiles(_hsvConfigsFolderPath, "*", SearchOption.AllDirectories)
				.Select(x => new ConfigFileInfo(Path.GetFileNameWithoutExtension(x), x.Substring(_hsvConfigsFolderPath.Length + 1)))
				.ToList();

			foreach (var configInfo in configFileInfoList)
			{
				configInfo.Configuration = await LoadConfig(Path.Combine(_hsvConfigsFolderPath, configInfo.ConfigPath)).ConfigureAwait(false);
				configInfo.State = GetConfigState(configInfo.Configuration, configInfo.ConfigName);
			}

			return configFileInfoList;
		}

		internal bool ConfigSelectable(ConfigState? state)
		{
			switch (state)
			{
				case ConfigState.Compatible:
				case ConfigState.NeedsMigration:
					return true;
				default:
					return false;
			}
		}

		internal async Task SelectUserConfig(ConfigFileInfo? configFileInfo)
		{
			// safe-guarding just to be sure
			if (!ConfigSelectable(configFileInfo?.State))
			{
				_hsvConfig.ConfigFilePath = null;
				return;
			}

			if (configFileInfo!.State == ConfigState.NeedsMigration)
			{
				if (configFileInfo.Configuration!.IsDefaultConfig)
				{
					configFileInfo.Configuration = Configuration.Default;
				}
				else
				{
					RunMigration(configFileInfo.Configuration!);
				}

				if (true)
				{
					await SaveConfig(configFileInfo.ConfigPath, configFileInfo.Configuration).ConfigureAwait(false);
				}
			}

			_currentConfig = configFileInfo.Configuration;
			_hsvConfig.ConfigFilePath = configFileInfo.ConfigPath;
		}

		internal void UnselectUserConfig()
		{
			_currentConfig = null;
			_hsvConfig.ConfigFilePath = null;
		}

		internal void YeetConfig(string relativePath)
		{
			var fullPath = Path.Combine(_hsvConfigsFolderPath, relativePath);
			if (File.Exists(fullPath))
			{
				File.Delete(fullPath);
			}
		}

		private async Task<Configuration?> LoadConfig(string relativePath)
		{
			CreateHsvConfigsFolderIfYeetedByPlayer(false);

			try
			{
				using var streamReader = new StreamReader(Path.Combine(_hsvConfigsFolderPath, relativePath));
				var content = await streamReader.ReadToEndAsync().ConfigureAwait(false);
				return JsonConvert.DeserializeObject<Configuration>(content, _jsonSerializerSettings);
			}
			catch (Exception ex)
			{
				_siraLog.Warning(ex);
				// Expected behaviour when file isn't an actual hsv config file...
				return null!;
			}
		}

		private async Task SaveConfig(string relativePath, Configuration configuration)
		{
			CreateHsvConfigsFolderIfYeetedByPlayer(false);

			var fullPath = Path.Combine(_hsvConfigsFolderPath, relativePath);
			var folderPath = Path.GetDirectoryName(fullPath);
			if (folderPath != null && Directory.Exists(folderPath))
			{
				Directory.CreateDirectory(folderPath);
			}

			try
			{
				using var streamWriter = new StreamWriter(fullPath, false);
				var content = JsonConvert.SerializeObject(configuration, Formatting.Indented);
				await streamWriter.WriteAsync(content).ConfigureAwait(false);
			}
			catch (Exception e)
			{
				Console.WriteLine(e);
				throw;
			}
		}

		private ConfigState GetConfigState(Configuration? configuration, string configName)
		{
			if (configuration?.Version == null)
			{
				return ConfigState.Broken;
			}

			if (configuration.Version > Plugin.Version)
			{
				return ConfigState.NewerVersion;
			}

			if (configuration.Version < _minimumMigratableVersion)
			{
				return ConfigState.Incompatible;
			}

			if (!Validate(configuration!, configName))
			{
				return ConfigState.ValidationFailed;
			}

			return configuration.Version <= _maximumMigrationNeededVersion ? ConfigState.NeedsMigration : ConfigState.Compatible;
		}

		// ReSharper disable once CognitiveComplexity
		private bool Validate(Configuration configuration, string configName)
		{
			if (!configuration.Judgments?.Any() ?? true)
			{
				_siraLog.Warning($"No judgments found for {configName}");
				return false;
			}

			if (!ValidateJudgments(configuration, configName))
			{
				return false;
			}

			// 99 is the max for NumberFormatInfo.NumberDecimalDigits
			if (configuration.TimeDependenceDecimalPrecision < 0 || configuration.TimeDependenceDecimalPrecision > 99)
			{
				_siraLog.Warning($"timeDependencyDecimalPrecision value {configuration.TimeDependenceDecimalPrecision} is outside the range of acceptable values [0, 99]");
				return false;
			}

			if (configuration.TimeDependenceDecimalOffset < 0 || configuration.TimeDependenceDecimalOffset > Math.Log10(float.MaxValue))
			{
				_siraLog.Warning($"timeDependencyDecimalOffset value {configuration.TimeDependenceDecimalOffset} is outside the range of acceptable values [0, {(int) Math.Log10(float.MaxValue)}]");
				return false;
			}

			if (configuration.BeforeCutAngleJudgments != null)
			{
				configuration.BeforeCutAngleJudgments = configuration.BeforeCutAngleJudgments.OrderByDescending(x => x.Threshold).ToList();
				if (!ValidateJudgmentSegment(configuration.BeforeCutAngleJudgments, configName))
				{
					return false;
				}
			}

			if (configuration.AccuracyJudgments != null)
			{
				configuration.AccuracyJudgments = configuration.AccuracyJudgments.OrderByDescending(x => x.Threshold).ToList();
				if (!ValidateJudgmentSegment(configuration.AccuracyJudgments, configName))
				{
					return false;
				}
			}

			if (configuration.AfterCutAngleJudgments != null)
			{
				configuration.AfterCutAngleJudgments = configuration.AfterCutAngleJudgments.OrderByDescending(x => x.Threshold).ToList();
				if (!ValidateJudgmentSegment(configuration.AfterCutAngleJudgments, configName))
				{
					return false;
				}
			}

			if (configuration.TimeDependenceJudgments != null)
			{
				configuration.TimeDependenceJudgments = configuration.TimeDependenceJudgments.OrderByDescending(x => x.Threshold).ToList();
				if (!ValidateTimeDependenceJudgmentSegment(configuration.TimeDependenceJudgments, configName))
				{
					return false;
				}
			}

			return true;
		}

		// ReSharper disable once CognitiveComplexity
		private bool ValidateJudgments(Configuration configuration, string configName)
		{
			configuration.Judgments = configuration.Judgments!.OrderByDescending(x => x.Threshold).ToList();
			var prevJudgement = configuration.Judgments.First();
			if (prevJudgement.Fade)
			{
				prevJudgement.Fade = false;
			}

			if (!ValidateJudgmentColor(prevJudgement, configName))
			{
				_siraLog.Warning($"Judgment entry for threshold {prevJudgement.Threshold} has invalid color in {configName}");
				return false;
			}

			if (configuration.Judgments.Count > 1)
			{
				for (var i = 1; i < configuration.Judgments.Count; i++)
				{
					var currentJudgement = configuration.Judgments[i];
					if (prevJudgement.Threshold != currentJudgement.Threshold)
					{
						if (!ValidateJudgmentColor(currentJudgement, configName))
						{
							_siraLog.Warning($"Judgment entry for threshold {currentJudgement.Threshold} has invalid color in {configName}");
							return false;
						}

						prevJudgement = currentJudgement;
						continue;
					}

					_siraLog.Warning($"Duplicate entry found for threshold {currentJudgement.Threshold} in {configName}");
					return false;
				}
			}

			return true;
		}

		private bool ValidateJudgmentColor(Judgment judgment, string configName)
		{
			if (judgment.Color.Count != 4)
			{
				_siraLog.Warning($"Judgment for threshold {judgment.Threshold} has invalid color in {configName}! Make sure to include exactly 4 numbers for each judgment's color!");
				return false;
			}

			if (judgment.Color.All(x => x >= 0f))
			{
				return true;
			}

			_siraLog.Warning($"Judgment for threshold {judgment.Threshold} has invalid color in {configName}! Make sure to include exactly 4 numbers that are greater or equal than 0 (and preferably smaller or equal than 1) for each judgment's color!");
			return false;
		}

		private bool ValidateJudgmentSegment(List<JudgmentSegment> segments, string configName)
		{
			if (segments.Count <= 1)
			{
				return true;
			}

			var prevJudgementSegment = segments.First();
			for (var i = 1; i < segments.Count; i++)
			{
				var currentJudgement = segments[i];
				if (prevJudgementSegment.Threshold != currentJudgement.Threshold)
				{
					prevJudgementSegment = currentJudgement;
					continue;
				}

				_siraLog.Warning($"Duplicate entry found for threshold {currentJudgement.Threshold} in {configName}");
				return false;
			}

			return true;
		}

		private bool ValidateTimeDependenceJudgmentSegment(List<TimeDependenceJudgmentSegment> segments, string configName)
		{
			if (segments.Count <= 1)
			{
				return true;
			}

			var prevJudgementSegment = segments.First();
			for (var i = 1; i < segments.Count; i++)
			{
				var currentJudgement = segments[i];
				if (prevJudgementSegment.Threshold - currentJudgement.Threshold > double.Epsilon)
				{
					prevJudgementSegment = currentJudgement;
					continue;
				}

				_siraLog.Warning($"Duplicate entry found for threshold {currentJudgement.Threshold} in {configName}");
				return false;
			}

			return true;
		}

		private void RunMigration(Configuration userConfig)
		{
			var userConfigVersion = userConfig.Version;
			foreach (var requiredMigration in _migrationActions.Keys.Where(migrationVersion => migrationVersion >= userConfigVersion))
			{
				_migrationActions[requiredMigration](userConfig);
			}

			userConfig.Version = Plugin.Version;
		}

		private static bool RunMigration2_0_0(Configuration configuration)
		{
			configuration.BeforeCutAngleJudgments = new List<JudgmentSegment> {JudgmentSegment.Default};
			configuration.AccuracyJudgments = new List<JudgmentSegment> {JudgmentSegment.Default};
			configuration.AfterCutAngleJudgments = new List<JudgmentSegment> {JudgmentSegment.Default};

			return true;
		}

		private static bool RunMigration2_1_0(Configuration configuration)
		{
			if (configuration.Judgments != null)
			{
				foreach (var j in configuration.Judgments.Where(j => j.Threshold == 110))
				{
					j.Threshold = 115;
				}
			}

			if (configuration.AccuracyJudgments != null)
			{
				foreach (var aj in configuration.AccuracyJudgments.Where(aj => aj.Threshold == 10))
				{
					aj.Threshold = 15;
				}
			}

			return true;
		}

		private static bool RunMigration2_2_3(Configuration configuration)
		{
			configuration.DoIntermediateUpdates = true;

			return true;
		}

		private bool CreateHsvConfigsFolderIfYeetedByPlayer(bool calledOnInit = true)
		{
			if (!Directory.Exists(_hsvConfigsFolderPath))
			{
				if (!calledOnInit)
				{
					_siraLog.Warning("*sigh* Don't yeet the HSV configs folder while the game is running... Recreating it again...");
				}

				Directory.CreateDirectory(_hsvConfigsFolderPath);

				return true;
			}

			return false;
		}
	}
}