/*
 * (C) 2015 Nick Smith. All Rights Reserved.
 */

#include "IwGameController.h"

namespace IwGameController
{
	IwGameControllerConnectCallback CIwGameController::s_ConnectCallback = NULL;
	IwGameControllerDisconnectCallback CIwGameController::s_DisconnectCallback = NULL;
	IwGameControllerPauseCallback CIwGameController::s_PauseCallback = NULL;
	IwGameControllerButtonCallback CIwGameController::s_ButtonCallback = NULL;

	void* CIwGameController::s_ConnectCallbackUserdata = NULL;
	void* CIwGameController::s_DisconnectCallbackUserdata = NULL;
	void* CIwGameController::s_PauseCallbackUserdata = NULL;
	void* CIwGameController::s_ButtonCallbackUserdata = NULL;

    const char* const CIwGameController::s_ButtonNames[Button::MAX] = {
        "A", "B", "X", "Y", "DPadCenter", "DPadUp", "DPadDown", "DPadLeft", "DPadRight",
        "ShoulderLeft", "ShoulderRight", "StickLeft", "StickRight",
        "TriggerLeft", "TriggerRight", "Select", "Start",
        "LeftStickUp", "LeftStickDown", "LeftStickLeft", "LeftStickRight",
        "RightStickUp", "RightStickDown", "RightStickLeft", "RightStickRight" };

    const char* const CIwGameController::s_AxisNames[Axis::MAX] = {
        "DPadX", "DPadY", "StickLeftX", "StickLeftY", "StickRightX", "StickRightY", "TriggerLeft", "TriggerRight" };

	CIwGameController::CIwGameController()
	: m_Type(Type::NONE)
	{
		s_ConnectCallback = NULL;
		s_DisconnectCallback = NULL;
		s_PauseCallback = NULL;
		s_ButtonCallback = NULL;

		s_ConnectCallbackUserdata = NULL;
		s_DisconnectCallbackUserdata = NULL;
		s_PauseCallbackUserdata = NULL;
		s_ButtonCallbackUserdata = NULL;
	}

	Type::eType CIwGameController::GetType() const
	{
		return m_Type;
	}

	bool CIwGameController::GetButtonDisplayName(char* dst, Button::eButton button, bool terminateString)
	{
		const char* name;

        int i = (int)button;

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

        int i = (int)axis;

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
		if (s_ConnectCallback)
			s_ConnectCallback(data, s_ConnectCallbackUserdata);
	}

	void CIwGameController::NotifyDisconnect(CIwGameControllerHandle* data)
	{
		if (s_DisconnectCallback)
			s_DisconnectCallback(data, s_DisconnectCallbackUserdata);
	}

	void CIwGameController::NotifyPause(CIwGameControllerHandle* data)
	{
		if (s_PauseCallback)
			s_PauseCallback(data, s_PauseCallbackUserdata);
	}

	void CIwGameController::NotifyButton(CIwGameControllerButtonEvent* data)
	{
		if (s_ButtonCallback)
			s_ButtonCallback(data, s_ButtonCallbackUserdata);
	}

	// Data constructors

	CIwGameControllerButtonEvent::CIwGameControllerButtonEvent()
	: m_Controller(NULL), m_Button(0), m_Pressed(0)
	{
	}

}   // namespace IwGameController
