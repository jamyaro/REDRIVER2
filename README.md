# REDRIVER2 (Reverse-Engineered Driver 2) Switch

A port of [REDRIVER2](https://github.com/OpenDriver2/REDRIVER2) to Switch homebrew.

## Installation

### Converting assets

First, set up a working install on PC using your copy of Driver 2:

1. Follow the [installation instructions for the PC version](https://github.com/OpenDriver2/REDRIVER2/wiki/Installation-instructions).
1. Test that FMVs play back and that levels load.

You should end up with a DRIVER2 folder that has:
* New folders, like `MAPS`
* `avi` files in the FMV folders

### Installing on Switch
1. Download the latest release
1. Extract to your SD card
1. Copy the DRIVER2 folder from your PC install folder to your SD card at `/switch/redriver2`. **Skip overwriting** existing assets when prompted.

## Gameplay Notes
* Controls use dpad by default. Press - and + to switch to analog mode
* Rebind controls, change resolution etc. by editing config.ini.

## Credits
- **SoapyMan** - lead reverse engineer and programmer
- **Fireboyd78** - code refactoring and improvements
- **Krishty, someone972** - early formats decoding
- **Gh0stBlade** - HLE Emulator code used as a base for Psy-Cross [(link)](https://github.com/TOMB5/TOMB5/tree/master/EMULATOR)
- **Ben Lincoln** - [This Dust Remembers What It Once Was](https://www.beneaththewaves.net/Software/This_Dust_Remembers_What_It_Once_Was.html) (*TDR*)
- **Stohrendorf** - [Symdump](https://github.com/stohrendorf/symdump) utility

AI was used in the development of this port.
