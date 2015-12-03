
#ifndef SOCKETS_UDP_H
#define SOCKETS_UDP_H

#include "s3e.h"
#include "IwDebug.h"

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

#define SOCKETSUDP_IP_ADDRESS_MAX_LENGTH 46 //45 char max possible plus null term

namespace SocketsUDP
{

    //Simple to use container for holding socket addresses
    class SocketAddress
    {
    private:
        unsigned short m_port;
        unsigned int m_address;
        sockaddr_in* m_sockaddr;

    public:
        
        SocketAddress();
        SocketAddress(const SocketAddress& other);
        SocketAddress(const char* addressString, unsigned short port);
        SocketAddress(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port);
        SocketAddress(unsigned int address, unsigned short port);
        SocketAddress(sockaddr_in* other);
        ~SocketAddress();
        void CleanUp();
        bool IsValid();
        
        sockaddr_in* GetAddress() const;
        unsigned short GetPort() const;
        bool GetAddressString(char* output);
        
        bool operator == (const SocketAddress & other) const;
        bool operator != (const SocketAddress & other) const;
        SocketAddress & operator=(const SocketAddress &other);
    };

    // Class to manage connect/open, disconnect/close, send and recieve data.
    class SocketUDP
    {
    private:
        int m_socket;

    public:

        SocketUDP(); //Socket does nothing useful until Open() is called
        ~SocketUDP();
        bool Open(unsigned short port);
        void Close();
        bool IsOpen() const;
        bool SendTo(const SocketAddress & destination, const void* packetData, int packetSize);
        int ReceiveFrom(SocketAddress & sender, void * data, int size);
    };

    // Get primary IP address of this device
    // Might just be able to use getlocalhost not... this always works anyway
    bool GetPrimaryAddr(in_addr &result);
}

#endif //SOCKETS_UDP_H
