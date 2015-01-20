/*
 * (C) 2015 Nick Smith.
 */

#include "IwGameController_DesktopHid.h"

namespace IwGameController
{

// ----------- Init terminate etc --------------

CIwGameControllerDesktopHid* CIwGameControllerDesktopHid::_instance = 0;

void CIwGameControllerDesktopHid::Create()
{
    if (_instance == 0)
        _instance = new CIwGameControllerDesktopHid();
}
void CIwGameControllerDesktopHid::Destroy()
{
    if (_instance != 0)
    {
        delete _instance;
        _instance = 0;
    }
}
CIwGameControllerDesktopHid* CIwGameControllerDesktopHid::getInstance()
{
    return _instance;
}

bool CIwGameControllerDesktopHid::Init(Type::eType type)
{
    if (!IwGameController::Init(type))
        return false;

    //extension init here if needed

    return true;
}

void CIwGameControllerDesktopHid::Release()
{
    IwGameController::Release();

    //extension terminate here if appropriate
}


// -------- Extension wrapping functions ---------------


void CIwGameControllerDesktopHid::StartFrame()
{
    s3eHidControllerUpdate();
}

bool CIwGameControllerDesktopHid::SelectControllerByPlayer(int player)
{
    return player == 1;
}

int CIwGameControllerDesktopHid::GetPlayerCount()
{
    return 1;
}

int CIwGameControllerDesktopHid::GetMaxControllers()
{
    return 1;
}

bool CIwGameControllerDesktopHid::GetButtonState(Button::eButton button)
{
    switch (button)
    {
        case Button::A:
            return s3eHidControllerGetButtonA();
        case Button::B:
            return s3eHidControllerGetButtonB();
        case Button::X:
            return s3eHidControllerGetButtonX();
        case Button::Y:
            return s3eHidControllerGetButtonY();
        case Button::DPAD_UP:
            return s3eHidControllerGetButtonDPadUp();
        case Button::DPAD_DOWN:
            return s3eHidControllerGetButtonDPadDown();
        case Button::DPAD_LEFT:
            return s3eHidControllerGetButtonDPadLeft();
        case Button::DPAD_RIGHT:
            return s3eHidControllerGetButtonDPadRight();
        case Button::SHOULDER_LEFT:
            return s3eHidControllerGetLeftTrigger() >= 0.95; //allow for trigger sensitivity
        case Button::SHOULDER_RIGHT:
            return s3eHidControllerGetRightTrigger() >= 0.95;
        case Button::TRIGGER_LEFT:
            return s3eHidControllerGetButtonLShoulderDown();
        case Button::TRIGGER_RIGHT:
            return s3eHidControllerGetButtonRShoulderDown();
        case Button::START:
            return s3eHidControllerGetButtonStart();
        case Button::SELECT:
            return s3eHidControllerGetButtonSelect();
        default:
            return false;
    }
}

float CIwGameControllerDesktopHid::GetAxisValue(Axis::eAxis axis)
{
    switch (axis)
    {
        case Axis::STICK_LEFT_X:
            return s3eHidControllerGetStick1XAxis();
        case Axis::STICK_LEFT_Y:
            return s3eHidControllerGetStick1YAxis();
        case Axis::STICK_RIGHT_X:
            return s3eHidControllerGetStick2XAxis();
        case Axis::STICK_RIGHT_Y:
            return s3eHidControllerGetStick2YAxis();
        case Axis::TRIGGER_LEFT:
            return s3eHidControllerGetLeftTrigger();
        case Axis::TRIGGER_RIGHT:
            return s3eHidControllerGetRightTrigger();
        default:
            return false;
    }
}

void CIwGameControllerDesktopHid::SetPropagateButtonsToKeyboard(bool propagate)
{
}

}   // namespace IwGameController
