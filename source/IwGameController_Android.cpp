/*
 * (C) 2015 Nick Smith.
 */

#include "IwGameController_Android.h"

#include "IwDebug.h"

namespace IwGameController
{

// ----------- Init terminate etc --------------

CIwGameControllerAndroid* CIwGameControllerAndroid::_instance = 0;

void CIwGameControllerAndroid::Create()
{
    if (_instance == 0)
        _instance = new CIwGameControllerAndroid();
}
void CIwGameControllerAndroid::Destroy()
{
    if (_instance != 0)
    {
        delete _instance;
        _instance = 0;
    }
}
CIwGameControllerAndroid* CIwGameControllerAndroid::getInstance()
{
    return _instance;
}

bool CIwGameControllerAndroid::Init(Type::eType type)
{
    if (!IwGameController::Init(type))
        return false;

    if (type == Type::ANDROID_OUYA_EVERYWHERE)
        s3eAndroidControllerSetType(S3E_ANDROIDCONTROLLER_TYPE_OUYA_EVERYWHERE);
    else if (type == Type::ANDROID_AMAZON)
        s3eAndroidControllerSetType(S3E_ANDROIDCONTROLLER_TYPE_AMAZON);

    return true;
}

void CIwGameControllerAndroid::Release()
{
    IwGameController::Release();

    //extension terminate here if appropriate
}


// -------- Extension wrapping functions ---------------

void CIwGameControllerAndroid::StartFrame()
{
    s3eAndroidControllerStartFrame();
}

bool CIwGameControllerAndroid::SelectControllerByPlayer(int player)
{
    return s3eAndroidControllerSelectControllerByPlayer(player);
}

int CIwGameControllerAndroid::GetPlayerCount()
{
    return s3eAndroidControllerGetPlayerCount();
}

int CIwGameControllerAndroid::GetMaxControllers()
{
    return S3E_ANDROID_CONTROLLER_MAX_PLAYERS;
}

bool CIwGameControllerAndroid::GetButtonState(Button::eButton button)
{
    switch (button)
    {
        case Button::A:
            return s3eAndroidControllerGetButtonState(S3E_ANDROID_CONTROLLER_BUTTON_A);
        case Button::B:
            return s3eAndroidControllerGetButtonState(S3E_ANDROID_CONTROLLER_BUTTON_B);
        case Button::DPAD_CENTER:
            return s3eAndroidControllerGetButtonState(S3E_ANDROID_CONTROLLER_BUTTON_DPAD_CENTER);
        case Button::DPAD_DOWN:
            return s3eAndroidControllerGetButtonState(S3E_ANDROID_CONTROLLER_BUTTON_DPAD_DOWN);
        case Button::DPAD_LEFT:
            return s3eAndroidControllerGetButtonState(S3E_ANDROID_CONTROLLER_BUTTON_DPAD_LEFT);
        case Button::DPAD_RIGHT:
            return s3eAndroidControllerGetButtonState(S3E_ANDROID_CONTROLLER_BUTTON_DPAD_RIGHT);
        case Button::DPAD_UP:
            return s3eAndroidControllerGetButtonState(S3E_ANDROID_CONTROLLER_BUTTON_DPAD_UP);
        case Button::SHOULDER_LEFT:
            return s3eAndroidControllerGetButtonState(S3E_ANDROID_CONTROLLER_BUTTON_SHOULDER_LEFT);
        case Button::SHOULDER_RIGHT:
            return s3eAndroidControllerGetButtonState(S3E_ANDROID_CONTROLLER_BUTTON_SHOULDER_RIGHT);
        case Button::STICK_LEFT:
            return s3eAndroidControllerGetButtonState(S3E_ANDROID_CONTROLLER_BUTTON_STICK_LEFT);
        case Button::STICK_RIGHT:
            return s3eAndroidControllerGetButtonState(S3E_ANDROID_CONTROLLER_BUTTON_STICK_RIGHT);
        case Button::TRIGGER_LEFT:
            return s3eAndroidControllerGetButtonState(S3E_ANDROID_CONTROLLER_BUTTON_TRIGGER_LEFT);
        case Button::TRIGGER_RIGHT:
            return s3eAndroidControllerGetButtonState(S3E_ANDROID_CONTROLLER_BUTTON_TRIGGER_RIGHT);
        case Button::X:
            return s3eAndroidControllerGetButtonState(S3E_ANDROID_CONTROLLER_BUTTON_X);
        case Button::Y:
            return s3eAndroidControllerGetButtonState(S3E_ANDROID_CONTROLLER_BUTTON_Y);
        default:
            return false;
    }
}

float CIwGameControllerAndroid::GetAxisValue(Axis::eAxis axis)
{
    switch (axis)
    {
        case Axis::STICK_LEFT_X:
            return s3eAndroidControllerGetAxisValue(S3E_ANDROID_CONTROLLER_AXIS_STICK_LEFT_X);
        case Axis::STICK_LEFT_Y:
            return s3eAndroidControllerGetAxisValue(S3E_ANDROID_CONTROLLER_AXIS_STICK_LEFT_Y);
        case Axis::STICK_RIGHT_X:
            return s3eAndroidControllerGetAxisValue(S3E_ANDROID_CONTROLLER_AXIS_STICK_RIGHT_X);
        case Axis::STICK_RIGHT_Y:
            return s3eAndroidControllerGetAxisValue(S3E_ANDROID_CONTROLLER_AXIS_STICK_RIGHT_X);
        case Axis::TRIGGER_LEFT:
            return s3eAndroidControllerGetAxisValue(S3E_ANDROID_CONTROLLER_AXIS_TRIGGER_LEFT);
        case Axis::TRIGGER_RIGHT:
            return s3eAndroidControllerGetAxisValue(S3E_ANDROID_CONTROLLER_AXIS_TRIGGER_RIGHT);
        default:
            return false;
    }
}

void CIwGameControllerAndroid::SetPropagateButtonsToKeyboard(bool propagate)
{
    return s3eAndroidControllerSetPropagateButtonsToKeyboard(propagate);
}

}   // namespace IwGameController
