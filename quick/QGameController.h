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
    typeNone
};

enum
{
    axisStickLeftX = 0,
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
    buttonShoulderLeft,
    buttonShoulderRight,
    buttonStickLeft,
    buttonStickRight,
    buttonTriggerLeft,
    buttonTriggerRight,
    buttonStart,
    buttonSelect,
    buttonMax
};

enum
{
    typeUnknown = 0,
    typeBasic,
    typeExtended,
    typeMicro,
    typeMax
};

bool isSupported();
bool init(int type);
void terminate();
void startFrame();
int getMaxControllers();
int getControllerCount();
void* getControllerByIndex(int player);
void* getControllerByPlayer(int player);
bool getButtonState(void* handle, int button);
float getAxisValue(void* handle, int axis);
const char* getButtonDisplayName(int button);
const char* getAxisDisplayName(int axis);
int getProperty(void* handle, char* property);
void setProperty(void* handle, char* property, int value);
int getControllerType(void* handle);
bool isButtonSupported(void* handle, int button);
bool isAxisSupported(void* handle, int axis);
void useButtonEvents(bool enabled);

}

// tolua_end
#endif // __Q_GAMECONTROLLER_H
