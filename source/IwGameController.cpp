/*
 * (C) 2015 Nick Smith. All Rights Reserved.
 */

#include "IwGameController.h"

namespace IwGameController
{
    const char* const CIwGameController::s_ButtonNames[Button::MAX] = {
        "A", "B", "X", "Y", "DPadCenter", "DPadUp", "DPadDown", "DPadLeft", "DPadRight", "DPadTouch",
        "ShoulderLeft", "ShoulderRight", "StickLeft", "StickRight",
        "TriggerLeft", "TriggerRight", "Select", "Start",
        "LeftStickUp", "LeftStickDown", "LeftStickLeft", "LeftStickRight",
        "RightStickUp", "RightStickDown", "RightStickLeft", "RightStickRight" };

    const char* const CIwGameController::s_AxisNames[Axis::MAX] = {
        "DPadX", "DPadY", "StickLeftX", "StickLeftY", "StickRightX", "StickRightY", "TriggerLeft", "TriggerRight" };

	CIwGameController::CIwGameController()
	: m_Type(Type::NONE)
	{
		m_ConnectCallback = NULL;
		m_DisconnectCallback = NULL;
		m_PauseCallback = NULL;
        m_ButtonCallback = NULL;
        m_AxisCallback = NULL;

		m_ConnectCallbackUserdata = NULL;
		m_DisconnectCallbackUserdata = NULL;
		m_PauseCallbackUserdata = NULL;
        m_ButtonCallbackUserdata = NULL;
        m_AxisCallbackUserdata = NULL;
	}

	Type::eType CIwGameController::GetType() const
	{
		return m_Type;
	}

	bool CIwGameController::GetButtonDisplayName(char* dst, Button::eButton button, bool terminateString)
	{
		const char* name;

        unsigned int i = (int)button;

        if (i > sizeof(s_ButtonNames)) // shouldnt happen but check in case we initialise the names wrong
            return false;

        name = s_ButtonNames[i];

		int length = strlen(name);

		if (terminateString)
			length += 1;
    
		strncpy(dst, name, length);
    
		return true;
	}

	bool CIwGameController::GetAxisDisplayName(char* dst, Axis::eAxis axis, bool terminateString)
	{
        const char* name;

        unsigned int i = (int)axis;

        if (i > sizeof(s_AxisNames)) // shouldnt happen but check in case we initialise the names wrong
            return false;

        name = s_AxisNames[i];
		    
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
			m_ConnectCallback(data, m_ConnectCallbackUserdata);
	}

	void CIwGameController::NotifyDisconnect(CIwGameControllerHandle* data)
	{
		if (m_DisconnectCallback)
			m_DisconnectCallback(data, m_DisconnectCallbackUserdata);
	}

	void CIwGameController::NotifyPause(CIwGameControllerHandle* data)
	{
		if (m_PauseCallback)
			m_PauseCallback(data, m_PauseCallbackUserdata);
	}

	void CIwGameController::NotifyButton(CIwGameControllerButtonEvent* data)
	{
		if (m_ButtonCallback)
			m_ButtonCallback(data, m_ButtonCallbackUserdata);
	}
    
    void CIwGameController::NotifyAxis(CIwGameControllerAxisEvent* data)
    {
        if (m_AxisCallback)
            m_AxisCallback(data, m_AxisCallbackUserdata);
    }

	// Data constructors

	CIwGameControllerButtonEvent::CIwGameControllerButtonEvent()
    : m_Controller(NULL), m_Button(Button::MAX), m_Pressed(0)
	{
	}
    
    CIwGameControllerAxisEvent::CIwGameControllerAxisEvent()
    : m_Controller(NULL), m_Axis(Axis::MAX), m_Value(0.0)
    {
    }

}   // namespace IwGameController
