# FlightPlugin
## Introduction
This is a plugin for [BakkesMod](https://bakkesmod.com/), a [Rocket League](https://www.rocketleague.com/) mod. It provides a new set of aerodynamic forces to the base game such as lift, drag, and tail-fin stabilization. It works in training, custom training, workshop maps, and private server matches using the BakkesMod `host` command and offers a lot of customizability with adjustable sliders and presets.

| Without FlightPlugin | With FlightPlugin | 
| ------------- |:-------------:|
| ![Fall demonstration without FlightPlugin.](./noplugin.gif)      | ![Fall demonstration with FlightPlugin.](./flightplugin.gif) |


## Installation
After installing BakkesMod and Rocket League, go into this repository's [releases](https://github.com/Archaic44/FlightPlugin/releases) and download the `flightplugin.dll` and `flightplugin.set` files. Place the `flightplugin.dll` and `flightplugin.set` files in these folders, respectively:
- \Program Files (x86)\Steam\steamapps\common\rocketleague\Binaries\Win32\bakkesmod\plugins
- \Program Files (x86)\Steam\steamapps\common\rocketleague\Binaries\Win32\bakkesmod\plugins\settings

## Using FlightPlugin
In Rocket League, with BakkesMod injected, open the BakkesMod GUI with `F2`. On the far right tab, `Plugins`, click on `Flight Plugin v1.0.0` in the side bar.

With `Flight Plugin v1.0.0` selected, load the plugin and enable it with the `Load Plugin` and `Enable Flight` buttons, respectively. 

#### Select a Preset
To select a desired preset for the slider values, select the dropdown tab labled `Flight Presets`. This dropdown menu will a scrollable list of presets. Base presets will be listed at the top in brackets and user presets will be listed below in the order of their creation below.
#### Delete or Create a Preset
To delete a user preset, begin by typing into the field labeled `[Name of Preset]`. Click the `Delete Preset` button. Base presets (that come with flightplugin.dll) cannot be deleted and are wrapped in brackets.

To create a user preset, begin by adjusting the slider values below to your personal preference. Type your desired name for the preset into the field labeled `[Name of Preset]`. Click the `Create Preset` button. 
*Note: Using an existing user preset name will overwrite that preset with the current slider values. A user preset name cannot overwrite a base preset name.*
#### Slider Values Meaning
| Slider Name | Effect |
| ------------- |-------------|
|Max Speed|Multiplies the car's default top speed by this factor.|
|Boost Power|Multiplies the default boost strength by this factor.|
|Air Density|The density of air used to calculate aerodynamic forces.|
|Length Width Height|An imaginary hitbox assigned to the player's car for calculating aerodynamic forces.|
|Drag|The air resistance force applied opposite the car's velocity.|
|Stabilization|The strength of the torque applied to straighten the car in the direction of movement.|
|Lift|The strength of the lift gained by the car's imaginary wings + lift-induced drag.|
|Air Throttle|Multiplies the base games throttle when airbourne by this factor.|


## Contributing
You must have BakkesMod downloaded (this plugin requires the Bakkesmod SDK and lib files). Visual Studio is the recommended IDE for compiling the `.dll` files which must be set to Release and x86.

Set these environment variables in your OS (where `Drive` is the disk partition with Rocket League & BakkesMod installed):
- `BakkesSDK` : `Drive:\Program Files %28x86%29\Steam\steamapps\common\rocketleague\Binaries\Win32\bakkesmod\bakkesmodsdk\include\`
- `BakkesLIB` : `Drive:\Program Files %28x86%29\Steam\steamapps\common\rocketleague\Binaries\Win32\bakkesmod\bakkesmodsdk\include\lib\`

*Note:* Your `Program Files (x86)` contains parenthesis that must be encoded in Unicode literals: `%28 = (` `%29 = )` as in the above example.

Reopening Visual Studio should refresh the OS environment variables. Fork this project, commit and push your changes to a separate branch, and submit a pull-request.

If you have any questions, feel free to reach out to us on Github or in the [BakkesMod Discord](https://discord.gg/9PtUKcj) (@Digby Deathstar#1307 @jsonV#2494).
