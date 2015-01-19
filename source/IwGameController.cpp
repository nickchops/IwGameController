/*
 * (C) 2015 Nick Smith. All Rights Reserved.
 */

#include "IwGameController.h"
#include "IwGameController_Android.h"
#include "IwGameController_iOS.h"
#include "IwGameController_DesktopHid.h"

namespace IwGameController
{

IwGameController*  IwGameController::m_CurrentGameController = 0;

IwGameController::IwGameController()
: m_Type(Type::NONE)
{
}

Type::eType IwGameController::GetType() const
{
    return m_Type;
}


void IwGameController::SetType(Type::eType type)
{
    m_Type = type;
}

bool IwGameController::Init()
{
    if (!IsAvailable(IW_GAMECONTROLLER->GetType())) //TODO WTF is this for?
        return false;

    //set event stuff to 0 here. look at iwbilling for info.

    return true;
}

void IwGameController::Release()
{
}

void IwGameController::NotifyButtonEvent(CIwGameControllerButtonEvent* data)
{
    if (ButtonCallback != 0)
        ButtonCallback(ButtonCallbackData, data);
}

IwGameController* IwGameController::Create(Type::eType type)
{
    IwGameController::m_CurrentGameController = 0;

    switch (type)
    {
        case Type::IOS:
            CIwGameControllerIOS::Create();
            IwGameController::m_CurrentGameController = IW_GAMECONTROLLER_IOS;
            IW_GAMECONTROLLER_IOS->SetType(Type::IOS);
            if (!IW_GAMECONTROLLER_IOS->Init())
                return 0;
            break;
        case Type::ANDROID_ANY:
        case Type::ANDROID_GENERIC:
        case Type::ANDROID_OUYA_EVERYWHERE:
        case Type::ANDROID_AMAZON:
            CIwGameControllerAndroid::Create();
            IwGameController::m_CurrentGameController = IW_GAMECONTROLLER_ANDROID;
            //todo, need to set tpe based on whether type supported  and falling back to generic if not
            IW_GAMECONTROLLER_ANDROID->SetType(type);
            if (!IW_GAMECONTROLLER_ANDROID->Init())
                return 0;
            break;
        case Type::DESKTOP_HID:
            CIwGameControllerDesktopHid::Create();
            IwGameController::m_CurrentGameController = IW_GAMECONTROLLER_DESKTOP_HID;
            IW_GAMECONTROLLER_DESKTOP_HID->SetType(type);
            if (!IW_GAMECONTROLLER_DESKTOP_HID->Init())
                return 0;
            break;
    }

    return IwGameController::GetGameController();
}

void IwGameController::Destroy()
{
    if (IwGameController::m_CurrentGameController == 0)
        return;
    
    switch (IwGameController::m_CurrentGameController->GetType())
    {
        case Type::IOS:
            if (IW_GAMECONTROLLER_IOS != 0)
            {
                IW_GAMECONTROLLER_IOS->Release();
                CIwGameControllerIOS::Destroy();
            }
            break;
        case Type::ANDROID_ANY:
        case Type::ANDROID_GENERIC:
        case Type::ANDROID_OUYA_EVERYWHERE:
        case Type::ANDROID_AMAZON:
            if (IW_GAMECONTROLLER_ANDROID != 0)
            {
                IW_GAMECONTROLLER_ANDROID->Release();
                CIwGameControllerAndroid::Destroy();
            }
            break;
        case Type::DESKTOP_HID:
            if (IW_GAMECONTROLLER_DESKTOP_HID != 0)
            {
                IW_GAMECONTROLLER_DESKTOP_HID->Release();
                CIwGameControllerDesktopHid::Destroy();
            }
            break;
    }
    IwGameController::m_CurrentGameController = 0;
}

// 
// Data constructors

CIwGameControllerButtonEvent::CIwGameControllerButtonEvent()
: Button(0)
, Pressed(0)
{
}

// ------------------------------------------------------------------------
//
//  Static Public API
//

bool IsAvailable(Type::eType type)
{
    if (type == Type::IOS)
        return (s3eIOSControllerAvailable() == S3E_TRUE);
    
    else if (type == Type::ANDROID_GENERIC)
        return (s3eAndroidControllerAvailable() == S3E_TRUE);
    
    else if (type == Type::ANDROID_OUYA_EVERYWHERE)
        return (s3eAndroidControllerAvailable() == S3E_TRUE && s3eAndroidControllerIsTypeSupported(S3E_ANDROIDCONTROLLER_TYPE_OUYA_EVERYWHERE));
    
    else if (type == Type::ANDROID_AMAZON)
        return (s3eAndroidControllerAvailable() == S3E_TRUE && s3eAndroidControllerIsTypeSupported(S3E_ANDROIDCONTROLLER_TYPE_AMAZON));
    
    else if (type == Type::DESKTOP_HID)
        return s3eHidControllerAvailable() == S3E_TRUE;
    
    else if (type == Type::ANY)
    {
        int os = s3eDeviceGetInt(S3E_DEVICE_OS);
    
        switch (os)
        {
            case S3E_OS_ID_IPHONE:
                return (s3eIOSControllerAvailable() == S3E_TRUE);
            case S3E_OS_ID_ANDROID:
                if (s3eAndroidControllerAvailable() == S3E_FALSE)
                    return false;
                else if (type == Type::ANDROID_GENERIC || type == Type::ANDROID_ANY)
                    return true;
                else
                {
                    if (type == Type::ANDROID_OUYA_EVERYWHERE)
                        return s3eAndroidControllerIsTypeSupported(S3E_ANDROIDCONTROLLER_TYPE_OUYA_EVERYWHERE);
                    else if (type == Type::ANDROID_AMAZON)
                        return s3eAndroidControllerIsTypeSupported(S3E_ANDROIDCONTROLLER_TYPE_AMAZON);
                    else
                        return false;
                }
            case S3E_OS_ID_WINDOWS:
            case S3E_OS_ID_OSX:
                return s3eHidControllerAvailable() == S3E_TRUE;
            default:
                return false;
        }
    }
    else
        return false;
}


bool Init(Type::eType type)
{
    if (type == Type::ANY)
    {
        int os = s3eDeviceGetInt(S3E_DEVICE_OS);
    
        switch (os)
        {
            case S3E_OS_ID_IPHONE:
                type = Type::IOS;
                break;
            case S3E_OS_ID_ANDROID:
                type = Type::ANDROID_ANY;
                break;
            case S3E_OS_ID_WINDOWS:
            case S3E_OS_ID_OSX:
                type = Type::DESKTOP_HID;
                break;
            default:
                return false;
        }
    }
    
    if (!IsAvailable(type))
        return false;
    
    return IwGameController::Create(type) != 0;
}

void Terminate()
{
    IwGameController::Destroy();
}

Type::eType GetType()
{
    return IW_GAMECONTROLLER->GetType();
}

// Callbacks

//TODO: look up whats going on with userdata in billing
void SetButtonCallback(IwGameControllerButtonCallback callback/*, void *userdata*/)
{
    if (IW_GAMECONTROLLER == 0)
        return;
    IW_GAMECONTROLLER->SetButtonCallback(callback/*, userdata*/);
}

// public functions
        
void StartFrame()
{
    if (IW_GAMECONTROLLER != 0)
        IW_GAMECONTROLLER->StartFrame();
}

bool SelectControllerByPlayer(int player)
{
    if (IW_GAMECONTROLLER == 0)
        return false;
    return IW_GAMECONTROLLER->SelectControllerByPlayer(player);
}

//TODO: rename this to controller count
int GetPlayerCount()
{
    if (IW_GAMECONTROLLER == 0)
        return 0;
    return IW_GAMECONTROLLER->GetPlayerCount();
}

int GetMaxControllers()
{
    if (IW_GAMECONTROLLER == 0)
        return 0;
    return IW_GAMECONTROLLER->GetMaxControllers();
}


bool GetButtonState(Button::eButton button)
{
    if (IW_GAMECONTROLLER == 0)
        return false;
    return IW_GAMECONTROLLER->GetButtonState(button);
}

float GetAxisValue(Axis::eAxis axis)
{
    if (IW_GAMECONTROLLER == 0)
        return false;
    return IW_GAMECONTROLLER->GetAxisValue(axis);
}

bool GetButtonDisplayName(char* dst, Button::eButton button, bool terminateString)
{
    const char* const buttons[] = { "A", "B", "X", "Y", "DPadCenter", "DPadUp", "DPadDown", "DPadLeft", "DPadRight", 
                                    "ShoulderLeft", "ShoulderRight", "StickLeft", "StickRight",
                                    "TriggerLeft", "TriggerRight"};
    const char* name;
    switch (button)
    {
        case Button::A:
            name = buttons[0];
            break;
        case Button::B:
            name = buttons[1];
            break;
        case Button::X:
            name = buttons[2];
            break;
        case Button::Y:
            name = buttons[3];
            break;
        case Button::DPAD_CENTER:
            name = buttons[4];
            break;
        case Button::DPAD_UP:
            name = buttons[5];
            break;
        case Button::DPAD_DOWN:
            name = buttons[6];
            break;
        case Button::DPAD_LEFT:
            name = buttons[7];
            break;
        case Button::DPAD_RIGHT:
            name = buttons[8];
            break;
        case Button::SHOULDER_LEFT:
            name = buttons[9];
            break;
        case Button::SHOULDER_RIGHT:
            name = buttons[10];
            break;
        case Button::STICK_LEFT:
            name = buttons[11];
            break;
        case Button::STICK_RIGHT:
            name = buttons[12];
            break;
        case Button::TRIGGER_LEFT:
            name = buttons[13];
            break;
        case Button::TRIGGER_RIGHT:
            name = buttons[14];
            break;
        default:
            return false;
    }
    
    int length = strlen(name);
    if (terminateString)
        length += 1;
    
    strncpy(dst, name, length);
    
	return true;
}

bool GetAxisDisplayName(char* dst, Axis::eAxis axis, bool terminateString)
{
    const char* const axes[] = { "StickLeftX", "StickLeftY", "StickRightX", "StickRightX", "TriggerLeft", "TriggerRight" };
    const char* name;
    switch (axis)
    {
        case Axis::STICK_LEFT_X:
            name = axes[0];
            break;
        case Axis::STICK_LEFT_Y:
            name = axes[1];
            break;
        case Axis::STICK_RIGHT_X:
            name = axes[2];
            break;
        case Axis::STICK_RIGHT_Y:
            name = axes[3];
            break;
        case Axis::TRIGGER_LEFT:
            name = axes[4];
            break;
        case Axis::TRIGGER_RIGHT:
            name = axes[5];
            break;
        default:
            return false;
    }
    
    int length = strlen(name);
    if (terminateString)
        length += 1;
    
    strncpy(dst, name, length);
    
	return true;
}

void SetPropagateButtonsToKeyboard(bool propagate)
{
    if (IW_GAMECONTROLLER == 0)
        return;
    IW_GAMECONTROLLER->SetPropagateButtonsToKeyboard(propagate);
}

}   // namespace IwGameController
