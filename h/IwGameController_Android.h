/*
 * (C) 2015 Nick Smith
 *
 * Android Implementation of IwGameController
 */
 
#ifndef IW_GAMECONTROLLER_ANDROID_H
#define IW_GAMECONTROLLER_ANDROID_H

#include "s3e.h"
#include "s3eAndroidController.h"
#include "IwGameController.h"

namespace IwGameController {

	class CIwGameControllerAndroid : public CIwGameController
	{
	public:
		CIwGameControllerAndroid(Type::eType type);
		virtual ~CIwGameControllerAndroid();

		int     GetControllerCount();
		int     GetMaxControllers();
		int     GetProperty(CIwGameControllerHandle* handle, Property::eProperty prop);
		void    SetProperty(CIwGameControllerHandle* handle, Property::eProperty prop, int value);
		ControllerType::eControllerType GetControllerType(CIwGameControllerHandle* handle);

		CIwGameControllerHandle* GetControllerByIndex(int index);
		CIwGameControllerHandle* GetControllerByPlayer(int player);
		void    SetPropagateButtonsToKeyboard(bool propagate);

		bool    IsButtonSupported(CIwGameControllerHandle* handle, Button::eButton button);
		bool    IsAxisSupported(CIwGameControllerHandle* handle, Axis::eAxis axis);

		void    StartFrame();
		bool    GetButtonState(CIwGameControllerHandle* handle, Button::eButton button);
		float   GetAxisValue(CIwGameControllerHandle* handle, Axis::eAxis axis);
	};

}// namespace IwGameController

#endif
