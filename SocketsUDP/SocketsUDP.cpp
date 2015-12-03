
#include "SocketsUDP.h"

namespace SocketsUDP
{
    //SocketAddress class implementation

    SocketAddress::SocketAddress()
    {
        m_port = 0;
        m_address = 0;

        m_sockaddr = NULL;
        IwTrace(SOCKETUDP, ("end def constructor"));
    }

    SocketAddress::SocketAddress(const SocketAddress& other)
    {
        IwTrace(SOCKETUDP, ("copy sconstruct"));
        this->m_port = other.m_port;
        this->m_address = other.m_address;

        m_sockaddr = new sockaddr_in;
        *m_sockaddr = *other.m_sockaddr;
        IwTrace(SOCKETUDP, ("copy sconstruct done"));
    }

    SocketAddress::SocketAddress(const char* addressString, unsigned short port)
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

    SocketAddress::SocketAddress(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port)
    {
        this->m_sockaddr = new sockaddr_in;
        this->m_sockaddr->sin_family = AF_INET;
        this->m_address = (a << 24) | (b << 16) | (c << 8) | d;
        this->m_sockaddr->sin_addr.s_addr = htonl(this->m_address);
        this->m_sockaddr->sin_port = htons(port);
        this->m_port = port;
        memset(&this->m_sockaddr->sin_zero, 0, sizeof(this->m_sockaddr->sin_zero));
    }

    SocketAddress::SocketAddress(unsigned int address, unsigned short port)
    {
        this->m_sockaddr = new sockaddr_in;
        this->m_sockaddr->sin_family = AF_INET;
        this->m_sockaddr->sin_addr.s_addr = htonl(address);
        this->m_sockaddr->sin_port = htons(port);
        this->m_address = address;
        this->m_port = port;
        memset(&this->m_sockaddr->sin_zero, 0, sizeof(this->m_sockaddr->sin_zero));
    }

    SocketAddress::SocketAddress(sockaddr_in* other)
    {
        IwTrace(SOCKETUDP, ("other construct"));
        m_sockaddr = new sockaddr_in;
        *m_sockaddr = *other;
        m_address = ntohl(other->sin_addr.s_addr);
        m_port = ntohs(other->sin_port);
        IwTrace(SOCKETUDP, ("other construct end"));
    }

    SocketAddress::~SocketAddress()
    {
        IwTrace(SOCKETUDP, ("destructor"));
        if (m_sockaddr)
        {
            delete m_sockaddr;
            m_sockaddr = NULL;
            IwTrace(SOCKETUDP, ("destructor NULLing"));
        }
    }
    
    bool SocketAddress::IsValid()
    {
        return this->m_sockaddr != NULL;
    }
    
    //TODO: this is terrible code! We should get rid of the new call
    //on the address. I used it to allo some pointer passing but bad idea really!
    //This funciton will clean the class up in its on the stack
    //(needed to avoid asserts triggered before destructor calls)
    void SocketAddress::CleanUp()
    {
        if (this->m_sockaddr)
            delete this->m_sockaddr;
        this->m_sockaddr = NULL;
    }

    sockaddr_in* SocketAddress::GetAddress() const
    {
        return m_sockaddr;
    }

    unsigned short SocketAddress::GetPort() const
    {
        return m_port;
    }

    bool SocketAddress::GetAddressString(char* output)
    {
        return inet_ntop(AF_INET, (void*)&(GetAddress()->sin_addr), output, INET_ADDRSTRLEN) != NULL;
    }

    bool SocketAddress::operator == (const SocketAddress & other) const
    {
        return m_address == other.m_address && m_port == other.m_port;
    }

    bool SocketAddress::operator != (const SocketAddress & other) const
    {
        return !(*this == other);
    }

    SocketAddress & SocketAddress::operator=(const SocketAddress &other)
    {
        IwTrace(SOCKETUDP, ("operator="));

        if (this == &other)
            return *this;

        IwTrace(SOCKETUDP, ("op = 1"));

        this->m_port = other.m_port;
        this->m_address = other.m_address;

        IwTrace(SOCKETUDP, ("op = 2"));

        if (this->m_sockaddr)
        {
            IwTrace(SOCKETUDP, ("op = 3a"));
            delete this->m_sockaddr;
            IwTrace(SOCKETUDP, ("op = 3b"));
        }

        IwTrace(SOCKETUDP, ("op = 4"));
        m_sockaddr = new sockaddr_in;
        *this->m_sockaddr = *other.m_sockaddr;

        IwTrace(SOCKETUDP, ("op = 5"));

        return *this;
    }


    // SocketUDP class implementation

    SocketUDP::SocketUDP()
    {
        m_socket = -1;
    }

    SocketUDP::~SocketUDP()
    {
        Close();
    }

    bool SocketUDP::Open(unsigned short port)
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

    void SocketUDP::Close()
    {
        if (this->m_socket != -1)
        {
            close(this->m_socket);
            this->m_socket = -1;
        }
    }

    bool SocketUDP::IsOpen() const
    {
        return this->m_socket != -1;
    }

    /*
    bool SocketUDP::SendTo(const sockaddr* destination, const void* packetData, int packetSize)
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

    bool SocketUDP::SendTo(const SocketAddress & destination, const void* packetData, int packetSize)
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

    int SocketUDP::ReceiveFrom(SocketAddress & sender, void * data, int size)
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


    //Static helper

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

}
