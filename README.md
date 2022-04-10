# HitScoreVisualizer

A fairly simple mod that allows you to heavily customize the appearance of hit scores.

## Usage

Having it all installed, you might wonder how you actually use this version. The first important thing that you'll need to know is that it has support for multiple configs which means that you won't have to restart your game in order to change a config.

On the first run, it will create a new folder `/sdcard/ModData/com.beatgames.beatsaber/Mods/HitScoreVisualizer` (and a default config for those who didn't have one yet). In that folder, you can drop all your HSV config files. It doesn't even matter if you create new folders in that folder, HSV will still be able to find them all. One remark about this though... while you can have config files with the same name when using folder structures, **I still strongly advise you to use unique config filenames** because, despite HSV being able to handle them just fine, config
files will appear in-game with only their filename, and you might end up with several files in the list that appear identical.

Having placed your config files in the folder doesn't mean you're ready to go yet as you still need to select it in-game.

## How-To Config? (aka config explained)

When first running the game, it will create a default config which can be found at `/sdcard/ModData/com.beatgames.beatsaber/Mods/HitScoreVisualizer/HitScoreVisualizerConfig (default).json`.<br>
You can use that file as a starting point in case you want to customize it. Just make sure to set `isDefaultConfig` to `false`.

| Property name(s) | Explanation / Info | Example or possible values |
| --- | --- | --- |
| isDefaultConfig | Indicates whether this file is a default config file. This also means that it will likely be overwritten by the built-in default config. | true or false |
| displayMode | - Currently unsupported, will always be the equivalent of "format" on PC. | "format"<br>"numeric"<br>"textOnly"<br>"scoreOnTop"<br>"" |
| fixedPosition| The coordinate object that defines the fixed location where the hit scores have to be shown.<br>Either leave this out or set as `null` to fully disable. | {<br>"x": 0.0,<br>"y": 3.0,<br>	"z": -2.0<br>}<br><br>null |
| targetPositionOffset| The coordinate object that indicates how much the hitscore fade animation target position has to be offset.<br>Note: If a fixed position is defined in the config, that one will take priority over this one and this will be fully ignored.<br>Either leave this out or set as `null` to fully disable.  | {<br>"x": 0.0,<br>"y": 3.0,<br>	"z": -2.0<br>}<br><br>null |
| timeDependencyDecimalPrecision | The number of decimal places to show the time dependence to.<br> **Must be between 0 and 99, inclusive** | ints |
| timeDependencyDecimalOffset | Which power of 10 to multiply the time dependence by (time dependence is from 0 - 1).<br> **Must be between 0 and 38, inclusive**  | ints |
| Judgments | The list of Judgments that can be used to customize the udgments in general.  | Uses Judgment objects.<br>More info below. |
| beforeCutAngleJudgments | The list that can be used to customize the Judgments for the part of the swing before cutting the block (score is from 0 - 70).<br>Format token: %B<br> | Uses JudgmentSegments objects.<br>More info below. |
| accuracyJudgments | The list that can be used to customize the Judgments for the accuracy of the cut. How close was the cut to the center of the block? (score is from 0 - 15).<br>Format token: %C<br> | Uses JudgmentSegments objects.<br>More info below. |
| afterCutAngleJudgments | The list that can be used to customize the Judgments for the part of the swing after cutting the block (score is from 0 - 30).<br>Format token: %A<br> | Uses JudgmentSegments.<br>More info below. |
| timeDependencyJudgments | The list that can be used to customize the Judgments for the time dependence (value is from 0 - 1).<br>Format token: %T<br> | Uses TimeDependenceJudgmentSegments.<br>More info below. |
| chainHeadJudgments | The list that can be used to customize the Judgments for the new chain block heads. | Uses Judgment objects. More info below.
| chainLinkDisplay | The judgment that will display when hitting the links of the chain block. `threshold` and `fade` don't do anything here because the links are a static score. | Uses Judgment objects. More info below.
### Important info

- The `text` property of Judgment, JudgmentSegment, and TimeDependenceJudgmentSegment all have support for [TextMeshPro formatting!](http://digitalnativestudios.com/textmeshpro/docs/rich-text/).
- The order of Judgments and JudgmentSegments in the list doesn't matter anymore from version 3.0.0 and onwards. However, it's still advised to keep a descending order if you plan on targetting older versions of HSV as well.
- `chainHeadJudgments` and `chainLinkDisplay` are not required in configs for backwards compatibility, and configs without them will simply not affect the displayed scores for those types of notes.

### Format tokens

| Token | Explanation / Info |
| --- | --- |
| %b | The score contributed by the swing before cutting the block. |
| %c | The score contributed by the accuracy of the cut. |
| %a | The score contributed by the part of the swing after cutting the block. |
| %t | The time dependence of the swing. This value indicates how depedent the accuracy part of the score is upon *when* you hit the block, measured from 0 - 1. A value of 0 indicates a completely time independent swing, while a value of 1 indicates that the accuracy part of the score would vary greatly if the block was hit even slightly earlier or later.
| %B, %C, %A, %T | Uses the Judgment text that matches the threshold as specified in either `beforeCutAngleJudgments`, `accuracyJudgments`, `afterCutAngleJudgments`, or `timeDependencyJudgments` (depending on the used token). |
| %s | The total score of the cut. |
| %p | The percentage of the total score of the cut out of the maximum possible. |
| %% | A literal percent symbol. |
| %n | A newline. |

### Judgments explanation

| Property name(s) | Explanation / Info | Example or possible values |
| --- | --- | --- |
| threshold | The threshold that defines whether this Judgment will be used for a given score. The Judgment will be used if it is the one with the highest threshold that's either equal or smaller than the score the given hit score. It can also be omitted when it's the Judgment for the lowest scores. | 110 |
| text | The text to display (if Judgment text is enabled). This can contain the formatting tokens (explained later on) if `displayMode` is set to `"format"` | "%BFantastic%A%n%s" |
| color | An array that specifies the color. Consists of 4 floating numbers ranging between (inclusive) 0 and 1. Red, Green, Blue and Glow. | [0, 0.5, 1, 0.75] |
| fade | If true, the text color will be interpolated between this Judgment's color and the Judgment with the next highest threshold based on how close to the next threshold it is. | true or false |

### JudgmentSegments explanation

| Property name(s) | Explanation / Info | Example or possible values |
| --- | --- | --- |
| threshold | The threshold that defines whether this JudgmentSegment will be used for a given score. The JudgmentSegment will be used if it is the one with the highest threshold that's either equal or smaller than the score of the given part. It can also be omitted for the JudgmentSegment for the lowest scores. | 30 |
| text | The text to display.<br>Remark: Format tokens can't be used in this text... or better, they can be used, but won't be replaced with the actual value. | "+++" |

### TimeDependenceJudgmentSegments explanation

| Property name(s) | Explanation / Info | Example or possible values |
| --- | --- | --- |
| threshold | The threshold that defines whether this TimeDependenceJudgmentSegment will be used for a given time dependence. The TimeDependenceJudgmentSegment will be used if it is the one with the highest threshold that's either equal or smaller than the given time dependence. It can also be omitted for the TimeDependenceJudgmentSegment for the lowest time dependences. | 0.5 |
| text | The text to display.<br>Remark: Format tokens can't be used in this text... or better, they can be used, but won't be replaced with the actual value. | "+++" |

## Useful links

[HSV Config Creator by @MoreOwO](https://github.com/MoreOwO/HSV-Config-Creator/releases/latest): A program that helps you create configs for HSV.

## Credits

Credit where credit is due:
* [@ErisApps](https://github.com/ErisApps) for maintaining the PC mod
* [@Metalit](https://github.com/Metalit) for doing a lot of the work
* [zoller27osu](https://github.com/zoller27osu), [Sc2ad](https://github.com/Sc2ad) and [jakibaki](https://github.com/jakibaki) - [beatsaber-hook](https://github.com/sc2ad/beatsaber-hook)
* [raftario](https://github.com/raftario)
* [Lauriethefish](https://github.com/Lauriethefish) and [danrouse](https://github.com/danrouse) for [this template](https://github.com/Lauriethefish/quest-mod-template)
* Everyone in #quest-mod-dev
