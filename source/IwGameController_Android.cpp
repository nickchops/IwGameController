/*
 * (C) 2015 Nick Smith.
 */

#include "IwGameController_Android.h"

#include "IwDebug.h"

namespace IwGameController
{

    // ----------- Init terminate etc --------------

    CIwGameControllerAndroid::CIwGameControllerAndroid(Type::eType type)
    {
        if (type == Type::ANDROID_OUYA_EVERYWHERE)
            s3eAndroidControllerSetType(S3E_ANDROIDCONTROLLER_TYPE_OUYA_EVERYWHERE);
        else if (type == Type::ANDROID_AMAZON)
            s3eAndroidControllerSetType(S3E_ANDROIDCONTROLLER_TYPE_AMAZON);
        
        //TODO: register for callbacks here. That will init extension if not already inited.
    }

    CIwGameControllerAndroid::~CIwGameControllerAndroid()
    {

    }

    // -------- Extension wrapping functions ---------------

    void CIwGameControllerAndroid::StartFrame()
    {
        s3eAndroidControllerStartFrame();
    }
        
    CIwControllerHandle* CIwGameControllerAndroid::GetControllerByIndex(int index)
    {
        return s3eIOSControllerGetController(index);
    }

    
    CIwControllerHandle* CIwGameControllerAndroid::GetControllerByPlayer(int player)
    {
        if (s3eAndroidControllerSelectControllerByPlayer(player) == false)
            return NULL;
        
        //TODO: create/access controller handle
        return NULL;
    }

    int CIwGameControllerAndroid::GetControllerCount()
    {
        return s3eAndroidControllerGetPlayerCount();
    }

    int CIwGameControllerAndroid::GetMaxControllers()
    {
        return S3E_ANDROID_CONTROLLER_MAX_PLAYERS;
    }
    
    //TODO: for keys that have only button or axis, map to the other type, e.g. dpad button -> axis

    bool CIwGameControllerAndroid::GetButtonState(CIwControllerHandle* handle, Button::eButton button)
    {
        //TODO: use handle to switch controller used. For now just always use controller one
        // or whatver GetControllerByPlayer recently set
        
        //TODO: these should be lookup hash tables
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

    float CIwGameControllerAndroid::GetAxisValue(CIwControllerHandle* handle, Axis::eAxis axis)
    {
        switch (axis)
        {
            case Axis::DPAD_X:
                if (s3eAndroidControllerGetButtonState(S3E_ANDROID_CONTROLLER_BUTTON_DPAD_LEFT))
                    return -1.0;
                if (s3eAndroidControllerGetButtonState(S3E_ANDROID_CONTROLLER_BUTTON_DPAD_RIGHT))
                    return 1.0;
                break;
            case Axis::DPAD_Y:
                if (s3eAndroidControllerGetButtonState(S3E_ANDROID_CONTROLLER_BUTTON_DPAD_DOWN))
                    return -1.0;
                if (s3eAndroidControllerGetButtonState(S3E_ANDROID_CONTROLLER_BUTTON_DPAD_UP))
                    return 1.0;
                break;
            case Axis::STICK_LEFT_X:
                return s3eAndroidControllerGetAxisValue(S3E_ANDROID_CONTROLLER_AXIS_STICK_LEFT_X);
            case Axis::STICK_LEFT_Y:
                return s3eAndroidControllerGetAxisValue(S3E_ANDROID_CONTROLLER_AXIS_STICK_LEFT_Y);
            case Axis::STICK_RIGHT_X:
                return s3eAndroidControllerGetAxisValue(S3E_ANDROID_CONTROLLER_AXIS_STICK_RIGHT_X);
            case Axis::STICK_RIGHT_Y:
                return s3eAndroidControllerGetAxisValue(S3E_ANDROID_CONTROLLER_AXIS_STICK_RIGHT_Y);
            case Axis::TRIGGER_LEFT:
                return s3eAndroidControllerGetAxisValue(S3E_ANDROID_CONTROLLER_AXIS_TRIGGER_LEFT);
            case Axis::TRIGGER_RIGHT:
                return s3eAndroidControllerGetAxisValue(S3E_ANDROID_CONTROLLER_AXIS_TRIGGER_RIGHT);
            default:
                return 0.0;
        }
    }

    bool CIwGameControllerAndroid::IsButtonSupported(CIwControllerHandle* handle, Button::eButton button)
    {
        switch (button)
        {
            case Button::A:
            case Button::B:
            case Button::DPAD_CENTER:
            case Button::DPAD_DOWN:
            case Button::DPAD_LEFT:
            case Button::DPAD_RIGHT:
            case Button::DPAD_UP:
            case Button::SHOULDER_LEFT:
            case Button::SHOULDER_RIGHT:
            case Button::STICK_LEFT:
            case Button::STICK_RIGHT:
            case Button::TRIGGER_LEFT:
            case Button::TRIGGER_RIGHT:
            case Button::X:
            case Button::Y:
                return true;
        }
        
        return false;
    }
    
    bool CIwGameControllerAndroid::IsAxisSupported(CIwControllerHandle* handle, Axis::eAxis axis)
    {
        switch (axis)
        {
            case Axis::DPAD_X:
            case Axis::DPAD_Y:
            case Axis::STICK_LEFT_X:
            case Axis::STICK_LEFT_Y:
            case Axis::STICK_RIGHT_X:
            case Axis::STICK_RIGHT_Y:
            case Axis::TRIGGER_LEFT:
            case Axis::TRIGGER_RIGHT:
                return true;
        }
        
        return false;
    }

    void CIwGameControllerAndroid::SetPropagateButtonsToKeyboard(bool propagate)
    {
        return s3eAndroidControllerSetPropagateButtonsToKeyboard(propagate);
    }

}   // namespace IwGameController
