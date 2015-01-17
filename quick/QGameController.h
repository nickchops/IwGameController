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
    any = 0,
    androidAny,
    androidGeneric,
    androidOuyaEverywhere,
    androidAmazon,
    ios,
    desktopHid,
    none
};

enum
{
    axisStickLeftX = 0,
    axisStickLeftY,
    axisStickRightX,
    axisStickRightY,
    axisTriggerLeft,
    axisTriggerRight,
    max
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
    max
};

bool isAvailable();
bool init(int type);
void startFrame();
bool selectControllerByPlayer(int player);
int getMaxControllers();
int getPlayerCount();
bool getButtonState(int button);
float getAxisValue(int axis);
char* getButtonDisplayName(int button);
char* getAxisDisplayName(int axis);
void setPropagateButtonsToKeyboard(bool propagate);
void useButtonEvents(bool enabled);

}

// tolua_end
#endif // __Q_GAMECONTROLLER_H
