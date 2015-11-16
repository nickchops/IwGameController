/*
 * (C) 2015 Nick Smith
 *
 * iOS Implementation of IwGameController
 */
 
#ifndef IW_GAMECONTROLLER_IOS_H
#define IW_GAMECONTROLLER_IOS_H

#include "s3e.h"
#include "s3eIOSController.h"
#include "IwGameController.h"

#ifndef S3E_OS_ID_TVOS
#define S3E_OS_ID_TVOS 32
#endif

namespace IwGameController {
    
	class CIwGameControllerIOS : public CIwGameController
	{
    
	private:
		static bool m_ButtonState[Button::MAX];
		static int32 _ConnectCallback(void *systemData, void *userData);
		static int32 _DisconnectCallback(void *systemData, void *userData);
		static int32 _PauseCallback(void *systemData, void *userData);
    
	public:
		CIwGameControllerIOS();
		virtual ~CIwGameControllerIOS();

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
