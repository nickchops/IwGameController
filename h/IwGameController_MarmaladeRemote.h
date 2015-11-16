/*
 * (C) 2015 Nick Smith
 *
 * MarmaladeRemote Implementation of IwGameController
 */
 
#ifndef IW_GAMECONTROLLER_MARMALADEREMOTE_H
#define IW_GAMECONTROLLER_MARMALADEREMOTE_H

#include "s3e.h"
#include "IwGameController.h"
#include "SocketsUDP.h"

namespace IwGameController {
    
    struct RemoteButtonIDs
    {
        enum eRemoteButtonIDs
        {
            DPAD_TOUCH = 0,
            A,
            X,
            START,
            BUTTON_COUNT
        };
    };

    struct RemoteAxisIDs
    {
        enum eRemoteAxisIDs
        {
            DPAD_X = 0,
            DPAD_Y,
            AXIS_COUNT
        };
    };

	class CIwGameControllerMarmaladeRemote : public CIwGameController
	{
    
	private:
		static bool m_ButtonState[Button::MAX];
		static int32 _ConnectCallback(void *systemData, void *userData);
		static int32 _DisconnectCallback(void *systemData, void *userData);
		static int32 _PauseCallback(void *systemData, void *userData);

        // Socket to listen for incomming conections
        // Currently have unique Connect/Disconnect/IsConnected methods
        // Probably should move these into the Connect etc callbacks to be generic
        SocketsUDP::SocketUDP m_socket;
        bool m_connected;
        int m_maxPackets; // limit for processing in an update
        bool m_BtnState[RemoteButtonIDs::BUTTON_COUNT];
        float m_AxisValue[RemoteAxisIDs::AXIS_COUNT];
    
	public:
		CIwGameControllerMarmaladeRemote();
		virtual ~CIwGameControllerMarmaladeRemote();

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

        static const int MARMALADE_REMOTE_PORT;
        static const int MARMALADE_REMOTE_PACKET_SIZE;
        static const int AXIS_VALUE_LENGTH;

        bool Connect();
        void Disconnect();
        bool IsConnected();
	};

}   // namespace IwGameController

#endif
