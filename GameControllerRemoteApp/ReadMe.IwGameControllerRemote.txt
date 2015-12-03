Created: Thu Nov 12 12:58:55 2015
By: Nick Smith
Using Template: 2D Graphics (Iw2DSceneGraph)

Remote controller application. Install and run on a device that
Marmalade supports s3eZeroConf on and then connect to another
s3eZeroConf enabled device running an app that uses
IwGameController. This application can then connect and act
as a controller via touch screen.

Currently the app mimics an Apple TV Siri Remore by providing
a touch pad and click, play/pause and menu buttons. It could
easily be extended to provide any IwGameController button and
axis events.

The UI is hand rolled using the simple TouchComponents classes.
It would be nice to replace this with something better.