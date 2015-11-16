#include "s3e.h"
#include "IwDebug.h"
#include "Iw2DSceneGraph.h"
#include "IwGx.h"

// ---- Rendering ----
using namespace Iw2DSceneGraphCore;
using namespace Iw2DSceneGraph;

// ---- Connection ----

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <fcntl.h>

// ---- Connections ----

class SocketAddress
{
private:
    unsigned short m_port;
    unsigned int m_address;
    sockaddr_in* m_sockaddr;

public:
    
    SocketAddress()
    {
        m_port = 0;
        m_address = 0;
        m_sockaddr = NULL;
    }

    SocketAddress(const SocketAddress& other)
    {
        this->m_port = other.m_port;
        this->m_address = other.m_address;

        m_sockaddr = new sockaddr_in;
        *m_sockaddr = *other.m_sockaddr;
    }

    SocketAddress(const char* addressString, unsigned short port)
    {
        if (!addressString)
        {
            this->m_sockaddr = NULL;
            return;
        }

        this->m_sockaddr = new sockaddr_in;
        this->m_sockaddr->sin_family = AF_INET;
        
        if (inet_pton(AF_INET, addressString, &(this->m_sockaddr->sin_addr)) != 1)
        {
            delete this->m_sockaddr;
            this->m_sockaddr = NULL;
            return;
        }
        this->m_sockaddr->sin_port = htons(port);
        this->m_port = port;
        this->m_address = ntohl(this->m_sockaddr->sin_addr.s_addr);

        memset(&this->m_sockaddr->sin_zero, 0, sizeof(this->m_sockaddr->sin_zero)); // just in case. Reports of this not being nulled causing bugs as it's used for some offset
    }

    SocketAddress(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port)
    {
        this->m_sockaddr = new sockaddr_in;
        this->m_sockaddr->sin_family = AF_INET;
        this->m_address = (a << 24) | (b << 16) | (c << 8) | d;
        this->m_sockaddr->sin_addr.s_addr = htonl(this->m_address);
        this->m_sockaddr->sin_port = htons(port);
        this->m_port = port;
        memset(&this->m_sockaddr->sin_zero, 0, sizeof(this->m_sockaddr->sin_zero));
    }

    SocketAddress(unsigned int address, unsigned short port)
    {
        this->m_sockaddr = new sockaddr_in;
        this->m_sockaddr->sin_family = AF_INET;
        this->m_sockaddr->sin_addr.s_addr = htonl(address);
        this->m_sockaddr->sin_port = htons(port);
        this->m_address = address;
        this->m_port = port;
        memset(&this->m_sockaddr->sin_zero, 0, sizeof(this->m_sockaddr->sin_zero));
    }

    SocketAddress(sockaddr_in* other)
    {
        m_sockaddr = new sockaddr_in;
        *m_sockaddr = *other;
        m_address = ntohl(other->sin_addr.s_addr);
        m_port = ntohs(other->sin_port);
    }

    ~SocketAddress()
    {
        if (this->m_sockaddr)
            delete this->m_sockaddr;
    }

    sockaddr_in* GetAddress() const
    {
        return m_sockaddr;
    }

    unsigned short GetPort() const
    {
        return m_port;
    }

    bool GetAddressString(char* output)
    {
        char testStr[100] = { 0 };
        inet_ntop(AF_INET, (void*)&(GetAddress()->sin_addr), testStr, INET_ADDRSTRLEN);

        return inet_ntop(AF_INET, (void*)&(GetAddress()->sin_addr), output, INET_ADDRSTRLEN) != NULL;
    }

    bool operator == (const SocketAddress & other) const
    {
        return m_address == other.m_address && m_port == other.m_port;
    }

    bool operator != (const SocketAddress & other) const
    {
        return !(*this == other);
    }

    SocketAddress & operator=(const SocketAddress &other)
    {
        if (this == &other)
            return *this;

        this->m_port = other.m_port;
        this->m_address = other.m_address;

        if (this->m_sockaddr)
            delete this->m_sockaddr;
        m_sockaddr = new sockaddr_in;
        *this->m_sockaddr = *other.m_sockaddr;

        return *this;
    }
};


class SocketUDP
{
private:
    int m_socket;

public:

    SocketUDP()
    {
        m_socket = -1;
    }

    ~SocketUDP()
    {
        Close();
    }

    bool Open(unsigned short port)
    {
        IwAssert(REMOTE, (port > 1024));
        IwAssert(REMOTE, (!IsOpen()));

        this->m_socket = socket(AF_INET, SOCK_DGRAM, 0); //UDP

        if (this->m_socket == -1)
        {
            IwTrace(REMOTE, ("socket creation failed: %s", strerror(errno)));
            return false;
        }

        //int yes = 1;
        //setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)); //allows multiple sockets to same port

        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;  //dont care what address it comes from
        addr.sin_port = htons((unsigned short)port);

        int rtn = bind(this->m_socket, (sockaddr*)&addr, sizeof(sockaddr_in));
        if (rtn == -1)
        {
            IwTrace(REMOTE, ("bind socket failed: %s", strerror(errno)));
            Close();
            return false;
        }

        // Set non blocking otherwise receive calls wait till data is available!
        int nonBlocking = 1;
        if (fcntl(this->m_socket, F_SETFL, O_NONBLOCK, nonBlocking) == -1)
        {
            IwTrace(REMOTE, ("failed to set non-blocking"));
            Close();
            return false;
        }

        return true;
    }

    void Close()
    {
        if (this->m_socket != -1)
        {
            close(this->m_socket);
            this->m_socket = -1;
        }
    }

    bool IsOpen() const
    {
        return this->m_socket != -1;
    }
    /*
    bool SendTo(const sockaddr* destination, const void* packetData, int packetSize)
    {
        int sent_bytes = sendto(m_socket, (const char*)packetData, packetSize, 0, (sockaddr*)&destination, sizeof(sockaddr_in));

        if (sent_bytes != packetSize)
        {
            IwTrace(REMOTE, ("failed to send packet"));
            return false;
        }

        return true;
    }
    */
    bool SendTo(const SocketAddress & destination, const void* packetData, int packetSize)
    {
        int sent_bytes = sendto(m_socket, (const char*)packetData, packetSize, 0,
            (sockaddr*)destination.GetAddress(), sizeof(sockaddr_in));
        
        if (sent_bytes != packetSize)
        {
            IwTrace(REMOTE, ("failed to send packet"));
            return false;
        }

        return true;
    }

    int ReceiveFrom(SocketAddress & sender, void * data, int size)
    {
        IwAssert(REMOTE, (data));
        IwAssert(REMOTE, (size > 0));
        IwAssert(REMOTE, (IsOpen()));

        sockaddr_in* from = new sockaddr_in;
        socklen_t fromLength = sizeof(sockaddr_in);

        int received_bytes = recvfrom(m_socket, (char*)data, size, 0, (sockaddr*)from, &fromLength);

        if (received_bytes <= 0)
            return 0;

        sender = SocketAddress(from);

        return received_bytes;
    }
};

// --------------------------------------------------------------

#define MARMALADE_REMOTE_PORT 1099
#define MAX_PACKETS 5

#define MAX_DATA_SIZE 256
char g_LastData[MAX_DATA_SIZE] = "";
char g_LastSender[50] = "";


bool ReceiveData(SocketUDP &socket, int maxPackets)
{
    int packetsRead = 0;
    
    while (maxPackets == 0 || packetsRead <= maxPackets)
    {
        sockaddr_in from;
        socklen_t fromLength = sizeof(from);

        SocketAddress sender = SocketAddress();
        int bytes = socket.ReceiveFrom(sender, (char*)g_LastData, MAX_DATA_SIZE);

        if (bytes <= 0)
            break;

        packetsRead++;

        //unsigned int from_address = ntohl(from.sin_addr.s_addr);
        //unsigned int from_port = ntohs(from.sin_port);
        
        // Would process data here. Likely use case is to do simple "handshake" connection
        // and check for still alive status periodically

        sender.GetAddressString(g_LastSender);
    }

    return packetsRead > 0;

}

// Get primary IP address of this device
// Not sure why we ant just use getlocalhost... maybe we can now, needs testing
bool GetPrimaryAddr(in_addr &result)
{
    // Create UDP socket for querying
    int sock = socket(AF_INET, SOCK_DGRAM, 0); // SOCK_DGRAM -> UDP

    if (sock <= 0)
        return false;

    const char* kOutboundIp = "1.2.3.4"; //not sure why we pass this. I guess it needs some address to be set.
    uint16_t kDnsPort = 53; //53 = DNS lookup port

    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr(kOutboundIp);
    serv.sin_port = htons(kDnsPort);

    int err = connect(sock, (const sockaddr*)&serv, sizeof(serv));

    if (err == -1)
        return false;

    sockaddr_in name;
    socklen_t namelen = sizeof(name);
    err = getsockname(sock, (sockaddr*)&name, &namelen);

    close(sock);

    result = name.sin_addr;

    if (err == -1)
        return false;
    else
        return true;
}

// ----------------------------------------------------------

int main()
{
    //Initialise graphics system(s)
    //Iw2DInit();

    // Create root node
    //CNode* g_SceneRoot = new CNode();

    // Add 2D scene graph nodes to the root node here

    // ---------- connection setup -----------

    sockaddr_in test;
    test.sin_family = AF_INET;
    unsigned int testInt = (123 << 24) | (45 << 16) | (37 << 8) | 12;
    test.sin_addr.s_addr = htonl(testInt);
    test.sin_port = htons(1749);

    char testStr[100] = {0};
    inet_ntop(AF_INET, (void*)&test.sin_addr, testStr, INET_ADDRSTRLEN);
    printf("TEST:%s\n", testStr);


    bool gotConnection = false;
    char lastConnectedAddr[50] = "None";
    bool seriousErrorState = false; // Use to force app restart if needed

    // Get host IP address of this device (creates a socket, does a query, closes socket)
    in_addr localAddr;
    seriousErrorState = !GetPrimaryAddr(localAddr);

    // create socket to listen for incomming conections
    SocketUDP socket = SocketUDP();

    if (!seriousErrorState)
        seriousErrorState = !socket.Open(MARMALADE_REMOTE_PORT);

    // Set up address to send to

    // For UDP we just push to an IP address.
    // This needs to be picked up through zeroconf in final version, for now just hard code
    char targetIP[50]; //IP sting max length = 45
    SocketAddress targetAddress;

    if (s3eConfigGetString("ControllerRemote", "TargetIPAddress", targetIP) == S3E_RESULT_ERROR)
        seriousErrorState = true;
    else
    {
        targetAddress = SocketAddress(targetIP, MARMALADE_REMOTE_PORT);
    }

    // ----------------------
    
    char testPacket[32] = "[NONE]";
    int testPacketSize = sizeof(testPacket);
    int testPacketCount = 0;

    // Loop forever, until the user or the OS performs some action to quit the app
    while (!s3eDeviceCheckQuitRequest())
    {
        //Update the input systems
        s3eKeyboardUpdate();
        s3ePointerUpdate();

        s3eSurfaceClear(0, 0, 255);

        if (seriousErrorState)
        {
            s3eDebugPrintf(10, 40, 1, "Error setting up sockets... try restarting");
            s3eDeviceYield;
            s3eSurfaceShow();
            continue;
        }

        s3eDebugPrintf(20, 20, 1, "Local host address: %s", inet_ntoa(localAddr));
        s3eDebugPrintf(20, 50, 1, "Sending data to:");
        char ip[50] = { 0 };
        targetAddress.GetAddressString(ip);
        s3eDebugPrintf(20, 70, 1, "   %s:%d", ip, targetAddress.GetPort());

        // send about once a second for testing
        if (testPacketCount % 60 == 0)
        {
            sprintf(testPacket, "TEST:%d", testPacketCount);
            socket.SendTo(targetAddress, (const char*)testPacket, testPacketSize);
        }

        s3eDebugPrintf(20, 100, 1, "Last sent packet: '%s'", testPacket);

        // process any received packets in internal queue
        ReceiveData(socket, MAX_PACKETS);

        s3eDebugPrintf(20, 130, 1, "Last received packet: '%s'", g_LastData);
        s3eDebugPrintf(20, 150, 1, "    from: '%s'", g_LastSender);

        testPacketCount++;
        
        s3eSurfaceShow();
        

        //Update the scene. The SDK's example framework has a fixed
        //framerate of 20fps, so we pass that duration to the update function.
        //g_SceneRoot->Update(1000/20);

        //Iw2DSurfaceClear(0xff00ff00);
        
        // Your rendering/app code goes here.

        //g_SceneRoot->Render();

        //Draws Surface to screen
        //Iw2DSurfaceShow();

        // Sleep for 0ms to allow the OS to process events etc.
        s3eDeviceYield(0);
    }

    //Terminate modules being used
    //delete g_SceneRoot;
    //Iw2DTerminate();
    
    // Return
    return 0;
}
