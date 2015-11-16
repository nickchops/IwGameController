/*
 * (C) 2015 Nick Smith.
 */

#include "IwGameController_MarmaladeRemote.h"
#include "IwDebug.h"

namespace IwGameController
{
    const int CIwGameControllerMarmaladeRemote::MARMALADE_REMOTE_PORT = 1099;
    const int CIwGameControllerMarmaladeRemote::AXIS_VALUE_LENGTH = 7; // "x.xxxxx" unit plus . plus 5 points precision
    const int CIwGameControllerMarmaladeRemote::MARMALADE_REMOTE_PACKET_SIZE = RemoteButtonIDs::BUTTON_COUNT + RemoteAxisIDs::AXIS_COUNT*AXIS_VALUE_LENGTH;

    // internal callback handlers
    int32 CIwGameControllerMarmaladeRemote::_ConnectCallback(void *systemData, void *userData)
    {
        NotifyConnect((CIwGameControllerHandle*)systemData);
        return 1;
    }
    
	int32 CIwGameControllerMarmaladeRemote::_DisconnectCallback(void *systemData, void *userData)
    {
		NotifyDisconnect((CIwGameControllerHandle*)systemData);
        return 1;
    }
    
	int32 CIwGameControllerMarmaladeRemote::_PauseCallback(void *systemData, void *userData)
    {
		NotifyPause((CIwGameControllerHandle*)systemData);
        return 1;
    }
    
    // Init/term
    
    CIwGameControllerMarmaladeRemote::CIwGameControllerMarmaladeRemote()
    {
        m_Type = Type::MARMALADE_REMOTE;
        m_connected = false;
        m_maxPackets = 5;


        // TODO: these should work on checking device is still discoverable
        // and calling disconnect when not, or connect when a new device is found
        // when currently disconnected
        /*s3eIOSControllerRegister(S3E_IOSCONTROLLER_CALLBACK_CONNECTED, _ConnectCallback, 0);
        s3eIOSControllerRegister(S3E_IOSCONTROLLER_CALLBACK_DISCONNECTED, _DisconnectCallback, 0);
        s3eIOSControllerRegister(S3E_IOSCONTROLLER_CALLBACK_PAUSE_PRESSED, _PauseCallback, 0);*/
    }

    CIwGameControllerMarmaladeRemote::~CIwGameControllerMarmaladeRemote()
    {
        //TODO: close down sockets here and NULL internal handle to active controller(s)
    }

    
    // -------- connection  ---------------

    bool CIwGameControllerMarmaladeRemote::Connect()
    {
        m_connected = m_socket.Open(CIwGameControllerMarmaladeRemote::MARMALADE_REMOTE_PORT);

        for (int i = 0; i < RemoteButtonIDs::BUTTON_COUNT; i++)
            m_BtnState[i] = false;

        for (int i = 0; i < RemoteAxisIDs::AXIS_COUNT; i++)
            m_AxisValue[i] = 0.0;

        return m_connected;

        //todo: zero conf here, for now just wait for any data on the port
    }

    void CIwGameControllerMarmaladeRemote::Disconnect()
    {
        m_socket.Close();
    }

    bool CIwGameControllerMarmaladeRemote::IsConnected()
    {
        return m_connected;
    }
    
    // -------- input checking functions  ---------------

    void CIwGameControllerMarmaladeRemote::StartFrame()
    {
        if (m_connected)
        {
            int packetsRead = 0;
            char* data;
            data = new char[CIwGameControllerMarmaladeRemote::MARMALADE_REMOTE_PACKET_SIZE+1];
            data[0] = '\0';
            
            while (m_maxPackets == 0 || packetsRead <= m_maxPackets)
            {
                char lastSender[50] = "[error]"; // TODO: def this out if not debugging

                sockaddr_in from;
                socklen_t fromLength = sizeof(from);

                SocketsUDP::SocketAddress sender = SocketsUDP::SocketAddress();
                int bytes = m_socket.ReceiveFrom(sender, (char*)data, CIwGameControllerMarmaladeRemote::MARMALADE_REMOTE_PACKET_SIZE+1);

                if (bytes <= 0)
                    break;

                packetsRead++;

                if (bytes != CIwGameControllerMarmaladeRemote::MARMALADE_REMOTE_PACKET_SIZE)
                    continue;

                // TODO: the true/false values ought to be 1 char of bits - currently just "0" or "1"

                int i = 0;
                for (; i < RemoteButtonIDs::BUTTON_COUNT; i++)
                {
                    if (!data[i])
                    {
                        // error print. if erroneous then just abandon as packet must be wrong, try next packet
                        break;
                    }
                    
                    if (data[i] == '1')
                        m_BtnState[i] = true;
                    else
                        m_BtnState[i] = false;
                }

                for (int j = 0; j < RemoteAxisIDs::AXIS_COUNT; j++)
                {
                    if (!data[j])
                    {
                        // error print
                        break;
                    }

                    char realString[CIwGameControllerMarmaladeRemote::AXIS_VALUE_LENGTH + 1];
                    strncpy(realString, data + i, CIwGameControllerMarmaladeRemote::AXIS_VALUE_LENGTH);
                    realString[CIwGameControllerMarmaladeRemote::AXIS_VALUE_LENGTH] = '\0';

                    m_AxisValue[j] = (float)strtod(realString, NULL) - 1.0;

                    i += CIwGameControllerMarmaladeRemote::AXIS_VALUE_LENGTH;
                }
                
                sender.GetAddressString(lastSender);
                IwTrace(GAME_CONTROLLER, ("Got data (%s) from sender (%s)", data, lastSender));
            }

            delete data;
        }

        /* Do events by just calling registered functions rather than setting flags to poll
        if (s_ButtonCallback)
        {
            
            
        }
        */
    }

    CIwGameControllerHandle* CIwGameControllerMarmaladeRemote::GetControllerByIndex(int index)
    {
        return NULL; // just one controller connected at a time for now
    }

    CIwGameControllerHandle* CIwGameControllerMarmaladeRemote::GetControllerByPlayer(int player)
    {
        return NULL;
    }

    int CIwGameControllerMarmaladeRemote::GetControllerCount()
    {
        return 1; //TODO return 1 if controller connected
    }

    int CIwGameControllerMarmaladeRemote::GetMaxControllers()
    {
        return 1;
    }

	int CIwGameControllerMarmaladeRemote::GetProperty(CIwGameControllerHandle* handle, Property::eProperty prop)
	{
		switch (prop)
		{
		default:
			return -1;
		}
	}

	void CIwGameControllerMarmaladeRemote::SetProperty(CIwGameControllerHandle* handle, Property::eProperty prop, int value)
	{
		switch (prop)
		{
		default:
			break;
		}
	}

	ControllerType::eControllerType CIwGameControllerMarmaladeRemote::GetControllerType(CIwGameControllerHandle* handle)
	{
        // always emulator Siri Remote for now
		return ControllerType::MICRO;
	}

    bool CIwGameControllerMarmaladeRemote::GetButtonState(CIwGameControllerHandle* handle, Button::eButton button)
    {
        // only supporting A, X and START for siri remote for now. Start === menu.
		switch (button)
        {
        case Button::A:
            return m_BtnState[RemoteButtonIDs::A];
        case Button::X:
            return m_BtnState[RemoteButtonIDs::X];
        case Button::START:
            return m_BtnState[RemoteButtonIDs::START];
        case Button::DPAD_TOUCH:
            return m_BtnState[RemoteButtonIDs::DPAD_TOUCH];
        default:
            return false;
        }
    }

	float CIwGameControllerMarmaladeRemote::GetAxisValue(CIwGameControllerHandle* handle, Axis::eAxis axis)
    {
        switch (axis)
        {
        case Axis::DPAD_X:
            return m_AxisValue[RemoteAxisIDs::DPAD_X];
        case Axis::DPAD_Y:
            return m_AxisValue[RemoteAxisIDs::DPAD_Y];
        default:
            return 0.0;
        }
    }
    
    
    bool CIwGameControllerMarmaladeRemote::IsButtonSupported(CIwGameControllerHandle* handle, Button::eButton button)
    {
        switch (button)
        {
        case Button::A:
        case Button::X:
        case Button::START:
            return true;
        default:
            return false;
        }    
    }
    
    bool CIwGameControllerMarmaladeRemote::IsAxisSupported(CIwGameControllerHandle* handle, Axis::eAxis axis)
    {
        switch (axis)
        {
        case Axis::DPAD_X:
        case Axis::DPAD_Y:
            return true;
        default:
            return false;
        }
    }

}   // namespace IwGameController

