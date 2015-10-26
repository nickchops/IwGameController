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

	CIwGameControllerHandle* CIwGameControllerDesktopHid::GetControllerByIndex(int index)
	{
		return NULL;
	}

	CIwGameControllerHandle* CIwGameControllerDesktopHid::GetControllerByPlayer(int player)
	{
		return NULL;
	}

    int CIwGameControllerDesktopHid::GetControllerCount()
    {
		if (s3eHidControllerIsConnected())
			return 1;
		else
			return 0;
    }

    int CIwGameControllerDesktopHid::GetMaxControllers()
    {
        return 1;
    }


	int CIwGameControllerDesktopHid::GetProperty(CIwGameControllerHandle* handle, Property::eProperty prop)
	{
		return -1;
	}

	void CIwGameControllerDesktopHid::SetProperty(CIwGameControllerHandle* handle, Property::eProperty prop, int value)
	{
	}

	ControllerType::eControllerType CIwGameControllerDesktopHid::GetControllerType(CIwGameControllerHandle* handle)
	{
		return ControllerType::UNKNOWN;
	}

	bool CIwGameControllerDesktopHid::GetButtonState(CIwGameControllerHandle* handle, Button::eButton button)
    {
		// Currently only supports one controller. handle is ignored.

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
                return s3eHidControllerGetLeftTrigger() >= 0.15; //allow for trigger sensitivity - TODO: should be settable like sticks below
            case Button::TRIGGER_RIGHT:
                return s3eHidControllerGetRightTrigger() >= 0.15;
            case Button::SHOULDER_LEFT:
                return s3eHidControllerGetButtonLShoulderDown();
            case Button::SHOULDER_RIGHT:
                return s3eHidControllerGetButtonRShoulderDown();
            case Button::STICK_LEFT:
                return s3eHidControllerGetButtonStick1();
            case Button::STICK_RIGHT:
                return s3eHidControllerGetButtonStick2();
            case Button::START:
                return s3eHidControllerGetButtonStart();
            case Button::SELECT:
                return s3eHidControllerGetButtonSelect();
            case Button::LEFT_STICK_UP:
                return s3eHidControllerGetStick1YAxis() < -0.15; //hard coded - should be settable so game could override, e.g. for sensitivity in user settings menu
            case Button::LEFT_STICK_DOWN:
                return s3eHidControllerGetStick1YAxis() > 0.15;
            case Button::LEFT_STICK_LEFT:
                return s3eHidControllerGetStick1XAxis() < -0.15;
            case Button::LEFT_STICK_RIGHT:
                return s3eHidControllerGetStick1XAxis() > 0.15;
            case Button::RIGHT_STICK_UP:
                return s3eHidControllerGetStick2YAxis() < -0.15;
            case Button::RIGHT_STICK_DOWN:
                return s3eHidControllerGetStick2YAxis() > 0.15;
            case Button::RIGHT_STICK_LEFT:
                return s3eHidControllerGetStick2XAxis() < -0.15;
            case Button::RIGHT_STICK_RIGHT:
                return s3eHidControllerGetStick2XAxis() > 0.15;
            default:
                return false;
        }
    }

	float CIwGameControllerDesktopHid::GetAxisValue(CIwGameControllerHandle* handle, Axis::eAxis axis)
	{
		// Currently only supports one controller. handle is ignored.

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
                    return 1.0;
                if (s3eHidControllerGetButtonDPadUp())
                    return -1.0;
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
				break;
        }

		return false;
    }

	bool CIwGameControllerDesktopHid::IsButtonSupported(CIwGameControllerHandle* handle, Button::eButton button)
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
            case Button::START:
            case Button::SELECT:
                return true;
        }
        
        return false;
    }
    
	bool CIwGameControllerDesktopHid::IsAxisSupported(CIwGameControllerHandle* handle, Axis::eAxis axis)
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

}   // namespace IwGameController
