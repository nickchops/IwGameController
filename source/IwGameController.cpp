/*
 * (C) 2015 Nick Smith. All Rights Reserved.
 */

#include "IwGameController.h"

namespace IwGameController
{

////IwGameController*  IwGameController::m_CurrentGameController = 0;

CIwGameController::CIwGameController()
: m_Type(Type::NONE)
{
    //set event stuff to 0 here. look at iwbilling for info.
}

Type::eType CIwGameController::GetType() const
{
    return m_Type;
}

bool CIwGameController::GetButtonDisplayName(char* dst, Button::eButton button, bool terminateString)
{
    const char* const buttons[] = { "A", "B", "X", "Y", "DPadCenter", "DPadUp", "DPadDown", "DPadLeft", "DPadRight",
        "ShoulderLeft", "ShoulderRight", "StickLeft", "StickRight",
        "TriggerLeft", "TriggerRight", "Start", "Select" };
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
        case Button::START:
            name = buttons[15];
            break;
        case Button::SELECT:
            name = buttons[16];
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

bool CIwGameController::GetAxisDisplayName(char* dst, Axis::eAxis axis, bool terminateString)
{
    const char* const axes[] = { "DPadX", "DPadY", "StickLeftX", "StickLeftY", "StickRightX", "StickRightX", "TriggerLeft", "TriggerRight" };
    const char* name;
    switch (axis)
    {
        case Axis::DPAD_X:
            name = axes[0];
            break;
        case Axis::DPAD_Y:
            name = axes[1];
            break;
        case Axis::STICK_LEFT_X:
            name = axes[2];
            break;
        case Axis::STICK_LEFT_Y:
            name = axes[3];
            break;
        case Axis::STICK_RIGHT_X:
            name = axes[4];
            break;
        case Axis::STICK_RIGHT_Y:
            name = axes[5];
            break;
        case Axis::TRIGGER_LEFT:
            name = axes[6];
            break;
        case Axis::TRIGGER_RIGHT:
            name = axes[7];
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

//---------------------------------------------------------------

void CIwGameController::NotifyConnect(CIwGameControllerHandle* data)
{
    if (m_ConnectCallback)
        m_ConnectCallback(data, m_ConnectCallbackData)
}

void CIwGameController::NotifyDisconnect(CIwGameControllerHandle* data)
{
    if (m_DisconnectCallback)
        m_DisconnectCallback(data, m_DisconnectCallbackData)
}

void CIwGameController::NotifyPause(CIwGameControllerHandle* data)
{
    if (m_PauseCallback)
        m_PauseCallback(data, m_PauseCallbackData)
}

void CIwGameController::NotifyButton(CIwGameControllerButtonEvent* data);
{
    if (m_ButtonCallback)
        m_ButtonCallback(data, m_ButtonCallbackData)
}

// Data constructors

CIwGameControllerButtonEvent::CIwGameControllerButtonEvent()
: Button(0)
, Pressed(0)
{
}

}   // namespace IwGameController
