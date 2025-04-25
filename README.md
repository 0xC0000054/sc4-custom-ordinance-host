# sc4-custom-ordinance-host

A DLL Plugin for SimCity 4 that allows new ordinances to be defined in ordinance exemplars.

The plugin can be downloaded from the Releases tab: https://github.com/0xC0000054/sc4-custom-ordinance-host/releases

## Ordinance Location

The ordinance DAT files must be installed in the root (or a sub-folder) of _Documents/SimCity 4/Plugins/140-ordinances_ for the DLL to find them.    

## Ordinance Exemplar Properties

The documentation is on the [Ordinance Exemplar Properties](docs/Ordinance_Exemplar_Properties.md) page.

## System Requirements

* Windows 10 or later

The plugin may work on Windows 7 or later with the [Microsoft Visual C++ 2022 x86 Redistribute](https://aka.ms/vs/17/release/vc_redist.x86.exe)
installed, but I do not have the ability to test that.

## Installation

1. Close SimCity 4.
2. Copy `SC4CustomOrdinanceHost.dll` into the Plugins folder in the SimCity 4 installation directory.
3. Start SimCity 4.

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
