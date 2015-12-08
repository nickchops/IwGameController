#ifndef __Q_GAMECONTROLLER_H
#define __Q_GAMECONTROLLER_H

#include <string>

//Currently using quite crude implementation based on s3eAndroidController
//TODO: Prob ought to expose the IwGameController header directly and then use
//a lua file to create public API, inc using strings for button, axis, type

// tolua_begin

namespace gameController {

enum
{
    typeAny = 0,
    typeAndroidAny,
    typeAndroidGeneric,
    typeAndroidOuyaEverywhere,
    typeAndroidAmazon,
    typeIos,
    typeDesktopHid,
    typeMarmaladeRemote,
    typeNone
};

enum
{
    axisDPadX = 0,
    axisDPadY,
    axisStickLeftX,
    axisStickLeftY,
    axisStickRightX,
    axisStickRightY,
    axisTriggerLeft,
    axisTriggerRight,
    axisMax
};

enum
{
    buttonA = 0,
    buttonB,
    buttonX,
    buttonY,
    buttonDPadCenter,
    buttonDPadUp,
    buttonDPadDown,
    buttonDPadLeft,
    buttonDPadRight,
    buttonDPadTouch,
    buttonShoulderLeft,
    buttonShoulderRight,
    buttonStickLeft,
    buttonStickRight,
    buttonTriggerLeft,
    buttonTriggerRight,
    buttonStart,
    buttonSelect,
    buttonLeftStickUp,
    buttonLeftStickDown,
    buttonLeftStickLeft,
    buttonLeftStickRight,
    buttonRightStickUp,
    buttonRightStickDown,
    buttonRightStickLeft,
    buttonRightStickRight,
    buttonMax
};

enum
{
    propertyPropagateButtonsToKeyboard = 0,
    propertyReportsAbsoluteDPadValues,
    propertyAllowsRotation,
    propertyMax
};

enum
{
    controllerTypeUnknown = 0,
    controllerTypeBasic,
    controllerTypeExtended,
    controllerTypeMicro,
    controllerTypeMax
};

bool isSupported();
bool init(unsigned int type = typeAny);
void terminate();
unsigned int getType();
void startFrame();
int getMaxControllers();
int getControllerCount();
void* getControllerByIndex(unsigned int player);
void* getControllerByPlayer(unsigned int player);
bool getButtonState(void* handle, unsigned int button);
float getAxisValue(void* handle, unsigned int axis);
const char* getButtonDisplayName(unsigned int button);
const char* getAxisDisplayName(unsigned int axis);
int getProperty(void* handle, unsigned int property);
void setProperty(void* handle, unsigned int property, int value);
unsigned int getControllerType(void* handle);
bool isButtonSupported(void* handle, unsigned int button);
bool isAxisSupported(void* handle, unsigned int axis);
void useButtonEvents(bool enabled);

//remote specific
bool connect(bool dontBroadcast=false, const char* appName = "Unknown");
void disconnect();
bool isConnecting();
bool isConnected();
void setConnectTimeout(float seconds);
void setKeepAliveTimeout(float seconds);
void resetValues();
void setIgnoreTimeouts(bool ignore, bool blockSender=false);

}

// tolua_end
#endif // __Q_GAMECONTROLLER_H
