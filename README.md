# sc4-custom-ordinance-host

A DLL Plugin for SimCity 4 that allows new ordinances to be defined in ordinance exemplars.

The plugin can be downloaded from the Releases tab: https://github.com/0xC0000054/sc4-custom-ordinance-host/releases

## Ordinance Exemplars

Each ordinance exemplar must have a unique instance id, this is how SC4 will identify the ordinance.
The ordinance exemplar must have an _ExemplarType_ property with the value set to Ordinances.

### Ordinance Exemplar Properties

The documentation is on the [Ordinance Exemplar Properties](docs/Ordinance_Exemplar_Properties.md) page.

The exemplar properties are persisted in the save game, and the exemplar will be ignored once the city has been saved.
Because of this, the ordinance tuning should be done in a new city or in an existing city without saving.

### Example Ordinances

The [examples](examples) directory contains several ordinances that can be used as-is or as a guide for creating your own ordinances.
See the [read-me](examples/README.md) in that directory for more information on those ordinances.

## System Requirements

* Windows 10 or later

The plugin may work on Windows 7 or later with the [Microsoft Visual C++ 2022 x86 Redistribute](https://aka.ms/vs/17/release/vc_redist.x86.exe)
installed, but I do not have the ability to test that.

## Installation

1. Close SimCity 4.
2. Copy `SC4CustomOrdinanceHost.dll` the top-level of the Plugins folder in the SimCity 4 installation directory or Documents/SimCity 4 directory.
3. Start SimCity 4.

### Installing New Ordinances

The ordinance DAT files must be installed in _Documents/SimCity 4/Plugins/140-ordinances_ (or a sub-folder).

## Troubleshooting

The plugin should write a `SC4CustomOrdinanceHost.log` file in the same folder as the plugin.    
The log contains status information for the most recent run of the plugin.

# License

This project is licensed under the terms of the GNU Lesser General Public License version 3.0.    
See [LICENSE.txt](LICENSE.txt) for more information.

## 3rd party code

[gzcom-dll](https://github.com/nsgomez/gzcom-dll/tree/master) - MIT License.    
[Windows Implementation Library](https://github.com/microsoft/wil) - MIT License   
[Frozen](https://github.com/serge-sans-paille/frozen) - Apache 2.0 License   
[SafeInt](https://github.com/dcleblanc/SafeInt) - MIT License   

# Source Code

## Prerequisites

* Visual Studio 2022
* `git submodule update --init`

## Building the plugin

* Open the solution in the `src` folder
* Update the post build events to copy the build output to you SimCity 4 application plugins folder.
* Build the solution

## Debugging the plugin

Visual Studio can be configured to launch SimCity 4 on the Debugging page of the project properties.
I configured the debugger to launch the game in a window with the following command line:    
`-intro:off -CPUCount:1 -CPUPriority:high -w -CustomResolution:enabled -r1920x1080x32`

You may need to adjust the resolution for your screen.
