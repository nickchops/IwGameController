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
#include "s3eZeroConf.h"

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

        // Socket to listen for incomming conections
        // Currently have unique Connect/Disconnect/IsConnected methods
        // Probably should move these into the Connect etc callbacks to be generic
        SocketsUDP::SocketUDP m_Socket;
        bool m_Connected;
        int m_Connecting;
        int m_MaxPackets; // limit for processing in an update
        bool m_BtnState[RemoteButtonIDs::BUTTON_COUNT];
        float m_AxisValue[RemoteAxisIDs::AXIS_COUNT];
        s3eZeroConfPublished* m_Service;
        s3eZeroConfSearch*    m_Search;
        SocketsUDP::SocketAddress m_SenderAddress;
        char m_SenderAddressString[SOCKETSUDP_IP_ADDRESS_MAX_LENGTH];
        int64 m_ConnectTime;
        int64 m_IgnoreCheckTime;
        bool m_IgnoreAllTimeouts;
        bool m_BlockSender;
        int m_ConnectTimeout;
        int m_KeepAliveTimeout;

        // Remote-only callbacks
        //IwGameControllerCallback         m_KeepConnectedCallback;
        //void*                            m_KeepConnectedCallbackUserdata;
        //void                             NotifyKeepConnected(CIwGameControllerHandle* data);

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
        static const int IGNORE_CHECK_TIMEOUT;

        static const char* SENDER_NAME;
        static const char* RECEIVER_NAME;
        static const char* CONNECT_TOKEN;
        static const char* DISCONNECT_TOKEN;
        static const char* KEEPCONNECT_TOKEN;
        static const char* IGNORETIMEOUT_TOKEN;
        static const char* NOSEND_TOKEN;
        
        bool Connect(bool dontBroadcast=false, const char* appName = "Unknown");
        void Disconnect();
        bool IsConnecting();
        bool IsConnected();
        void SetConnectTimeout(float seconds);
        void SetKeepAliveTimeout(float seconds);
        void ResetValues();

        // If set to true, disables the receiver and sender (assuming sender gets the message) from
        // disconnecting on timeouts. Can set on/off either side of blocking menus for example.
        // No guarantee that sender wont disconnect anyway (e.g. close app)
        // If ignore is true, blockSender tells the sender to stop sending data (helps avoid
        // old data queuing up if StartFrame isnt being called regularly)
        // Sender always continues sending data when ignore is set to false
        void SetIgnoreTimeouts(bool ignore, bool blockSender=false);
	};

}   // namespace IwGameController

#endif
