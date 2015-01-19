IwGameController
================

This is a gamepad/controller extension API for the Marmalade C++ and
Marmalade Quick SDKs. It uses a single API, implemented by various
platform-specific extensions.

Currently, it supports:
- Android USB and Bluetooth HID controllers via s3eAndroidController
  - Generic support for 1 controller
  - 4 player support for Amazon Fire TV
- iOS via modern controller API using s3eIOSController
- Windows and Mac desktop HID wired/Bluetooth controllers via s3eHidController

The Quick API is a simple wrapper around the C++ one, with the usual shortened
naming scheme. Quick version supports all the C++ features.

**NB:** This is relatively untested and also the C++ part is a bit overkill
in design (based on IwBilling). I may trim it down to be leaner/faster so API
is subject to change! Likely to also change the Quick part to use strings like
init("any") getButtonState("A") etc.


Requirements and setup for C++ and Quick
----------------------------------------

### Prerequisites

You need the following extensions:

- https://github.com/nickchops/s3eAndroidController
- https://github.com/nickchops/s3eIOSController
- https://github.com/nickchops/s3eHidController

### For Quick only

- Marmalade SDK 7.4 or newer is needed for Quick extension improvements.
   
- You need scripts for rebuilding Quick binaries. Get these from
  https://github.com/nickchops/MarmaladeQuickRebuildScripts Copy those to the
  root *quick* folder in the SDK.

### Setup

You can either add IwGameController ("the module") and the extensions to
default paths or keep them in your github project folder and add that to
your Marmalade search paths. The later is recommended so that you don't
have to do this every time you install a new Marmalade version and can
get Github updates easier.

To copy modules to default paths, put IwGameController in
marmalade-root/modules and the extensions in marmalade-root/extensions.

To add your github root to global search, put the following in
marmalade-root/s3e/s3e-default.mkf:

        options { module_paths="path/to/my/github/projects/root" }

        
## Adding the module to your C++ or Quick project

Add the following to your app project's MKB file:

        subprojects
        {
            IwGameController
        }
   
        deployments
        {
            android-custom-activity='com.s3eAndroidController.s3eAndroidController'
        }

#### Custom activity requirement on Android

Note that we hace to set a custom main activity above.
This is needed in order to catch key and axis events.


Additional setup for Quick only
-------------------------------

For Quick, you need to make a few changes to quick config files and then
rebuild the Quick binaries. Paths here refer to marmalade-root/quick.

1. Edit quick/quickuser_tolua.pkg and add this new line:

        $cfile "path/to/projects/IwGameController/quick/QGameController.h"

2. Edit quick/quickuser.mkf and add the following to the 'subprojects' block:

        subprojects
        {
            IwGameController/quick/QGameController
        }
        
   This allows C++ parts of the module to be built into Quick's binaries.
   Make sure the parent folder of IwGameController is in global paths
        
3. Run quick/quickuser_tolua.bat to generate Lua bindings.

4. Rebuild the Quick binaries by running the scripts (build_quick_prebuilt.bat
   etc.)


Using the C++ API
-----------------

The C++ API uses static functions in a namespace. No need to initialise
classes (done internally); instead you must call Init before other functions.

Type button and axis values are enums:

- IwGameController::Type::eType
- IwGameController::Button::eButton
- IwGameController::Axis::eAxis

See IwGameController.h for type/button/axis values and additional functions.

**void    IwGameController::IsAvailable(type)**

- Check controllers are supported on device. Can ask for particular type or
  leave blank for any type.

**void    IwGameController::Init(type)**

- Initialise system. Must be called before any following functions. By default
  it uses the current platform's extension, or the "best" (most features/device
  OEM)available if platform supports many types.

**void    IwGameController::StartFrame()**

- You must call this at the start of each update loop so that the get
  button/axis values are updated

**bool    IwGameController::SelectControllerByPlayer(int player)**

- Select controller by Player (1-4) to get states for

**int     IwGameController::GetPlayerCount()**

- Get number of players

**bool    IwGameController::GetButtonState(int button)**

- Get button state. True is down/pressed, false is up/released.
  Includes direction pad presses. D-pad and sticks can also
  have centre press states.

**float   IwGameController::GetAxisValue(int axis)**

- Get value from -1 to 1 indicating how far stick/pad is pressed along an axis.
  -1 is left/bottom, 0 centered and 1 is right/top.

**void IwGameController::SetPropagateButtonsToKeyboard(bool propagate)**

- If set true, on controller button presses regular s3eKeyboard events/states
  for the pressed button will still occur. This only affects platforms that
  use the same key events internally for keyboard and controller (eg Android)
  If set false, no s3eKeyboard event will happen. Default is true.



Using the Quick API
-------------------

**NB:** If updating an existing project, you'll likely need to delete the
project's 'build_temp' folder. This is so that the Hub will regenerate all the
necessary deployment scripts and include the new extension.

You need to include IwGameController as a subproject so that extension libs get
deployed.

Quick functions behave like their similarly-named C++ versions:

        gameController.isAvailable()
        gameController.init(type)
        gameController.startFrame()
        gameController.selectControllerByPlayer(int player)
        gameController.getMaxControllers()
        gameController.getPlayerCount()
        gameController.getButtonState(int button)
        gameController.getAxisValue(int axis)

See quick/QGameController.h for types, axes and buttons. Use like this:

        gameController.any
        gameController.androidAny
        gameController.axisStickLeftX
        gameController.buttonA
        etc

TODO: I'll probably change these to strings "any" "a" "stickLeftX" etc
since Lua can do fast string compares (copmpares object address).


Issues with clashing custom activities on Android
-------------------------------------------------

NB: The Android extension (s3eAndroidController) overrides the main Android
activity. You can only have one custom activity set like this. If already
using another extensions that requires a custom activity, you will have to
edit one to inherit and daisy-chain off the other. For example, edit
s3eAndroidController/source/android/IwGameControllerActivity.java
so that the activity *imports* and *extends* your other existing activity
instead of LoaderActivity. Then re-build the s3eAndroidController extension.


------------------------------------------------------------------------------------------
(C) 2014 Nick Smith.

All code is provided under the MIT license unless stated otherwise:

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
