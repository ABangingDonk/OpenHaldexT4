
# OpenHaldexT4

## Brief
Gain control over the [Gen 1 Haldex](https://www.awdwiki.com/en/haldex/#haldex_generation_I__1998_2001) equipped to Quattro and 4Motion vehicles based on the Mk. 4 VW Golf platform (8N Audi TT, 8L Audi A3, 1J VW Golf, etc.)

[Demo video](https://youtu.be/rtE-rsQuckA)

[Ordering form](https://abangingdonk.github.io/)

Installation notes here are for a Mk1 Audi TT.

## App notes

### App install & pairing
- Download [OpenHaldex apk](https://github.com/ABangingDonk/OpenHaldexApp/releases/latest), copy it to your Android device and install it
- It will probably complain about installing apps from unknown sources and/or Google's Play Protect not knowing who I am.. you'll have to click through all the warnings
- As long as the HC-05 BT module is powered, configured and within range (5-10 metres), the app will request bluetooth permissions and try to pair when you press the connect button within the app
- I recommend **not** pairing with the OpenHaldex module through your phone's menus first.. let the app do it

### Basic usage
- Comes with three pre-loaded modes which cannot be deleted or edited
    - Stock
    - FWD
    - 50/50
- Minimum pedal threshold can be used to prevent binding during tight low speed turns (such as when parking). I recommend setting this to at least 15% if you're not prepared to switch to stock mode when maneuvering

### Custom modes
- Custom modes that have been added can be edited via long press
- Vehicle speed will be used to lookup the lock target
- If vehicle speed is between two lock points, the lock value will be interpolated
- If vehicle speed is lower than that of the first lock point, the lock value of the first lock point will be used
- If vehicle speed is greater than that of the last lock point, the lock value of the last lock point will be used
- **Lock points are expected to be ordered by speed (ascending.. i.e. lowest to highest). This is not enforced by software and I don't know how the interpolation will behave if this rule is not followed.**
- You can add as many custom modes as you like however they must be given unique names
- Custom modes will be lost if uninstalling the App

## Installation

#### Prep
Program the Teensy using PlatformIO & VS:Code. First, download Visual Studio: Code and clone the OpenHaldexT4 repository (https://github.com/ABangingDonk/OpenHaldexT4.git).

Then install the Platform IO extension for VS:Code. Once this is installed, you should be able to build/upload the code.

#### Install
Take out the spare wheel and toolkit to expose the wiring going down to the Haldex controller

<img src="https://github.com/ABangingDonk/OpenHaldex/blob/master/media/openhaldex_install_boot_location.jpg?raw=true" width="50%"/>

Gently cut the fabric sheath and unravel a 10cm section or so. The wires we're interested in here are:

**CAN high:** solid black (narrow gauge)
**CAN low:** solid white (narrow gauge)
**12V switched:** black/grey
**Ground:** solid brown
**Brake light:** red/brown

Expect the CAN lines to be a (loosely) twisted pair.

Pictured above, the CAN lines on the side that's going into the floor of the boot should be connected to the 'Haldex' pads and the CAN lines on the side that goes to the rear of the car should be connected to the 'Chassis' pads.

Red/brown wire can be disconnected if you wish for haldex to remain engaged during braking (not tried this myself yet)

<img src="https://github.com/ABangingDonk/OpenHaldex/blob/master/media/v0.1_board.jpg?raw=true" width="50%"/>

## Issues
- Haldex behaviour doesn't track the target as closely as I'd like.. more work to be done here
- Board needs some changes... but it's functional