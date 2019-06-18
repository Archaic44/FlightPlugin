# FlightPlugin
## Introduction
This is a plugin for [BakkesMod](https://bakkesmod.com/), a [Rocket League](https://www.rocketleague.com/) mod. It provides a new set of flight mechanics to the game such as lift, drag, and stabilization to the base game. It works in training, custom training, and workshop maps.

| Without FlightPlugin | With FlightPlugin | 
| ------------- |:-------------:|
| ![Fall demonstration without FlightPlugin.](./noplugin.gif)      | ![Fall demonstration with FlightPlugin.](./flightplugin.gif) |


### Installation
After installing BakkesMod and Rocket League, go into this repository's [Releases] (https://github.com/Archaic44/FlightPlugin/releases) and download the `flightplugin.dll` and `flightplugin.set` files. Place the `flightplugin.dll` and `flightplugin.set` files in these folders, respectively:
- \Program Files (x86)\Steam\steamapps\common\rocketleague\Binaries\Win32\bakkesmod\plugins
- \Program Files (x86)\Steam\steamapps\common\rocketleague\Binaries\Win32\bakkesmod\plugins\settings

### Using FlightPlugin
In game, open the BakkesMod GUI with `F2`. On the far right tab, `Plugins`, click on `Flight Plugin` in the side bar.

With `Flight Plugin` selected, load the plugin and enable it with the `Load Plugin` and `Enable Flight` buttons, respectively. Play with the sliders at your own risk! To create/delete a preset, type into the field labeled `[Name of Preset` and click the `Create Preset` or `Delete Preset` buttons. Base presets (that come installed onto flightplugin.dll) cannot be deleted and are wrapped in brackets.

## Contributing
You must have BakkesMod downloaded (this plugin requires the Bakkesmod SDK and lib files). Visual Studio is the recommended IDE for compiling the `.dll` files, and must be set to Release and x86.

Set these environment variables in your OS (where `Drive` is the disk partition with Rocket League & BakkesMod installed):
- `BakkesSDK` : `Drive:\Program Files %28x86%29\Steam\steamapps\common\rocketleague\Binaries\Win32\bakkesmod\bakkesmodsdk\include\`
- `BakkesLIB` : `Drive:\Program Files %28x86%29\Steam\steamapps\common\rocketleague\Binaries\Win32\bakkesmod\bakkesmodsdk\include\lib\`

*Note:* Your `Program Files (x86)` contains parenthesis that must be encoded in Unicode literals: `%28 = (` `%29 = )` as in the above example.

Reopening Visual Studio should refresh the OS environment variables. If you have any questions, feel free to reach out to us on Github or in the [BakkesMod Discord](https://discord.gg/9PtUKcj).
