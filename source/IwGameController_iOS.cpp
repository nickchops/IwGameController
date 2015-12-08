/*
 * (C) 2015 Nick Smith.
 */

#include "IwGameController_iOS.h"

namespace IwGameController
{
    // internal callback handlers
    int32 CIwGameControllerIOS::_ConnectCallback(void *systemData, void *userData)
    {
        ((CIwGameControllerIOS*)userData)->NotifyConnect((CIwGameControllerHandle*)systemData);
        return 1;
    }

    int32 CIwGameControllerIOS::_DisconnectCallback(void *systemData, void *userData)
    {
        ((CIwGameControllerIOS*)userData)->NotifyDisconnect((CIwGameControllerHandle*)systemData);
        return 1;
    }

    int32 CIwGameControllerIOS::_PauseCallback(void *systemData, void *userData)
    {
        ((CIwGameControllerIOS*)userData)->NotifyPause((CIwGameControllerHandle*)systemData);
        return 1;
    }

    // Init/term

    CIwGameControllerIOS::CIwGameControllerIOS()
    {
        m_Type = Type::IOS;

        s3eIOSControllerRegister(S3E_IOSCONTROLLER_CALLBACK_CONNECTED, _ConnectCallback, (void*)this);
        s3eIOSControllerRegister(S3E_IOSCONTROLLER_CALLBACK_DISCONNECTED, _DisconnectCallback, (void*)this);
        s3eIOSControllerRegister(S3E_IOSCONTROLLER_CALLBACK_PAUSE_PRESSED, _PauseCallback, (void*)this);
    }

    CIwGameControllerIOS::~CIwGameControllerIOS()
    {
        //TODO: extension terminate here
    }

    // -------- Extension wrapping functions ---------------

    void CIwGameControllerIOS::StartFrame()
    {
        s3ePointerUpdate(); // needed for s3ePointer touch state -> micro cotroller DPAD_TOUCH state

        /*if (m_ButtonCallback)
        {
            // Can emulate events via state check if needed. Look like we can do events in IOS extension...
            // Loop through all controllers here
            {
                for (int i = 0; i < Button::MAX; i++)
                {
                    if (m_ButtonState != GetButtonState((Button::eButton)i))
                    {
                        //Construct obj with controller, button and state
                        NotifyButton(obj)
                    }
                }
            }
        }*/
    }

    CIwGameControllerHandle* CIwGameControllerIOS::GetControllerByIndex(int index)
    {
        return (CIwGameControllerHandle*)s3eIOSControllerGetController(index);
    }

    CIwGameControllerHandle* CIwGameControllerIOS::GetControllerByPlayer(int player)
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

    int CIwGameControllerIOS::GetProperty(CIwGameControllerHandle* handle, Property::eProperty prop)
    {
        switch (prop)
        {
        case Property::REPORTS_ABSOLUTE_DPAD_VALUES:
            return s3eIOSControllerGetReportsAbsoluteDpadValues((s3eIOSController*)handle);
        case Property::ALLOWS_ROTATION:
            return s3eIOSControllerGetAllowsRotation((s3eIOSController*)handle);
        default:
            return -1;
        }
    }

    void CIwGameControllerIOS::SetProperty(CIwGameControllerHandle* handle, Property::eProperty prop, int value)
    {
        switch (prop)
        {
        case Property::REPORTS_ABSOLUTE_DPAD_VALUES:
            s3eIOSControllerSetReportsAbsoluteDpadValues((s3eIOSController*)handle, value == 1 ? true : false);
            break;
        case Property::ALLOWS_ROTATION:
            s3eIOSControllerSetAllowsRotation((s3eIOSController*)handle, value == 1 ? true : false);
            break;
        default:
            break;
        }
    }

    ControllerType::eControllerType CIwGameControllerIOS::GetControllerType(CIwGameControllerHandle* handle)
    {
        if (!handle)
            return ControllerType::UNKNOWN;

        if (s3eIOSControllerSupportsExtended((s3eIOSController*)handle))
            return ControllerType::EXTENDED;
        else if (s3eIOSControllerSupportsMicro((s3eIOSController*)handle))
            return ControllerType::MICRO;
        else if (s3eIOSControllerSupportsBasic((s3eIOSController*)handle))
            return ControllerType::BASIC;
        else
            return ControllerType::UNKNOWN;
    }

    bool CIwGameControllerIOS::GetButtonState(CIwGameControllerHandle* handle, Button::eButton button)
    {
        if (!handle)
            return false;

        s3ePointerState state;

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
            return s3eIOSControllerGetButtonState((s3eIOSController*)handle, S3E_IOSCONTROLLER_BUTTON_LEFT_THUMBSTICK_UP);
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
#ifdef SDK_SUPPORTS_TVOS
            case Button::DPAD_TOUCH:
            state = s3ePointerGetState(S3E_POINTER_BUTTON_TOUCHPAD);
            return (state & S3E_POINTER_STATE_DOWN);
#endif
        case Button::START:
            //back key gets pressed/released both set for a single frame on menu key events
            //(down state is never set)
            return (s3eKeyboardGetState(s3eKeyBack) & S3E_KEY_STATE_PRESSED);
        default:
            return false;
        }
    }

    float CIwGameControllerIOS::GetAxisValue(CIwGameControllerHandle* handle, Axis::eAxis axis)
    {
        if (!handle)
            return 0.0;

        switch (axis)
        {
        case Axis::DPAD_X:
            return s3eIOSControllerGetAxisValue((s3eIOSController*)handle, S3E_IOSCONTROLLER_AXIS_DPAD_X);
        case Axis::DPAD_Y:
            //Touch pad has origin at bottom left unlike normal controllers
            return -s3eIOSControllerGetAxisValue((s3eIOSController*)handle, S3E_IOSCONTROLLER_AXIS_DPAD_Y);
        case Axis::STICK_LEFT_X:
            return s3eIOSControllerGetAxisValue((s3eIOSController*)handle, S3E_IOSCONTROLLER_AXIS_LEFT_THUMBSTICK_X);
        case Axis::STICK_LEFT_Y:
            //Not sure if we need to flip these too - dont have a pad to test with atm!
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


    bool CIwGameControllerIOS::IsButtonSupported(CIwGameControllerHandle* handle, Button::eButton button)
    {
        if (!handle || handle && s3eIOSControllerSupportsBasic((s3eIOSController*)handle))
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

        if (!handle || handle && s3eIOSControllerSupportsExtended((s3eIOSController*)handle))
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

        if (!handle || handle && s3eIOSControllerSupportsMicro((s3eIOSController*)handle))
            if (button == Button::DPAD_TOUCH)
                return true;

        return false;

    }

    bool CIwGameControllerIOS::IsAxisSupported(CIwGameControllerHandle* handle, Axis::eAxis axis)
    {
        if (!handle || handle && s3eIOSControllerSupportsBasic((s3eIOSController*)handle))
        {
            switch (axis)
            {
            case Axis::DPAD_X:
            case Axis::DPAD_Y:
                return true;
            }
        }

        if (!handle || handle && s3eIOSControllerSupportsExtended((s3eIOSController*)handle))
        {
            switch (axis)
            {
            case Axis::STICK_LEFT_X:
            case Axis::STICK_LEFT_Y:
            case Axis::STICK_RIGHT_X:
            case Axis::STICK_RIGHT_Y:
                    return true;
            }
        }

        return false;
    }

}   // namespace IwGameController

