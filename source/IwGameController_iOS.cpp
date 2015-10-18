/*
 * (C) 2015 Nick Smith.
 */

#include "IwGameController_iOS.h"

namespace IwGameController
{

    CIwGameControllerIOS::CIwGameControllerIOS()
    {
        m_Type = Type::IOS;
        
        //TODO: register for callbacks here. That will init extension if not already inited.
    }

    CIwGameControllerIOS::~CIwGameControllerIOS()
    {
        //TODO: extension terminate here
    }

    // -------- Extension wrapping functions ---------------

    void CIwGameControllerIOS::StartFrame()
    {
    }

    CIwControllerHandle* CIwGameControllerIOS::GetControllerByIndex(int index)
    {
        return s3eIOSControllerGetController(index);
    }

    CIwControllerHandle* CIwGameControllerIOS::GetControllerByPlayer(int player)
    {
        return NULL;
    }

    int CIwGameControllerIOS::GetControllerCount()
    {
        return s3eIOSControllerGetControllerCount();
    }

    int CIwGameControllerIOS::GetMaxControllers()
    {
        return 4; // Hard coded for now...
    }

    bool CIwGameControllerIOS::GetButtonState(CIwControllerHandle* handle, Button::eButton button)
    {
        switch (button)
        {
            case Button::A:
                return s3eIOSControllerGetButtonState((s3eIOSController*)handle, S3E_IOSCONTROLLER_BUTTON_A);
            case Button::B:
                return s3eIOSControllerGetButtonState((s3eIOSController*)handle, S3E_IOSCONTROLLER_BUTTON_B);
            case Button::DPAD_DOWN:
                return s3eIOSControllerGetButtonState((s3eIOSController*)handle, S3E_IOSCONTROLLER_BUTTON_DPAD_DOWN);
            case Button::DPAD_LEFT:
                return s3eIOSControllerGetButtonState((s3eIOSController*)handle, S3E_IOSCONTROLLER_BUTTON_DPAD_LEFT);
            case Button::DPAD_RIGHT:
                return s3eIOSControllerGetButtonState((s3eIOSController*)handle, S3E_IOSCONTROLLER_BUTTON_DPAD_RIGHT);
            case Button::DPAD_UP:
                return s3eIOSControllerGetButtonState((s3eIOSController*)handle, S3E_IOSCONTROLLER_BUTTON_DPAD_UP);
            case Button::SHOULDER_LEFT:
                return s3eIOSControllerGetButtonState((s3eIOSController*)handle, S3E_IOSCONTROLLER_BUTTON_LEFT_SHOULDER);
            case Button::SHOULDER_RIGHT:
                return s3eIOSControllerGetButtonState((s3eIOSController*)handle, S3E_IOSCONTROLLER_BUTTON_RIGHT_SHOULDER);
            case Button::TRIGGER_LEFT:
                return s3eIOSControllerGetButtonState((s3eIOSController*)handle, S3E_IOSCONTROLLER_BUTTON_LEFT_TRIGGER);
            case Button::TRIGGER_RIGHT:
                return s3eIOSControllerGetButtonState((s3eIOSController*)handle, S3E_IOSCONTROLLER_BUTTON_RIGHT_TRIGGER);
            case Button::X:
                return s3eIOSControllerGetButtonState((s3eIOSController*)handle, S3E_IOSCONTROLLER_BUTTON_X);
            case Button::Y:
                return s3eIOSControllerGetButtonState((s3eIOSController*)handle, S3E_IOSCONTROLLER_BUTTON_Y);
            case Button::LEFT_STICK_UP:
                return s3eIOSControllerGetButtonState((s3eIOSController*)handle, S3E_IOSCONTROLLER_BUTTON_RIGHT_THUMBSTICK_UP);
            case Button::LEFT_STICK_DOWN:
                return s3eIOSControllerGetButtonState((s3eIOSController*)handle, S3E_IOSCONTROLLER_BUTTON_LEFT_THUMBSTICK_DOWN);
            case Button::LEFT_STICK_LEFT:
                return s3eIOSControllerGetButtonState((s3eIOSController*)handle, S3E_IOSCONTROLLER_BUTTON_LEFT_THUMBSTICK_LEFT);
            case Button::LEFT_STICK_RIGHT:
                return s3eIOSControllerGetButtonState((s3eIOSController*)handle, S3E_IOSCONTROLLER_BUTTON_LEFT_THUMBSTICK_RIGHT);
            case Button::RIGHT_STICK_UP:
                return s3eIOSControllerGetButtonState((s3eIOSController*)handle, S3E_IOSCONTROLLER_BUTTON_RIGHT_THUMBSTICK_UP);
            case Button::RIGHT_STICK_DOWN:
                return s3eIOSControllerGetButtonState((s3eIOSController*)handle, S3E_IOSCONTROLLER_BUTTON_RIGHT_THUMBSTICK_DOWN);
            case Button::RIGHT_STICK_LEFT:
                return s3eIOSControllerGetButtonState((s3eIOSController*)handle, S3E_IOSCONTROLLER_BUTTON_RIGHT_THUMBSTICK_LEFT);
            case Button::RIGHT_STICK_RIGHT:
                return s3eIOSControllerGetButtonState((s3eIOSController*)handle, S3E_IOSCONTROLLER_BUTTON_RIGHT_THUMBSTICK_RIGHT);
            default:
                return false;
        }
    }

    float CIwGameControllerIOS::GetAxisValue((s3eIOSController*)handle, Axis::eAxis axis)
    {
        switch (button)
        {
            case Axis::DPAD_X:
                return s3eIOSControllerGetAxisValue((s3eIOSController*)handle, S3E_IOSCONTROLLER_AXIS_DPAD_X);
            case Axis::DPAD_Y:
                return s3eIOSControllerGetAxisValue((s3eIOSController*)handle, S3E_IOSCONTROLLER_AXIS_DPAD_Y);
            case Axis::STICK_LEFT_X:
                return s3eIOSControllerGetAxisValue((s3eIOSController*)handle, S3E_IOSCONTROLLER_AXIS_LEFT_THUMBSTICK_X);
            case Axis::STICK_LEFT_Y:
                return s3eIOSControllerGetAxisValue((s3eIOSController*)handle, S3E_IOSCONTROLLER_AXIS_LEFT_THUMBSTICK_Y);
            case Axis::STICK_RIGHT_X:
                return s3eIOSControllerGetAxisValue((s3eIOSController*)handle, S3E_IOSCONTROLLER_AXIS_RIGHT_THUMBSTICK_X);
            case Axis::STICK_RIGHT_Y:
                return s3eIOSControllerGetAxisValue((s3eIOSController*)handle, S3E_IOSCONTROLLER_AXIS_RIGHT_THUMBSTICK_Y);
            case Axis::TRIGGER_LEFT:
                return s3eIOSControllerGetAxisValue((s3eIOSController*)handle, S3E_IOSCONTROLLER_AXIS_LEFT_TRIGGER);
            case Axis::TRIGGER_RIGHT:
                return s3eIOSControllerGetAxisValue((s3eIOSController*)handle, S3E_IOSCONTROLLER_AXIS_RIGHT_TRIGGER);
            default:
                return 0.0;
        }
    }
    
    
    bool CIwGameControllerIOS::IsButtonSupported(CIwControllerHandle* handle, Button::eButton button)
    {
        if (!handle || handle and s3eIOSControllerSupportsBasic((s3eIOSController*)handle))
        {
            switch (button)
            {
                case Button::A:
                case Button::B:
                case Button::DPAD_DOWN:
                case Button::DPAD_LEFT:
                case Button::DPAD_RIGHT:
                case Button::DPAD_UP:
                case Button::SHOULDER_LEFT:
                case Button::SHOULDER_RIGHT:
                case Button::X:
                case Button::Y:
                    return true;
            }
        }
        
        if (!handle || handle and s3eIOSControllerSupportsExtended((s3eIOSController*)handle))
        {
            switch (button)
            {
                case Button::TRIGGER_LEFT:
                case Button::TRIGGER_RIGHT:
                case Button::LEFT_STICK_UP:
                case Button::LEFT_STICK_DOWN:
                case Button::LEFT_STICK_LEFT:
                case Button::LEFT_STICK_RIGHT:
                case Button::RIGHT_STICK_UP:
                case Button::RIGHT_STICK_DOWN:
                case Button::RIGHT_STICK_LEFT:
                case Button::RIGHT_STICK_RIGHT:
                    return true;
            }
        }
        
        return false;
    
    }
    
    bool CIwGameControllerIOS::IAxisSupported(CIwControllerHandle* handle, Axis::eAxis axis)
    {
        if (!handle || handle and s3eIOSControllerSupportsBasic((s3eIOSController*)handle))
        {
            switch (axis)
            {
                case Axis::DPAD_X:
                case Axis::DPAD_Y:
                    return true;
            }
        }
        
        if (!handle || handle and s3eIOSControllerSupportsExtended((s3eIOSController*)handle))
        {
            switch (axis)
            {
                case STICK_LEFT_X:
                case STICK_LEFT_Y:
                case STICK_RIGHT_X:
                case STICK_RIGHT_Y:
                    return true;
            }
        }
        
        return false;

    }


    void CIwGameControllerIOS::SetPropagateButtonsToKeyboard(bool propagate)
    {
    }

}   // namespace IwGameController

