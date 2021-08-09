﻿using System.Collections.Generic;
using Newtonsoft.Json;

namespace HitScoreVisualizer.Settings
{
	public class Judgment
	{
		[JsonIgnore]
		internal static Judgment Default { get; } = new Judgment {Threshold = 0, Text = string.Empty, Color = new List<float> {1f, 1f, 1f, 1f}, Fade = false};

		// This judgment will be applied only to notes hit with score >= this number.
		// Note that if no judgment can be applied to a note, the text will appear as in the unmodded
		// game.
		[JsonProperty("threshold")]
		public int Threshold { get; internal set; }

		// The text to display (if judgment text is enabled).
		[JsonProperty("text")]
		public string Text { get; internal set; }

		// 4 floats, 0-1; red, green, blue, glow (not transparency!)
		// leaving this out should look obviously wrong
		[JsonProperty("color")]
		public List<float> Color { get; internal set; }

		// If true, the text color will be interpolated between this judgment's color and the previous
		// based on how close to the next threshold it is.
		// Specifying fade : true for the first judgment in the array is an error, and will crash the
		// plugin.
		[JsonProperty("fade")]
		public bool Fade { get; internal set; }

		[JsonConstructor]
		internal Judgment(int threshold = 0, string? text = null, List<float>? color = null, bool fade = false)
		{
			Threshold = threshold;
			Text = text ?? string.Empty;
			Color = color ?? new List<float>();
			Fade = fade;
		}
	}
}