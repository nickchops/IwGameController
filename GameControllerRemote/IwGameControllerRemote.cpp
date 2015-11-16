#include "s3e.h"
#include "IwDebug.h"
#include "Iw2DSceneGraph.h"
#include "IwGx.h"

// ---- Rendering ----
using namespace Iw2DSceneGraphCore;
using namespace Iw2DSceneGraph;

// ---- Connection ----

#include "SocketsUDP.h"
using namespace SocketsUDP;

#include "IwGameController_MarmaladeRemote.h"
using namespace IwGameController;

// --------------------------------------------------------------

//#define MARMALADE_REMOTE_PORT 1099
#define MAX_PACKETS 5

#define MAX_DATA_SIZE 256
char g_LastData[MAX_DATA_SIZE] = "";
char g_LastSender[50] = "";

// suerp simple debug graphics thigns for touch areas for now!
int touchX, touchY, touchW, touchH, aX, aY, xX, xY, menuX, menuY, btnRad;

typedef struct TouchState
{
    int x;
    int y;
    int w;
    int h;
    float touchX;
    float touchY;
    bool pressed;
    int touchID;
} TouchState;

#define TOUCH_AREA_COUNT 4
TouchState touchArea[TOUCH_AREA_COUNT];

int touch = 0;
int aBtn = 1;
int xBtn = 2;
int menuBtn = 3;

int32 TouchEvent(void* systemData, void* userData)
{
    s3ePointerTouchEvent* event = (s3ePointerTouchEvent*)systemData;

    for (int i = 0; i < TOUCH_AREA_COUNT; ++i)
    {
        bool released = touchArea[i].touchID == event->m_TouchID && touchArea[i].pressed && !event->m_Pressed;
        
        if (event->m_x > touchArea[i].x && event->m_x < touchArea[i].x + touchArea[i].w &&
            event->m_y > touchArea[i].y && event->m_y < touchArea[i].y + touchArea[i].h)
        {
            if (!touchArea[i].pressed && event->m_Pressed)
            {
                touchArea[i].pressed = true;
                touchArea[i].touchID = event->m_TouchID;

                if (i == touch)
                {
                    touchArea[i].touchX = (float)(event->m_x - touchArea[i].x) / (float)touchArea[i].w * 2.0; // scale to be 0 -> 2 (instead of 1 -> -1 as the minus adds a digit)
                    touchArea[i].touchY = (float)(event->m_y - touchArea[i].y) / (float)touchArea[i].h * 2.0;
                }
            }
            else if (released)
            {
                if (i == touch)
                {
                    touchArea[i].touchX = (float)(event->m_x - touchArea[i].x) / (float)touchArea[i].w * 2.0;
                    touchArea[i].touchY = (float)(event->m_y - touchArea[i].y) / (float)touchArea[i].h * 2.0;
                    //Leaving the positions at last set after release. A game would typically ignore them
                    //Could reset to zero in next update loop (in StarFrame in IwGameController).
                }
            }
        }

        if (released)
        {
            touchArea[i].pressed = false;
            touchArea[i].touchID = -1;
        }
    }

    return 0;
}

int32 MotionEvent(void* systemData, void* userData)
{
    s3ePointerTouchMotionEvent* event = (s3ePointerTouchMotionEvent*)systemData;

    if (touchArea[touch].pressed && touchArea[touch].touchID == event->m_TouchID &&
        event->m_x > touchArea[touch].x && event->m_x < touchArea[touch].x + touchArea[touch].w &&
        event->m_y > touchArea[touch].y && event->m_y < touchArea[touch].y + touchArea[touch].h)
    {
        touchArea[touch].touchX = (float)(event->m_x - touchArea[touch].x) / (float)touchArea[touch].w * 2.0;
        touchArea[touch].touchY = (float)(event->m_y - touchArea[touch].y) / (float)touchArea[touch].h * 2.0;
    }
    
    return 0;
}
// ----------------------------------------------------------

bool ReceiveData(SocketUDP &socket, int maxPacketSize, int maxPackets) //TODO: better to have timeout that max packet value
{
    int packetsRead = 0;
    
    while (maxPackets == 0 || packetsRead <= maxPackets)
    {
        sockaddr_in from;
        socklen_t fromLength = sizeof(from);

        SocketAddress sender = SocketAddress();
        int bytes = socket.ReceiveFrom(sender, (char*)g_LastData, maxPacketSize);

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

// ----------------------------------------------------------

int main()
{
    //Initialise graphics system(s)
    Iw2DInit();

    // Create root node
    //CNode* g_SceneRoot = new CNode();
    // Add 2D scene graph nodes to the root node here

    // Load the group containing the "arial14" font
    IwResManagerInit();
    IwGetResManager()->LoadGroup("font.group");
    CIw2DFont* font = Iw2DCreateFontResource("arial14");

    // ---------- connection setup -----------

    //bool gotConnection = false;
    //char lastConnectedAddr[50] = "None";
    bool seriousErrorState = false; // Use to force app restart if needed

    // Get host IP address of this device (creates a socket, does a query, closes socket)
    in_addr localAddr;
    seriousErrorState = !GetPrimaryAddr(localAddr);
    char localAddrString[50];
    sprintf(localAddrString, inet_ntoa(localAddr));

    // create socket to listen for incomming conections
    SocketUDP socket = SocketUDP();

    if (!seriousErrorState)
        seriousErrorState = !socket.Open(CIwGameControllerMarmaladeRemote::MARMALADE_REMOTE_PORT);

    // Set up address to send to

    // For UDP we just push to an IP address.
    // This needs to be done through zeroconf in final version, for now just hard code
    char targetIP[50]; //IP sting max length = 45
    SocketAddress targetAddress;

    if (s3eConfigGetString("ControllerRemote", "TargetIPAddress", targetIP) == S3E_RESULT_ERROR)
        seriousErrorState = true;
    else
    {
        targetAddress = SocketAddress(targetIP, CIwGameControllerMarmaladeRemote::MARMALADE_REMOTE_PORT);
    }

    char testPacket[32] = "[NONE]";
    int testPacketSize = sizeof(testPacket);
    int testPacketCount = 0;

    // ----------------------

    // For now, just packing controller state info as simple text string. In reality will want to compress...
    //C++99 wont let us have this on stack due to MARMALADE_REMOTE_PACKET_SIZE. Might want to just use #define!

    int packetSize = CIwGameControllerMarmaladeRemote::MARMALADE_REMOTE_PACKET_SIZE + 1; //+1 for nul terminator. Prob should improve code to not need it.
    char* packetToSend = new char[packetSize];
    packetToSend[0] = '\0';
    

    int precision = CIwGameControllerMarmaladeRemote::AXIS_VALUE_LENGTH - 2; // length=whole number: x.<precision>
    char formatter[11];
    sprintf(formatter, "%%2.%df%%2.%df", precision, precision);

    touchX = touchW = touchH = s3eSurfaceGetInt(S3E_SURFACE_WIDTH)/2;
    int midY = s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)/2;
    touchY = midY - touchH;
    aX = touchX * 1.25;
    aY = midY * 1.3;
    xX = touchX * 1.75;
    xY = aY;
    menuX = xX;
    menuY = midY * 1.6;
    btnRad = touchX * 0.18;

    int height = s3eSurfaceGetInt(S3E_SURFACE_HEIGHT);

    for (int i = 0; i < 4; ++i)
    {
        touchArea[i].pressed = false;
        touchArea[i].touchID = -1;
        touchArea[i].touchX = .0f;
        touchArea[i].touchY = .0f;
    }

    touchArea[touch].x = touchX;
    touchArea[touch].y = touchY;
    touchArea[touch].w = touchW;
    touchArea[touch].h = touchH;

    touchArea[aBtn].x = aX - btnRad;
    touchArea[aBtn].y = aY - btnRad;
    touchArea[aBtn].w = btnRad*2;
    touchArea[aBtn].h = btnRad*2;

    touchArea[xBtn].x = xX - btnRad;
    touchArea[xBtn].y = xY - btnRad;
    touchArea[xBtn].w = btnRad * 2;
    touchArea[xBtn].h = btnRad * 2;

    touchArea[menuBtn].x = menuX - btnRad;
    touchArea[menuBtn].y = menuY - btnRad;
    touchArea[menuBtn].w = btnRad * 2;
    touchArea[menuBtn].h = btnRad * 2;

    s3ePointerRegister(S3E_POINTER_TOUCH_EVENT, TouchEvent, NULL);
    s3ePointerRegister(S3E_POINTER_TOUCH_MOTION_EVENT, MotionEvent, NULL);

    // ----------------------

    // Loop forever, until the user or the OS performs some action to quit the app
    while (!s3eDeviceCheckQuitRequest())
    {
        //Update the input systems
        s3eKeyboardUpdate();
        s3ePointerUpdate();

        Iw2DSurfaceClear(0xff00ff00);

        // ---- ABORT if needed ----

        if (seriousErrorState)
        {
            s3eDebugPrintf(10, 40, 1, "Error setting up sockets... try restarting");
            Iw2DSurfaceShow();
            s3eDeviceYield;
            continue;
        }

        // send about once a second for testing
        /*if (testPacketCount % 60 == 0)
        {
            sprintf(testPacket, "TEST:%d", testPacketCount);
            socket.SendTo(targetAddress, (const char*)testPacket, testPacketSize);
        }
        s3eDebugPrintf(20, 100, 1, "Last sent packet: '%s'", testPacket);
        testPacketCount++;
        */

        // ---- Send and get data over UDP ----

        // Update button/pad states
        int writer = 0;
        for (int i = 0; i < TOUCH_AREA_COUNT; ++i)
        {
            sprintf(packetToSend + writer, "%d", touchArea[i].pressed);
            ++writer;
        }

        sprintf(packetToSend + writer, formatter, touchArea[touch].touchX, touchArea[touch].touchY);
        //sprintf(packetToSend + writer, "%2.5f%2.5f", touchArea[touch].touchX, touchArea[touch].touchY);

        IwAssert(REMOTE, (strlen(packetToSend) < packetSize));

        socket.SendTo(targetAddress, (const char*)packetToSend, packetSize-1);
        
        // -- Receive 

        // process any received packets in internal queue
        ReceiveData(socket, packetSize, MAX_PACKETS);

        
        //Update the scene. The SDK's example framework has a fixed
        //framerate of 20fps, so we pass that duration to the update function.
        //g_SceneRoot->Update(1000/20);

        CIwColour col = CIwColour();
        col.Set(0, 0, 255);
        Iw2DSetColour(col);
        Iw2DFillRect(CIwFVec2(touchX, touchY), CIwFVec2(touchW, touchH));
        Iw2DFillArc(CIwFVec2(aX, aY), CIwFVec2(btnRad, btnRad), 0, 2 * PI);
        Iw2DFillArc(CIwFVec2(xX, xY), CIwFVec2(btnRad, btnRad), 0, 2 * PI);
        Iw2DFillArc(CIwFVec2(menuX, menuY), CIwFVec2(btnRad, btnRad), 0, 2 * PI);


        Iw2DSetFont(font);
        Iw2DSetColour(0xff000000);
        const int LINE_HEIGHT = 20;
        CIwFVec2 region(Iw2DGetSurfaceWidth(), LINE_HEIGHT); // Render the text into this size region

        // Centred at top of the surface
        CIwFVec2 topLeft(0,0);

        // Draw the string into the region
        char labelBuf[200];
        sprintf(labelBuf, "Local host address: %s", localAddrString);
        Iw2DDrawString(labelBuf, topLeft, region, IW_2D_FONT_ALIGN_LEFT, IW_2D_FONT_ALIGN_CENTRE);
        topLeft.y += LINE_HEIGHT;

        char ip[50] = { 0 };
        targetAddress.GetAddressString(ip);
        sprintf(labelBuf, "Sending data to: %s:%d", ip, targetAddress.GetPort());
        Iw2DDrawString(labelBuf, topLeft, region, IW_2D_FONT_ALIGN_LEFT, IW_2D_FONT_ALIGN_CENTRE);
        topLeft.y += LINE_HEIGHT;

        sprintf(labelBuf, "Last sent packet: '%s'", packetToSend);
        Iw2DDrawString(labelBuf, topLeft, region, IW_2D_FONT_ALIGN_LEFT, IW_2D_FONT_ALIGN_TOP);
        topLeft.y += LINE_HEIGHT;

        sprintf(labelBuf, "Last received packet: '%s'", g_LastData);
        Iw2DDrawString(labelBuf, topLeft, region, IW_2D_FONT_ALIGN_LEFT, IW_2D_FONT_ALIGN_CENTRE);
        topLeft.y += LINE_HEIGHT;

        sprintf(labelBuf, "    from: '%s'", g_LastSender);
        Iw2DDrawString(labelBuf, topLeft, region, IW_2D_FONT_ALIGN_LEFT, IW_2D_FONT_ALIGN_TOP);
        topLeft.y += LINE_HEIGHT;


        //g_SceneRoot->Render();

        //Draws Surface to screen
        Iw2DSurfaceShow();

        // Sleep for 0ms to allow the OS to process events etc.
        s3eDeviceYield(0);
    }

    delete packetToSend;

    if (socket.IsOpen())
        socket.Close();

    s3ePointerUnRegister(S3E_POINTER_TOUCH_EVENT, TouchEvent);
    s3ePointerUnRegister(S3E_POINTER_TOUCH_MOTION_EVENT, MotionEvent);

    //Terminate modules being used
    delete font;
    //IwGetResManager()->DestroyGroup("font");
    IwResManagerTerminate(); //clear up font resources
    
    //delete g_SceneRoot;
    Iw2DTerminate();
    
    // Return
    return 0;
}
