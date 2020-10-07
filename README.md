# HitScoreVisualizer

A fairly simple mod that allows you to heavily customize the appearance of hit scores.


## Installation

This mod requires a few other mods in order to work.

- BeatSaberMarkupLanguage v1.3.4 or higher
- BSIPA v4.0.5 or higher
- SiraUtil v1.0.2 or higher

Installation is fairly simple.

1. Grab the latest plugin release from BeatMods/ModAssistant (once it's available) or from the [releases page](https://github.com/ErisApps/HitScoreVisualizer/releases) (once there is actually a
   release)
2. Drop the .dll file in the "Plugins" folder of your Beat Saber installation.
3. Boot it up (or reboot)


## Usage

Having it all installed, you might wonder how you actually use this version compared to the original one. The first important thing that you'll need to know, is that it now has support for multiple
configs which means that you won't have restart your game anymore in order to change a config.

On the first run, it will create a new folder `UserData\HitScoreVisualizer` (and a default config for those who didn't have one yet).
In that folder, you can drop all your HSV config files. It doesn't even matter if you create new folders in that folder, HSV will still be able to find them all. One remark about this though... while
you can have config files with the same name when using folder structures, **I still strongly advise you to use unique config files** because, despite HSV being able to handle them just fine, config
files will appear in-game with only their filename, and you might end up with several files in the list that have the same one.

Having dropped your config files in the folder doesn't mean you're ready to go yet as you still need to select it in-game.
It's pretty straight-forward to do so, but follow the steps below if you really want to be sure.

1) You'll need to start up Beat Saber (you don't have to restart it if it was still running though)
2) Select HitScoreVisualizer in the mod list on the left side.
3) Push the 'SELECT' button
4) Profit I guess?

As you might have noticed, each config has a text below its name that describes it current state. You can find all the possible config states in the table below.

| State (internal) | Text shown in-game | Description |
| --- | --- | --- |
| Broken | Invalid config. Not selectable... | Means that the file cannot be loaded, it's either not a valid hsv config file or not a config file at all. |
| Incompatible | Config is too old. Targets version {Version} | The config is too old and therefore can't be migrated to the newer format automagically. |
| ValidationFailed | Validation failed, please check the file again. | You made an error in the config itself, you can check the logs to see what might be wrong (will be logged as a warning). |
| NeedsMigration | Config made for HSV {Version}. Migration possible. | The config is valid, but still needs a migration to the newer format, this can be done automagically though. |
| Compatible | OK - {Version} | The config file is just fine, nothing special needs to happen. Shows the version for which it was originally made. |
| NewerVersion | Config is too new. Targets version {Version} | The config file is made for a newer version of HSV and therefore won't be allowed to be loaded in as it's correct workings cannot be guaranteed. |


## How-To Config? (aka config explained)

When first running the game, it will create a default config which can be found at `UserData\HitScoreVisualizer\HitScoreVisualizerConfig-default.json`.<br>
You can use that file as a starting point in case you want to customize it. Just make sure to set `isDefaultConfig` to `false`.

| Property name(s) | Explanation / Info | Example or possible values |
| --- | --- | --- |
| majorVersion<br>minorVersion<br>patchVersion | Indicates the version of HSV that the config is made for specifically. | N/A |
| isDefaultConfig | Indicates whether this file is a default config file. This also means that it will be overwritten by the built-in default config if migration is required. | true or false |
| displayMode | - "format": Allows you to use format tokens in the judgement texts. (More info later on)<br>- "numeric": Displays only the note score. (Eg: 115)<br>- "textOnly": Displays only the judgment text. (Eg: Purrfect)<br>- "scoreOnTop": Displays both (numeric score above judgment text).<br>- Anything else or empty: Displays both (judgment text above numeric score). | "format"<br>"numeric"<br>"textOnly"<br>"scoreOnTop"<br>"" |
| useFixedPos | Whether or not to display the hit scores on a fixed position. When enabled, this will also hide the previous score if a new one appears. | true or false |
| fixedPosX<br>fixedPosY<br>fixedPosZ| The coordinate where the hit scores should be shown when `useFixedPos` is set to `true` | floats |
| doIntermediateUpdates | When enabled, judgments will be updated multiple times. This will make score popups more accurate during a brief period before the note's score is finalized, at some cost of performance. | true or false |
| judgments | The list of judgments that can be used to customize the judgments in general.  | Uses Judgment objects.<br>More info below. |
| beforeCutAngleJudgments | The list that can be used to customize the judgments for the part of the swing before cutting the block (score is from 0 - 70).<br>Format token: %B<br>* Can only be used when `displayMode` is set to `"format"` | Uses JudgmentSegments objects.<br>More info below. |
| accuracyJudgments | The list that can be used to customize the judgments for the accuracy of the cut. How close was the cut to the center of the block? (score is from 0 - 15).<br>Format token: %C<br>* Can only be used when `displayMode` is set to `"format"` | Uses JudgmentSegments objects.<br>More info below. |
| afterCutAngleJudgments | The list that can be used to customize the judgments for the part of the swing after cutting the block (score is from 0 - 30).<br>Format token: %A<br>* Can only be used when `displayMode` is set to `"format"` | Uses JudgmentSegments.<br>More info below. |


### Important info
- The `text` property of both Judgment and JudgmentSegment also has support for [TextMeshPro formatting!](http://digitalnativestudios.com/textmeshpro/docs/rich-text/).
- The order of judgments and judgmentSegments in the list doesn't matter anymore from version 3.0.0 and onwards. However, it's still advised to keep a descending order if you plan on targetting older versions of HSV as well.
- To prevent unexpected issues down the road, the validation will also check within each list whether there are judgments or judgementSegments that share a threshold. Validation will mark the config is failed if when this is detected.


### Judgments explanation

| Property name(s) | Explanation / Info | Example or possible values |
| --- | --- | --- |
| threshold | The threshold that defines whether this judgment will be used for a given score. The judgment will be used if it is the one with the highest threshold that's either equal or smaller than the score the current score. It can also be omitted when it's the judgment for the lowest scores. | 110 |
| text | The text to display (if judgment text is enabled). This can contain the formatting tokens (explained later on) if `displayMode` is set to `"format"` | "%BFantastic%A%n%s" |
| color | An array that specifies the color. Consists of 4 floating numbers ranging between (inclusive) 0 and 1. Red, Green, Blue and Glow. | [0, 0.5, 1, 0.75] |
| fade | If true, the text color will be lerped between this judgment's color and the judgment with the next highest threshold based on how close to the next threshold it is.<br>Remark: If your config still targets older, but compatible versions, then don't specify it on the judgment with the highest threshold or the game and plugin will be burned to a crisp. This is mitigated from 3.0.0 and onwards. | true or false |


### JudgmentSegments explanation

| Property name(s) | Explanation / Info | Example or possible values |
| --- | --- | --- |
| threshold | The threshold that defines whether this judgmentSegment will be used for a given score. The judgmentSegment will be used if it is the one with the highest threshold that's either equal or smaller than the score the current score. It can also be omitted when it's the judgmentSegment for the lowest scores. | 30 |
| text | The text to display.<br>Remark: Format tokens can't be used in this text... or better, they can be used, but won't be replaced with the actual value. | "+++" |


### Format tokens

| Token | Explanation / Info |
| --- | --- |
| %b | The score contributed by the part of the swing before cutting the block. |
| %c | The score contributed by the accuracy of the cut. |
| %a | The score contributed by the part of the swing after cutting the block. |
| %B, %C, %A | Uses the judgment text that matches the threshold as specified in either `beforeCutAngleJudgments`, `accuracyJudgments` or `afterCutAngleJudgments` (depending on the used token). |
| %s | The total score of the cut. |
| %% | A literal percent symbol. |
| %n | A newline. |


## Developers

To build this project you will need to create a `HitScoreVisualizer/HitScoreVisualizer.csproj.user` file specifying where the game is located:

```xml
<?xml version="1.0" encoding="utf-8"?>
<Project xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
	<PropertyGroup>
		<!-- Change this path if necessary. Make sure it ends with a backslash. -->
		<GameDirPath>D:\Program Files (x86)\Steam\steamapps\common\Beat Saber\</GameDirPath>
	</PropertyGroup>
</Project>
```

### Changelog
x.x.x: Probably created and such, no clue here...<br>
2.0.2: Bug fixes and performance improvements. If you've been noticing lag, try disabling the fade option on all judgments in the config.<br>
2.1.0: Added display mode "format" (see below).<br>
2.1.2: Actually bump version number this time; minor fixes to default config<br>
2.1.3: Minor bug fix<br>
x.x.x: Probably a few more changes and such, no clue here either...<br>
3.0.0: Big rewrite! Support for changing configs at runtime, better validation, ...


### Credits
Credit where credit is due:
 - @artemiswkearney for writing the original mod
 - @AntRazor for the mod idea/request and default config input in the original mod
 - @wulkanat for the default config input in the original mod
 - Everyone in #pc-mod-dev (and the rest of the server) for the love and support. ❤

***

**TO BE YEETED**<br>

Colors the score popups that appear when hitting notes. Also adds judgment text above (or below) the score.

After installing the plugin, run the game once to generate the config file at `UserData/HitScoreVisualizerConfig.json` in your Beat Saber install folder. A graphical editor for this file is in the
works; until then, editing the config is considered an advanced feature, as JSON is not particularly human-friendly. If you want to edit it anyway, here's the documentation:

* Remove the `isDefaultConfig` line. If this option is set to true, your config will get overwritten by the updated default config with every plugin update.
* Valid options for `displayMode` are "numeric" (displays only the score), "textOnly" (displays only the judgment text), "scoreOnTop" (displays the score above the judgment text), "format" (see "
  Formatting" below), or any other string (displays the judgment text above the score).
* Put your judgments in descending order; the first one encountered with `threshold` <= the score earned for a note will be applied.
* Include exactly 4 numbers in each judgment's `color` array. These represent red, green, blue, and a 4th channel which, knowing Beat Saber's shaders, could be glow, transparency, or even both at
  once. Modify the 4th number if you like, but I can't say what effect it will have.
* If you include more or fewer than 4 numbers in a judgment's color array, your entire config will be ignored (but not overwritten).
* If you include the line `"fade": true` in a judgment, the color for scores earning that judgment will be interpolated between that judgment's color and the color of the previous judgment in the
  list, based on how close to the threshold for that judgment the score was. Use this to create a smooth gradient of color if you want one.
* If you enable `fade` for the first judgment in the list, the plugin and Beat Saber itself will quite possibly die in a fire if that judgment is ever earned. Don't enable `fade` for the first
  judgment in the list. You have been warned.
* Judgment text supports [TextMeshPro formatting!](http://digitalnativestudios.com/textmeshpro/docs/rich-text/)
* **Formatting**
  In displayMode "format", a number of HitScoreVisualizer format specifiers are available (as well as TextMeshPro's formatting tools). Your formatted string will replace the score text (so include %s
  if you want to see your score).
	* %b: The score contributed by the part of the swing before cutting the block.
	* %c: The score contributed by the accuracy of the cut.
	* %a: The score contributed by the part of the swing after cutting the block.
	* %B, %C, %A: As above, except using the appropriate judgment from that part of the swing (as configured for "beforeCutAngleJudgments", "accuracyJudgments", or "afterCutAngleJudgments").
	* %s: The total score for the cut.
	* %%: A literal percent symbol.
	* %n: A newline.
* Note that before-swing/accuracy/after-swing judgments don't support color or fading, although you can put TextMeshPro color tags in them if you want to. If you make your own config, feel free to
  share it in #other-files!

Plugin originally requested by @AntRazor on the modding discord. Thanks go to @AntRazor and @wulkanat for input on the default config released with this update, as well as everyone in #mod-development
and the entire server for the love and support. :')
