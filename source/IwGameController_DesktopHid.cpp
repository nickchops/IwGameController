/*
 * (C) 2015 Nick Smith.
 */

#include "IwGameController_DesktopHid.h"

namespace IwGameController
{

    // ----------- Init terminate etc --------------

    CIwGameControllerDesktopHid::CIwGameControllerDesktopHid()
    {
        m_Type = Type::DESKTOP_HID;
        //TODO: register callbacks here will call init
    }

    CIwGameControllerDesktopHid::~CIwGameControllerDesktopHid()
    {
        
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

    int CIwGameControllerDesktopHid::GetControllerCount()
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
            case Button::TRIGGER_LEFT:
                return s3eHidControllerGetLeftTrigger() >= 0.95; //allow for trigger sensitivity
            case Button::TRIGGER_RIGHT:
                return s3eHidControllerGetRightTrigger() >= 0.95;
            case Button::SHOULDER_LEFT:
                return s3eHidControllerGetButtonLShoulderDown();
            case Button::SHOULDER_RIGHT:
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
            case Axis::DPAD_X:
                if (s3eHidControllerGetButtonDPadLeft())
                    return -1.0;
                if (s3eHidControllerGetButtonDPadRight())
                    return 1.0;
                break;
            case Axis::DPAD_Y:
                if (s3eHidControllerGetButtonDPadDown())
                    return -1.0;
                if (s3eHidControllerGetButtonDPadUp())
                    return 1.0;
                break;
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
            case Button::TRIGGER_LEFT:
            case Button::TRIGGER_RIGHT:
            case Button::X:
            case Button::Y:
            case Button::START:
            case Button::SELECT:
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

    void CIwGameControllerDesktopHid::SetPropagateButtonsToKeyboard(bool propagate)
    {
    }

}   // namespace IwGameController
