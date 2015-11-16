/*
 * (C) 2015 Nick Smith
 *
 * DesktopHid Implementation of IwGameController
 */
 
#ifndef IW_GAMECONTROLLER_DESKTOP_HID_H
#define IW_GAMECONTROLLER_DESKTOP_HID_H

#include "s3e.h"
#include "s3eHidController.h"
#include "IwGameController.h"

namespace IwGameController {
    
	class CIwGameControllerDesktopHid : public CIwGameController
	{
	public:
		CIwGameControllerDesktopHid();
		virtual ~CIwGameControllerDesktopHid();

		int     GetControllerCount();
		int     GetMaxControllers();
		int     GetProperty(CIwGameControllerHandle* handle, Property::eProperty prop);
		void    SetProperty(CIwGameControllerHandle* handle, Property::eProperty prop, int value);
		ControllerType::eControllerType GetControllerType(CIwGameControllerHandle* handle);

		CIwGameControllerHandle* GetControllerByIndex(int index);
		CIwGameControllerHandle* GetControllerByPlayer(int player);

		bool    IsButtonSupported(CIwGameControllerHandle* handle, Button::eButton button);
		bool    IsAxisSupported(CIwGameControllerHandle* handle, Axis::eAxis axis);

		void    StartFrame();
		bool    GetButtonState(CIwGameControllerHandle* handle, Button::eButton button);
		float   GetAxisValue(CIwGameControllerHandle* handle, Axis::eAxis axis);
	};

}   // namespace IwGameController

#endif
