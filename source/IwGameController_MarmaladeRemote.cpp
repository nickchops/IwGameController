/*
 * (C) 2015 Nick Smith.
 */

#include "IwGameController_MarmaladeRemote.h"
#include "IwDebug.h"
#include "s3eTimer.h"

namespace IwGameController
{
    const int CIwGameControllerMarmaladeRemote::MARMALADE_REMOTE_PORT = 1099;
    const int CIwGameControllerMarmaladeRemote::AXIS_VALUE_LENGTH = 7; // "x.xxxxx" unit plus . plus 5 points precision
    const int CIwGameControllerMarmaladeRemote::MARMALADE_REMOTE_PACKET_SIZE = RemoteButtonIDs::BUTTON_COUNT + RemoteAxisIDs::AXIS_COUNT*AXIS_VALUE_LENGTH + 1;
    const int CIwGameControllerMarmaladeRemote::IGNORE_CHECK_TIMEOUT = 1000;

    // used to identify in zeroconf connections
    const char* CIwGameControllerMarmaladeRemote::SENDER_NAME = "MarmaladeControllerSender";
    const char* CIwGameControllerMarmaladeRemote::RECEIVER_NAME = "MarmaladeControllerReceiver";
    const char* CIwGameControllerMarmaladeRemote::CONNECT_TOKEN = "marm_connect";
    const char* CIwGameControllerMarmaladeRemote::DISCONNECT_TOKEN = "marm_disconn";
    const char* CIwGameControllerMarmaladeRemote::KEEPCONNECT_TOKEN = "marm_keepcon";
    const char* CIwGameControllerMarmaladeRemote::IGNORETIMEOUT_TOKEN = "marm_ignore";
    const char* CIwGameControllerMarmaladeRemote::NOSEND_TOKEN = "marm_nosend";

    // Init/term

    CIwGameControllerMarmaladeRemote::CIwGameControllerMarmaladeRemote()
    {
        m_Type = Type::MARMALADE_REMOTE;
        m_Connected = false;
        m_Connecting = 0;
        m_MaxPackets = 8;
        m_Service = NULL;
        m_Search = 0;
        m_ConnectTime = 0;
        m_IgnoreCheckTime = 0;
        m_IgnoreAllTimeouts = false;
        m_BlockSender = false;
        m_ConnectTimeout = 5000;
        m_KeepAliveTimeout = 5000;
        m_SenderAddressString[0] = '\0';
    }

    CIwGameControllerMarmaladeRemote::~CIwGameControllerMarmaladeRemote()
    {
        Disconnect();
    }


    // -------- connection  ---------------

    bool CIwGameControllerMarmaladeRemote::Connect(bool dontBroadcast, const char* appName)
    {
        Disconnect();

        for (int i = 0; i < RemoteButtonIDs::BUTTON_COUNT; i++)
            m_BtnState[i] = false;

        for (int i = 0; i < RemoteAxisIDs::AXIS_COUNT; i++)
            m_AxisValue[i] = 0.0;

        if (!m_Socket.Open(CIwGameControllerMarmaladeRemote::MARMALADE_REMOTE_PORT))
            return false;

        if (dontBroadcast)
        {
            m_Connecting = 3; //go straight to waiting for first real packet from any address
        }
        else
        {
            IwAssertMsg(REMOTE, appName, ("CIwGameControllerMarmaladeRemote app Name is NULL"));

            in_addr localAddr;
            if(!SocketsUDP::GetPrimaryAddr(localAddr))
                return false;

            char localAddrString[SOCKETSUDP_IP_ADDRESS_MAX_LENGTH];
            sprintf(localAddrString, inet_ntoa(localAddr));

            const char* whatAmI = "Marmalade Remote Game Controller";
            const char* friendlyName = s3eDeviceGetString(S3E_DEVICE_NAME);
            char timeout[6];
            //strnprintf(timeout, )
            const char* textRecords[3] = { whatAmI, friendlyName, appName };

            // Not all implementations handle name collisions so try to avoid clashes
            char serviceName[255];
            snprintf(serviceName, 255, "%s(%s)", CIwGameControllerMarmaladeRemote::RECEIVER_NAME, localAddrString);

            // create a service (so remote apps can find us)
            m_Service = s3eZeroConfPublish(CIwGameControllerMarmaladeRemote::MARMALADE_REMOTE_PORT,
                                           serviceName,
                                           "_http._udp", NULL, 3, textRecords);
            if (!m_Service)
                return false;

            printf("STARTING ZEROCONF... service: %s\n", serviceName);

            m_Connecting = 1;
        }

        return true;
    }

    void CIwGameControllerMarmaladeRemote::Disconnect()
    {
        if (m_Service)
        {
            s3eZeroConfUnpublish(m_Service);
            m_Service = NULL;
        }

        if (m_Socket.IsOpen())
        {
            if (m_SenderAddress.IsValid()) //UDP so not guaranteed to arrive. Sender will timeout if not.
            {
                m_Socket.SendTo(m_SenderAddress, CIwGameControllerMarmaladeRemote::DISCONNECT_TOKEN,
                    strlen(CIwGameControllerMarmaladeRemote::DISCONNECT_TOKEN) + 1);
            }
            m_Socket.Close();
        }
    }

    bool CIwGameControllerMarmaladeRemote::IsConnecting()
    {
        return m_Connecting > 0;
    }

    bool CIwGameControllerMarmaladeRemote::IsConnected()
    {
        return m_Connected;
    }

    void CIwGameControllerMarmaladeRemote::SetConnectTimeout(float seconds)
    {
        m_ConnectTimeout = (int)(1000.0 * seconds);
    }

    void CIwGameControllerMarmaladeRemote::SetKeepAliveTimeout(float seconds)
    {
        m_KeepAliveTimeout = (int)(1000.0 * seconds);
    }

    void CIwGameControllerMarmaladeRemote::SetIgnoreTimeouts(bool ignore, bool blockSender)
    {
        if (m_IgnoreAllTimeouts == ignore)
            return;

        m_IgnoreAllTimeouts = ignore;
        m_BlockSender = blockSender;

        // send once now incase no further StartFrames, then keep sending...
        if (m_Connected)
        {
            if (ignore)
            {
                m_IgnoreCheckTime = 0;

                if (m_BlockSender)
                    m_Socket.SendTo(m_SenderAddress, CIwGameControllerMarmaladeRemote::NOSEND_TOKEN,
                        strlen(CIwGameControllerMarmaladeRemote::NOSEND_TOKEN) + 1);
                else
                    m_Socket.SendTo(m_SenderAddress, CIwGameControllerMarmaladeRemote::IGNORETIMEOUT_TOKEN,
                        strlen(CIwGameControllerMarmaladeRemote::IGNORETIMEOUT_TOKEN) + 1);
            }
            else
            {
                m_ConnectTime = s3eTimerGetUST();
                m_Socket.SendTo(m_SenderAddress, CIwGameControllerMarmaladeRemote::KEEPCONNECT_TOKEN,
                    strlen(CIwGameControllerMarmaladeRemote::KEEPCONNECT_TOKEN) + 1);
            }
        }
    }

    void CIwGameControllerMarmaladeRemote::ResetValues()
    {
        for (int i = 0; i < RemoteButtonIDs::BUTTON_COUNT; i++)
        {
            m_BtnState[i] = false;
        }

        for (int j = 0; j < RemoteAxisIDs::AXIS_COUNT; j++)
        {
            m_AxisValue[j] = 0.0;
        }

        //TODO: send reset message to sender instead seems more useful!
    }

    // -------- input checking functions  ---------------

    void CIwGameControllerMarmaladeRemote::StartFrame()
    {
        if (m_Connecting == 2)
        {
            if (s3eTimerGetUST() - m_ConnectTime > m_ConnectTimeout)
            {
                // -- ATTEMPTED CONNECTION FAILED (timeout), ALLOW ANOTHER SENDER TO CONNECT --

                m_Connecting = 1;
                // give up and allow other remote apps to try to connect
            }
            else
            {
                // send the senders address back to it. Sender will know it succeeds when it
                // gets this back from the address it wants to connect to
                m_Socket.SendTo(m_SenderAddress, (const char*)m_SenderAddressString,
                              strlen(m_SenderAddressString)+1);

                IwTrace(GAMECONTROLLER, ("Connecting to sender: %s:%d", m_SenderAddressString,
                                         m_SenderAddress.GetPort()));
            }
        }

        int packetsRead = 0;
        char* data;
        data = new char[CIwGameControllerMarmaladeRemote::MARMALADE_REMOTE_PACKET_SIZE+1]; //+1 is standard socket behaviour - can get odd padded packets otherwise

        while (m_Socket.IsOpen() && (m_MaxPackets == 0 || packetsRead <= m_MaxPackets))
        {
            data[0] = '\0';

            sockaddr_in from;
            socklen_t fromLength = sizeof(from);

            SocketsUDP::SocketAddress sender = SocketsUDP::SocketAddress();
            int bytes = m_Socket.ReceiveFrom(sender, data, CIwGameControllerMarmaladeRemote::MARMALADE_REMOTE_PACKET_SIZE+1);

            if (bytes <= 0)
                break;

            //make sure has some terminator for printing
            data[CIwGameControllerMarmaladeRemote::MARMALADE_REMOTE_PACKET_SIZE-1] = '\0';

            packetsRead++;

            // Explicit disconnect request
            if (m_Connected && strncmp(data, CIwGameControllerMarmaladeRemote::DISCONNECT_TOKEN, strlen(CIwGameControllerMarmaladeRemote::DISCONNECT_TOKEN)) == 0)
            {
                m_Connecting = 0;
                m_Connected = false;
                m_SenderAddressString[0] = '\0';
                Disconnect();
                this->NotifyDisconnect((CIwGameControllerHandle*)m_SenderAddressString);
            }

            // Respond to a connection check from sender
            else if(strncmp(data, CIwGameControllerMarmaladeRemote::KEEPCONNECT_TOKEN, strlen(CIwGameControllerMarmaladeRemote::KEEPCONNECT_TOKEN)) == 0)
            {
                if (m_Connected && m_SenderAddress == sender)
                {
                    const char* msg = m_BlockSender ? CIwGameControllerMarmaladeRemote::NOSEND_TOKEN : (m_IgnoreAllTimeouts ? CIwGameControllerMarmaladeRemote::IGNORETIMEOUT_TOKEN : CIwGameControllerMarmaladeRemote::KEEPCONNECT_TOKEN);
                    m_Socket.SendTo(sender, msg, strlen(msg)+1);
                }
                else
                {
                    m_Socket.SendTo(sender, CIwGameControllerMarmaladeRemote::DISCONNECT_TOKEN,
                                    strlen(CIwGameControllerMarmaladeRemote::DISCONNECT_TOKEN)+1);
                }
                // Don't mind if these messages go missing... eventually will happen and sender can disconnect
            }

            if (m_Connecting == 1)
            {
                IwTrace(GAMECONTROLLER, ("Have data, checkinng for connect token..."));

                if (sender.GetAddressString(m_SenderAddressString)) //might want to make these string check only for debug and use int addresses otherwise
                {
                    m_SenderAddress = sender;
                    if(strncmp(data, CIwGameControllerMarmaladeRemote::CONNECT_TOKEN, strlen(CIwGameControllerMarmaladeRemote::CONNECT_TOKEN)) == 0)
                    {
                        IwTrace(GAMECONTROLLER, ("Found connect token!"));

                        m_Connecting = 2;
                        m_ConnectTime = s3eTimerGetUST();
                        // now we send unique (ip) token back to sender until it starts
                        // sending real data
                        break; //no point reading more until start sending tokens
                    }
                }
                continue;
            }

            // only now care if connected/ing and got valid packet size
            if ((!m_Connected && m_Connecting == 0) || bytes != CIwGameControllerMarmaladeRemote::MARMALADE_REMOTE_PACKET_SIZE-1)
                continue;

            // assume we have valid data once its the right size and connecting = 2 for first time
            if (m_Connecting) // will be 2 for zeroconf, 3 for accepting from any sender.
            {
                if (m_Connecting == 3)
                {
                    IwAssert(!m_SenderAddressString[0], ("Sender address already stored on connection found when not using dontBroadcast!"));

                    if (!sender.GetAddressString(m_SenderAddressString))
                    {
                        IwTrace(GAMECONTROLLER, ("Failed to get sender address, cant notify connection")); //unlikley to ever happen
                        continue; // try next packet
                    }
                    IwTrace(GAMECONTROLLER, ("Accepting connection from any controller: %s", m_SenderAddressString));
                }
                else
                {
                    if (!m_SenderAddressString[0])
                    {
                        IwAssert(true, ("Connection should be starting but no sender address stored"));
                        continue;
                    }
                    char checkAddr[SOCKETSUDP_IP_ADDRESS_MAX_LENGTH];
                    if (!sender.GetAddressString(checkAddr))
                    {
                        IwTrace(GAMECONTROLLER, ("Failed to verify sender address, cant notify connection")); //unlikley to ever happen
                        continue; // try next packet
                    }

                    if (m_SenderAddress != sender)
                    {
                        IwTrace(GAMECONTROLLER, ("Got data from wrong sender... ignoring"));
                        continue; // try next packet
                    }
                }

                // -- CONNECTION ACHIEVED --
                IwTrace(GAMECONTROLLER, ("Notifying controller connected as first packet was received from: %s", m_SenderAddressString));
                m_Connecting = 0;
                m_Connected = true;

                if (m_Service)
                {
                    s3eZeroConfUnpublish(m_Service);
                    m_Service = NULL;
                }

                //Using this string pointer as the controller indetifier. Could use any pointer. Has side effect that you could cast and print it in game!
                //Mixes OK with iOS handles as both are memory addresses.
                this->NotifyConnect((CIwGameControllerHandle*)m_SenderAddressString);

                //this should be a real controlls data packet... so carry on and read it
            }
            else //connected
            {
                if(m_SenderAddress != sender) //only allow "connected" sender
                    continue;
            }

            m_ConnectTime = s3eTimerGetUST(); //stamp last valid packet time

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
                {
                    if (i == RemoteButtonIDs::START && m_BtnState[RemoteButtonIDs::START] == false)
                        this->NotifyPause((CIwGameControllerHandle*)m_SenderAddressString);

                    m_BtnState[i] = true;
                }
                else
                    m_BtnState[i] = false;
            }

            for (int j = 0; j < RemoteAxisIDs::AXIS_COUNT; j++)
            {
                if (!data[i])
                {
                    // error print
                    break;
                }

                char realString[CIwGameControllerMarmaladeRemote::AXIS_VALUE_LENGTH + 1];
                strncpy(realString, data + i, CIwGameControllerMarmaladeRemote::AXIS_VALUE_LENGTH);
                realString[CIwGameControllerMarmaladeRemote::AXIS_VALUE_LENGTH] = '\0';

                m_AxisValue[j] = (float)strtod(realString, NULL) - 1.0;

                if (m_AxisValue[j] > 1.0 || m_AxisValue[j] < -1.0)
                    IwTrace(GAMECONTROLLER, ("WTF?!"));

                i += CIwGameControllerMarmaladeRemote::AXIS_VALUE_LENGTH;
            }
        }

        delete data;

        // Controller always sends data even if its all zeros. After a gap, flag as not connected again.

        if (m_Connected)
        {
            if (m_IgnoreAllTimeouts)
            {
                // Send message regularly in case sender hasnt got it
                m_ConnectTime = s3eTimerGetUST();
                if (m_ConnectTime - m_IgnoreCheckTime > CIwGameControllerMarmaladeRemote::IGNORE_CHECK_TIMEOUT)
                {
                    m_IgnoreCheckTime = m_ConnectTime;
                    if (m_BlockSender)
                        m_Socket.SendTo(m_SenderAddress, CIwGameControllerMarmaladeRemote::NOSEND_TOKEN,
                            strlen(CIwGameControllerMarmaladeRemote::NOSEND_TOKEN) + 1);
                    else
                        m_Socket.SendTo(m_SenderAddress, CIwGameControllerMarmaladeRemote::IGNORETIMEOUT_TOKEN,
                                        strlen(CIwGameControllerMarmaladeRemote::IGNORETIMEOUT_TOKEN) + 1);
                }
            }
            else if (s3eTimerGetUST() - m_ConnectTime > m_KeepAliveTimeout)
            {
                // -- CONNECTION STOPPED (timeout) --

                m_Connecting = 0;
                m_Connected = false;
                m_SenderAddressString[0] = '\0';
                Disconnect();
                this->NotifyDisconnect((CIwGameControllerHandle*)m_SenderAddressString);
            }
        }

        /* Do events by just calling registered functions rather than setting flags to poll
        if (s_ButtonCallback)
        {
        }
        */
    }

    CIwGameControllerHandle* CIwGameControllerMarmaladeRemote::GetControllerByIndex(int index)
    {
        // Just one atm, but does have a handle (memory of sender address)
        if (IsConnected() && index == 0)
            return (CIwGameControllerHandle*)m_SenderAddressString;
        else
            return NULL;
    }

    CIwGameControllerHandle* CIwGameControllerMarmaladeRemote::GetControllerByPlayer(int player)
    {
        return GetControllerByIndex(player);
    }

    int CIwGameControllerMarmaladeRemote::GetControllerCount()
    {
        if (IsConnected())
            return 1;
        else
            return 0;
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
        // always emulate Siri Remote for now...
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
            if ( m_AxisValue[RemoteAxisIDs::DPAD_Y] > 1 || m_AxisValue[RemoteAxisIDs::DPAD_Y] < -1)
                IwTrace(GAMECONTROLLER, ("WTF?!"));

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

