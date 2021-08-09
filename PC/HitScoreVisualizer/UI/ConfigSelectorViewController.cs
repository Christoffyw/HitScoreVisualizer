﻿using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using BeatSaberMarkupLanguage.Attributes;
using BeatSaberMarkupLanguage.Components;
using BeatSaberMarkupLanguage.ViewControllers;
using HitScoreVisualizer.Converters;
using HitScoreVisualizer.Models;
using HitScoreVisualizer.Services;
using HitScoreVisualizer.Settings;
using HMUI;
using IPA.Utilities.Async;
using SiraUtil.Tools;
using Zenject;

namespace HitScoreVisualizer.UI
{
	[HotReload(RelativePathToLayout = @"Views\ConfigSelector.bsml")]
	[ViewDefinition("HitScoreVisualizer.UI.Views.ConfigSelector.bsml")]
	internal class ConfigSelectorViewController : BSMLAutomaticViewController
	{
		private SiraLog _siraLog = null!;
		private ConfigProvider _configProvider = null!;
		private HSVConfig _hsvConfig = null!;

		private ConfigFileInfo? _selectedConfigFileInfo;

		public ConfigSelectorViewController()
		{
			AvailableConfigs = new List<object>();
		}

		[Inject]
		internal void Construct(SiraLog siraLog, HSVConfig hsvConfig, ConfigProvider configProvider)
		{
			_siraLog = siraLog;
			_hsvConfig = hsvConfig;
			_configProvider = configProvider;

			SetMigrationButtonColors();
		}

		[UIValue("migration-button-color-face")]
		internal string QueueButtonColorFace { get; set; } = ButtonColorValueConverter.Convert();

		[UIValue("migration-button-color-glow")]
		internal string QueueButtonColorGlow { get; set; } = ButtonColorValueConverter.Convert();

		[UIValue("migration-button-color-stroke")]
		internal string QueueButtonColorStroke { get; set; } = ButtonColorValueConverter.Convert();

		[UIComponent("configs-list")]
		public CustomCellListTableData? customListTableData;

		[UIValue("available-configs")]
		internal List<object> AvailableConfigs { get; }

		[UIValue("loading-available-configs")]
		internal bool LoadingConfigs { get; private set; }

		[UIValue("has-loaded-available-configs")]
		internal bool HasLoadedConfigs => !LoadingConfigs;

		[UIValue("is-valid-config-selected")]
		internal bool CanConfigGetSelected => _selectedConfigFileInfo?.ConfigPath != _configProvider.CurrentConfigPath && _configProvider.ConfigSelectable(_selectedConfigFileInfo?.State);

		[UIValue("has-config-loaded")]
		internal bool HasConfigCurrently => !string.IsNullOrWhiteSpace(_configProvider.CurrentConfigPath);

		[UIValue("config-loaded-text")]
		internal string LoadedConfigText => $"Currently loaded config: <size=80%>{(HasConfigCurrently ? Path.GetFileNameWithoutExtension(_configProvider.CurrentConfigPath) : "None")}";

		[UIValue("is-config-yeetable")]
		internal bool CanConfigGetYeeted => _selectedConfigFileInfo?.ConfigPath != null && _selectedConfigFileInfo.ConfigPath != _configProvider.CurrentConfigPath;

		[UIAction("config-Selected")]
		internal void Select(TableView _, object @object)
		{
			_selectedConfigFileInfo = (ConfigFileInfo)@object;
			NotifyPropertyChanged(nameof(CanConfigGetSelected));
			NotifyPropertyChanged(nameof(CanConfigGetYeeted));
		}

		[UIAction("toggle-migration")]
		internal void ToggleMigration()
		{
			_hsvConfig.SaveOnMigration = !_hsvConfig.SaveOnMigration;
			SetMigrationButtonColors();
		}

		[UIAction("reload-list")]
		internal async void RefreshList()
		{
			await LoadInternal().ConfigureAwait(false);
		}

		[UIAction("pick-config")]
		internal async void PickConfig()
		{
			if (CanConfigGetSelected)
			{
				await _configProvider.SelectUserConfig(_selectedConfigFileInfo).ConfigureAwait(false);
				await LoadInternal().ConfigureAwait(false);
			}
		}

		[UIAction("unpick-config")]
		internal void UnpickConfig()
		{
			if (HasConfigCurrently)
			{
				UnityMainThreadTaskScheduler.Factory.StartNew(() =>
				{
					if (customListTableData == null)
					{
						_siraLog.Warning($"{nameof(customListTableData)} is null.");
						return;
					}

					customListTableData.tableView.ClearSelection();
				});

				_configProvider.UnselectUserConfig();
				NotifyPropertyChanged(nameof(HasConfigCurrently));
				NotifyPropertyChanged(nameof(LoadedConfigText));
			}
		}

		[UIAction("yeet-config")]
		internal async void YeetConfig()
		{
			if (!CanConfigGetYeeted)
			{
				return;
			}

			_configProvider.YeetConfig(_selectedConfigFileInfo!.ConfigPath);
			await LoadInternal().ConfigureAwait(false);

			NotifyPropertyChanged(nameof(CanConfigGetYeeted));
		}

		protected override async void DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
		{
			base.DidActivate(firstActivation, addedToHierarchy, screenSystemEnabling);

			await LoadInternal().ConfigureAwait(false);
		}

		protected override void DidDeactivate(bool removedFromHierarchy, bool screenSystemDisabling)
		{
			base.DidDeactivate(removedFromHierarchy, screenSystemDisabling);

			AvailableConfigs.Clear();

			_selectedConfigFileInfo = null;
		}

		private async Task LoadInternal()
		{
			if (customListTableData == null)
			{
				_siraLog.Warning($"{nameof(customListTableData)} is null.");
				return;
			}

			if (AvailableConfigs.Count > 0)
			{
				AvailableConfigs.Clear();
			}

			LoadingConfigs = true;
			NotifyPropertyChanged(nameof(LoadingConfigs));
			NotifyPropertyChanged(nameof(HasLoadedConfigs));

			var intermediateConfigs = (await _configProvider.ListAvailableConfigs())
				.OrderByDescending(x => x.State)
				.ThenBy(x => x.ConfigName)
				.ToList();
			AvailableConfigs.AddRange(intermediateConfigs);

			var currentConfigIndex = intermediateConfigs.FindIndex(x => x.ConfigPath == _configProvider.CurrentConfigPath);

			await UnityMainThreadTaskScheduler.Factory.StartNew(() =>
			{
				customListTableData.tableView.ReloadData();
				customListTableData.tableView.ScrollToCellWithIdx(0, TableView.ScrollPositionType.Beginning, false);
				if (currentConfigIndex >= 0)
				{
					customListTableData.tableView.SelectCellWithIdx(currentConfigIndex, true);
				}

				LoadingConfigs = false;
				NotifyPropertyChanged(nameof(LoadingConfigs));
				NotifyPropertyChanged(nameof(HasLoadedConfigs));
				NotifyPropertyChanged(nameof(HasConfigCurrently));
				NotifyPropertyChanged(nameof(LoadedConfigText));
			}).ConfigureAwait(false);
		}

		private void SetMigrationButtonColors()
		{
			QueueButtonColorFace = QueueButtonColorGlow = QueueButtonColorStroke = ButtonColorValueConverter.Convert(_hsvConfig.SaveOnMigration);

			NotifyPropertyChanged(nameof(QueueButtonColorFace));
			NotifyPropertyChanged(nameof(QueueButtonColorGlow));
			NotifyPropertyChanged(nameof(QueueButtonColorStroke));
		}
	}
}