IwGameController
================

This is a gamepad/controller extension API for the Marmalade SDK (C++ and
Quick/Lua). It uses a single API, implemented by various platform-specific
extensions.

Currently, it supports:

- Android:
  - USB and Bluetooth HID controllers via s3eAndroidController
  - Generic support for 1 controller
  - 4 player support on Amazon Fire TV
- iOS:
  - Uses Apple's Game Controller API via s3eIOSController (wifi/bluetooth MFi)
  - Multiple controller support (4 player currently)
- Apple TV (tvOS):
  - Uses same s3eIOSController backed as iOS
  - Up to two controllers
  - Supports Siri Remote (micro cotroller type) and MFi game pads
  - Needs Marmalade SDK that supports Apple TV
- Windows and Mac desktop HID wired/Bluetooth controllers via s3eHidController
  - Single controller only currently

The Quick API is a  wrapper around the C++ one, with the usual shortened
naming scheme. Quick supports all the C++ features but not the remote app yet.


Apple TV (tvOS) caveats
-----------------------

You currently need to define SDK_SUPPORTS_TVOS in you project to support it.
This is because the tvOS MKB options are not supported on older SDKs.

For tvOS, *any* controller will return true for the DPAD_TOUCH button state
if one remote has its pad touched. This is because those events come through
a separate global API (pointer/touch) and are not tied to the controller.
Check the type of the controller before checking its DPAD_TOUCH state.


Requirements and setup for C++ and Quick
----------------------------------------

### Prerequisites

You need the following extensions:

- https://github.com/nickchops/s3eAndroidController
  - https://github.com/nickchops/s3eAndroidUserActivity (needed by
    s3eAndroidController, see its readme)
- https://github.com/nickchops/s3eIOSController
- https://github.com/nickchops/s3eHidController

Check the s3eAndroidController readme for info on custom activity setup.


#### For Quick only

- Marmalade SDK 7.7 or newer for Marmalade Quick extension improvements.


### Add the module and extensions to your Marmalade search path

You can either add IwGameController ("the module") and the extensions above to
default paths or keep them in your github project folder and add that to
your Marmalade search paths. The later is recommended so that you don't
have to do this every time you install a new Marmalade version and can
get Github updates easier.

To add your github root to global search, put the following in
marmalade-root/s3e/s3e-default.mkf:

        options { module_path="path/to/my/github/projects/root" }

Alternatively, to copy modules to default paths, put IwGameController in
marmalade-root/modules and the extensions in marmalade-root/extensions.


Additional setup for Quick only
-------------------------------

For Quick, you need to make changes to two quick config files and then
rebuild the Quick binaries. Paths here refer to < marmalade-root >/quick.

1. Edit quick/quickuser_tolua.pkg and add this new line:

        $cfile "path/to/projects/IwGameController/quick/QGameController.h"

2. Edit quick/quickuser.mkf and add the following to the 'subprojects' block:

        subprojects
        {
            IwGameController/quick/QGameController
        }
        
   Make sure the parent folder of IwGameController is in global paths
        
3. Run quick/quickuser_tolua.bat to generate Lua bindings.

4. Rebuild the Quick binaries by running the scripts (build_quick_prebuilt.bat
   etc.)


Add the module to your C++ or Quick app/game project
----------------------------------------------------

You must add the following to your app project's MKB file:

        subprojects
        {
            IwGameController
        }

#### NB: Custom activity requirement on Android!

Note that for Android we have to set a custom main activity as shown above.
This is needed in order to catch key and axis events.


Using the C++ API
-----------------

All functions and classes are in the IwGameController namespace.

The C++ API uses a generic CIwGameController class which represents an underlying controller API (iOS, Android, etc) and a CIwGameConstrollerHandle opaque pointer
which represents an actual connected controller.

Each type of controller (iOs, Android, etc) has its own sub-class, e.g.
CIwGameControllerAndroid. You can include the header for these (e.g. IwGameController_Android.h) and instantiate these direclty with new.

Or you can use the static Create() fucntion in IwGameController_Any.h
to return a generic CIwGameController object. By default, Create will
instantiate the relevant type for the platform.

Type button and axis values are enums:

- IwGameController::Type::eType
- IwGameController::Button::eButton
- IwGameController::Axis::eAxis

See IwGameController.h for type/button/axis values and additional functions.

Static namespace functions from IwGameController_Any.h

**void IsSupported(type)**

-  Check if controllers are supported on device.
  Can ask for particular type or leave blank for any type.

**void Create(type)**
  Creates a CIwGameController subclass that uses the default extension and device
  type for the OS the app is running on, or the "best" (most features/device
  OEM) available if the platform supports many types.

  Alternatively you can instantiate a specific type using, for exmaple:
  new CIwGameControllerIOS()

Calls below are methods of instantiated CIwGameController objects:

**CIwGameControllerHandle* controller->GetControllerByIndex(int index)**

- Select controller by index (1-4 usually) This returns a handle that must be
  passed to most other functions to query states.

**int controller->GetControllerCount()**

- Get number of controllers

**bool controller->GetButtonState(CIwGameControllerHandle handle, Button::eButton button)**

- Get button state. True is down/pressed, false is up/released.
  Includes direction pad presses. D-pad and sticks can also
  have centre press states.

**float controller->GetAxisValue(CIwGameControllerHandle handle, Axis::eAxis axis)**

- Get value from -1 to 1 indicating how far stick/pad is pressed along an axis.
  -1 is left/bottom, 0 centered and 1 is right/top.

  
## The MarmaladeRemote backend

The CIwGameControllerMarmaladeRemote class from
IwGameController_MarmaladeRemote.h allows another Marmalade built app to
act as a controller itself.

This works via UDP socket connection on local WiFi using ZeroConf/Bonjour for
connection. Essentially, you use a remote app as a controller; it finds an app
using IwGameController via ZeroConf and then streams button/axis data over UDP.
UDP is used as we care more getting data fast that about loosing some data.

CIwGameControllerMarmaladeRemote objects have an adidtional Connect() function
to find remote apps and listen for data, turning it into the same button/axis
events shared by other CIwGaeController classes.

An example "remote app" is provided in the GameControllerRemoteApp folder.
This provides on screen buttos to mimic real controllers. The
IwGameController/example app is set up to detect a remote app and connect it as
a second controller.

Currently the remote app mimics an Apple TV Siri Remote by sending DPAD axis
values and A, X and START button states. It would be easy to extend to support
any buttons and axes.

CIwGameControllerMarmaladeRemote only supports one controller at the moment.
it could be extended to support more.

In order to connect, both apps need to be on a platform that is supported by
s3eZeroConf. Currently that's iOS, tvOS and Windows Desktop. This means you can
for example connect two PCs or an iPhone and a PC (nce for debugging). You
can't have both apps running on the same PC (can't share same socket
address/port combo) Hopefully somone will extend s3eZeroConf to Android and Mac
soon...

You can bypass ZeroConf by specifying the target/receiver IP address explicitly.
This would allow the apps to run on any devices with WiFi, but the user would
need to enter the address manually.


Using the Quick API
-------------------

**NB:** If updating an existing project, you'll likely need to delete the
project's 'build_temp' folder. This is so that the Hub will regenerate all the
necessary deployment scripts and include the new extension.

You need to include IwGameController as a subproject so that extension libs get
deployed.

Quick functions behave like their similarly-named C++ versions:

        gameController.isSupported()
        gameController.init(type)
        gameController.startFrame()
        handle = gameController.getControllerByIndex(int index)
        gameController.getMaxControllers()
        gameController.getButtonState(handle, button)
        gameController.getAxisValue(handle, axis)
        etc

gameController is a static API rather than using objects. You need to call
init() before other functions or they will just return nil.

Handles are opaque userdata types. Just pass them around - you cant directly
alter them.

Buttons, axis etc are all numbers

See quick/QGameController.h for types, axes and buttons. Use like this:

        gameController.any
        gameController.androidAny
        gameController.axisStickLeftX
        gameController.buttonA
        etc


TODO
----

MarmaladeRemote backend needs updating to support multiple controllers.

s3eZeroConf needs extending to support Android and Mac OS X.

The Quick wrapper needs updating to support MarmaladeRemote types.

I'll probably change Quick identifiers to strings ("any" "a" "stickLeftX" etc.)
since Lua can do fast string compares (compares object addresses).


Issues with clashing custom activities on Android
-------------------------------------------------

NB: The Android extension (s3eAndroidController) overrides the main Android
activity. You can only have one custom activity set like this. If already
using another extensions that requires a custom activity, you will have to
edit one to inherit and daisy-chain off the other. For example, edit
s3eAndroidController/source/android/IwGameControllerActivity.java
so that the activity *imports* and *extends* your other existing activity
instead of LoaderActivity. Then re-build the s3eAndroidController extension.


------------------------------------------------------------------------------
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
