#include "s3e.h"
#include "IwDebug.h"
#include "Iw2DSceneGraph.h"
#include "IwGx.h"
#include "s3eZeroConf.h"
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "IwTween.h"
#include "TouchComponents.h"
#include <list>
#include "s3eOSExec.h"

using namespace IwTween;
using namespace Iw2DSceneGraphCore;
using namespace Iw2DSceneGraph;

#include "SocketsUDP.h"
using namespace SocketsUDP;

#include "IwGameController_MarmaladeRemote.h"
using namespace IwGameController;

bool g_multiTouch = false;

CTweenManager* g_tweener;
float g_connectAnim = 0.0f;
CTween* g_connectTween = NULL;
int g_debugDisplay = 0;
CNode* g_connectRoot = NULL;
CIw2DFont* g_font = NULL;
CIw2DFont* g_fontSmall = NULL;

typedef enum eAppState {
    //STATE_INFO = 0,
    STATE_DISCONNECTED = 0,
    STATE_HELP,
    STATE_FINDING_RECEIVERS,
    STATE_SHOWING_RECEIVERS, // found at least one but still searching for more
    STATE_CHANGE_RECEIVER,   // animating changing focussed button
    STATE_CONNECTING,
    STATE_CONNECTED
} eAppState;

eAppState g_appState = STATE_DISCONNECTED;// STATE_INFO;

// -------------------------- Reading packets -----------------------------

#define MAX_PACKETS 5

char g_LastData[SOCKETSUDP_IP_ADDRESS_MAX_LENGTH] = ""; // biggest thing ever is an address
char g_LastSender[SOCKETSUDP_IP_ADDRESS_MAX_LENGTH] = "";

// Address to send data to
SocketAddress g_targetAddress;
char g_targetAddressString[SOCKETSUDP_IP_ADDRESS_MAX_LENGTH] = "";
bool g_gotTarget = false;
bool g_connecting = false;

int64 g_connectStartTime = 0;
const int CONNECT_TIMEOUT = 5 * 1000;

int64 g_fullRestartTime = 0;
const int CONNECT_FULL_RESTART_TIMEOUT = 10 * 1000;

int64 g_keepAliveCheckTime = 0;
const int KEEP_ALIVE_CHECK_TIMEOUT = 500;

int64 g_keepAliveLastCheck = 0;
int g_KeepAliveTimeout = 5 * 1000;

int64 g_networkCheckStartTime = 0;
const int NETWORK_CHECK_TIMEOUT = 2 * 1000;

char g_localAddrString[SOCKETSUDP_IP_ADDRESS_MAX_LENGTH] = "";

bool StartReceiverSearch();

bool g_needsRestart = false;

in_addr g_localAddr; // Host IP address of this device (creates a socket, does a query, closes socket)
SocketUDP g_socket = SocketUDP(); // socket to listen for incomming conections
bool g_fixedTarget = false;

TouchComponents g_touchComponents;

// -------------------------- ZeroConf discovery -----------------------------

static s3eZeroConfSearch*    g_Search = 0;

//Recording info about services we've found
class Record
{
public:
    void*           m_Ref; //handle to internal ref

    int               m_ListPos; // local index in our list, needed for positioning
    CButtonComponent* m_TargetBtn;
    float             m_BtnXAnimStart; 
    float             m_BtnXAnimDest;

    SocketAddress   m_Address; //IP address
    char*           m_AddressString;  //IP address as string
    char*           m_FriendlyName;   //Friendly device name - whatever user has set for their TV/device
    char*           m_AppDescription; //Friendly add description string set by the receiver app
    
    // Records are in double linked list
    Record*         m_Next;
    Record*         m_Prev;

    Record() {}

    ~Record()
    {
        if (m_AddressString)
            free(m_AddressString);
        m_AddressString = NULL;

        if (m_FriendlyName)
            free(m_FriendlyName);
        m_FriendlyName = NULL;

        if (m_AppDescription)
            free(m_AppDescription);
        m_AppDescription = NULL;

        if (m_TargetBtn)
            delete m_TargetBtn;
        m_TargetBtn = NULL;
    }
};

std::list<Record*> g_resultList;
Record* g_currentRecord = NULL;
int g_targetBtnTouchIndex = -1;
float g_targetBtnWidth = 0;

// Calback when a target receiver button is pressed
int32 ConnectToReceiver(void* systemData, void* userData)
{
    Record* record = (Record*)systemData;
    g_connecting = true;
    g_targetAddress = record->m_Address;
    strcpy(g_targetAddressString, record->m_AddressString);
    s3eZeroConfStopSearch(g_Search);
    g_Search = NULL;
    g_appState = STATE_CONNECTING;
    g_currentRecord = NULL;

    g_connectStartTime = s3eTimerGetUST();

    // Delete recors and attached buttons later, once event handler isn't
    // processing buttons....

    return 0;
}

void DestroyTargetRecords()
{
    for (std::list<Record*>::const_iterator it = g_resultList.begin(); it != g_resultList.end(); ++it)
    {
        Record* record = *it;
        delete record; //deletes attached button
    }

    g_resultList.clear();
}

// Callback issued when a new service (matching the query) is discovered
static int32 SearchAddCallBack(s3eZeroConfSearch* search, s3eZeroConfAddRecord* rec, void* userData)
{
    s3eDebugTracePrintf("SearchAddCallBack: %s %s %d", rec->m_Name, rec->m_Hostname, rec->m_Port);
    
    if (strncmp(rec->m_Name, CIwGameControllerMarmaladeRemote::RECEIVER_NAME,
                strlen(CIwGameControllerMarmaladeRemote::RECEIVER_NAME)) == 0 &&
        rec->m_Port == CIwGameControllerMarmaladeRemote::MARMALADE_REMOTE_PORT)
    {
        bool gotValidDevice = true;
        
        SocketAddress targetAddress = SocketAddress(ntohl(rec->m_HostIP), rec->m_Port);
        char targetAddressString[SOCKETSUDP_IP_ADDRESS_MAX_LENGTH] = "";

        if (targetAddress.IsValid())
        {
            if (targetAddress.GetAddressString(targetAddressString))
            {
                IwTrace(REMOTE, ("Found receiver to connect to"));
                g_appState = STATE_SHOWING_RECEIVERS;

                // push a new record onto the list
                Record* record = new Record();

                record->m_Ref = NULL;// rec->m_RecordRef;
                record->m_Prev = NULL;
                
                g_resultList.push_back(record);

                record->m_Address = targetAddress;

                uint32 len = strlen(targetAddressString) + 1;
                record->m_AddressString = (char*)malloc(len);
                strncpy(record->m_AddressString, targetAddressString, len);

                char buf[50];
                len = snprintf(buf, sizeof(buf), "%s", rec->m_NumTxtRecords > 1 ? rec->m_TxtRecords[1] : "<unknown>") + 1;
                record->m_FriendlyName = (char*)malloc(len);
                strncpy(record->m_FriendlyName, buf, len);

                len = snprintf(buf, sizeof(buf), "%s", rec->m_NumTxtRecords > 2 ? rec->m_TxtRecords[2] : "<unknown>") + 1;
                record->m_AppDescription = (char*)malloc(len);
                strncpy(record->m_AppDescription, buf, len);

                IwTrace(REMOTE, ("Address: %s", record->m_AddressString));
                IwTrace(REMOTE, ("Name: %s", record->m_FriendlyName));
                IwTrace(REMOTE, ("Description: %s", record->m_AppDescription));
                
                // -- Add button
                snprintf(buf, sizeof(buf), "%s\n%s", record->m_FriendlyName, record->m_AppDescription);
                record->m_TargetBtn = new CButtonComponent("textures/ButtonTall.png", g_connectRoot,
                    Iw2DGetSurfaceWidth()*0.225, Iw2DGetSurfaceHeight()*0.75, g_targetBtnWidth, 0, //0 -> scale to match width
                    "textures/ButtonTall_down.png", buf, g_fontSmall, ConnectToReceiver, (void*)record, "textures/TVIcon.png", 0.8);
            
                if (!g_currentRecord)
                {
                    record->m_ListPos = 0;
                    g_currentRecord = record;
                    g_touchComponents.components.push_back(record->m_TargetBtn); //only allowing one button to be clicked currently.. just switch which button is pointed to
                    g_targetBtnTouchIndex = g_touchComponents.components.size() - 1;
                }
                else
                {
                    record->m_ListPos = g_resultList.back()->m_ListPos + 1;
                    record->m_TargetBtn->SetX(record->m_TargetBtn->GetX() + (g_resultList.size() - (g_currentRecord->m_ListPos + 1)) * (g_targetBtnWidth * 1.05));
                }
            }
        }
    }
    
    return 0;
}

// this call back is issued when a previously discovered service updates it's txtRecord
static int32 SearchUpdateCallBack(s3eZeroConfSearch* search, s3eZeroConfUpdateRecord* rec, void* userData)
{
    Record  *record = NULL;
    char    buf[50];

    for (std::list<Record*>::const_iterator it = g_resultList.begin(); it != g_resultList.end(); ++it)
    {
        Record* record = *it;
        if (record->m_Ref == rec->m_RecordRef)
            break;
        else
            record = NULL;
    }

    if (!record)
        return 0;

    // Update text record values
    //s3eFree(record->m_FriendlyName);
    int len = snprintf(buf, sizeof(buf), "%s", rec->m_NumTxtRecords > 1 ? rec->m_TxtRecords[1] : "<unknown>") + 1;
    //record->m_FriendlyName = (char*)s3eMalloc(len);
    strncpy(record->m_FriendlyName, buf, len);

    //s3eFree(record->m_AppDescription);
    len = snprintf(buf, sizeof(buf), "%s", rec->m_NumTxtRecords > 2 ? rec->m_TxtRecords[2] : "<unknown>") + 1;
    //record->m_AppDescription = (char*)s3eMalloc(len);
    strncpy(record->m_AppDescription, buf, len);

    // Update button text
    snprintf(buf, sizeof(buf), "%s\n%s", record->m_FriendlyName, record->m_AppDescription);
    record->m_TargetBtn->SetText(buf);
    
    return 0;
}

// this call back is issued when a previously discovered service is unregisted
static int32 SearchRemoveCallBack(s3eZeroConfSearch* search, void* userRef, void* userData)
{
    Record* record;
    bool gotRecord = false;

    for (std::list<Record*>::iterator it = g_resultList.begin(); it != g_resultList.end(); )
    {
        Record* record = *it;
        if (record->m_Ref == userRef)
        {
            gotRecord = true;

            if (g_currentRecord == record)
            {
                std::list<Record*>::const_iterator next = it;
                next++;
                if (next != g_resultList.end())
                {
                    g_currentRecord = *next;
                    g_touchComponents.components[g_targetBtnTouchIndex] = g_currentRecord->m_TargetBtn;
                }
            }

            it = g_resultList.erase(it);

            if (g_resultList.size() == 0)
            {
                g_touchComponents.components.erase(g_touchComponents.components.begin() + g_targetBtnTouchIndex);
                g_targetBtnTouchIndex = -1;
                g_currentRecord = NULL;
            }

            delete record;
        }
        else
        {
            if (gotRecord)
            {
                record->m_ListPos--;
                record->m_TargetBtn->SetX(record->m_TargetBtn->GetX() - g_targetBtnWidth * 1.05);
            }

            ++it;
        }
    }

    return 0;
}

bool StartReceiverSearch()
{   
    // search for "_http._udp" services in domain "local"
    // TODO: Can we just use e.g. "_marmalade"?
    g_Search = s3eZeroConfStartSearch("_http._udp", "local",
                                      SearchAddCallBack, SearchUpdateCallBack, SearchRemoveCallBack, 0);
    return g_Search != NULL;
}


// ------------------------------------------------------------------------

void ClearConnectivity()
{
    if (g_socket.IsOpen())
        g_socket.Close();

    if (s3eZeroConfAvailable() && g_Search)
    {
        s3eZeroConfStopSearch(g_Search);
        g_Search = NULL;
    }

    g_networkCheckStartTime = s3eTimerGetUST();
    
    if (!g_fixedTarget)
    {
        g_targetAddressString[0] = '\0';
        g_gotTarget = false;
        g_connecting = false;
    }
    else
    {
        g_connecting = true;
    }

    g_localAddrString[0] = '\0';
    g_appState = STATE_DISCONNECTED;
}

bool RestartConnectivity()
{
    g_connectStartTime = s3eTimerGetUST();

    DestroyTargetRecords();
    
    if (!g_connectTween)
    {
        g_connectAnim = 0.0f;
        g_connectTween = g_tweener->Tween(3.0f, MIRROR, FLOAT, &g_connectAnim, 1.0f, END);
    }
    else
    {
        g_connectTween->Restart();
    }
    
    ClearConnectivity();

    if (s3eTimerGetUST() - g_fullRestartTime > CONNECT_FULL_RESTART_TIMEOUT)
    {
        g_fullRestartTime = s3eTimerGetUST();
        g_appState = STATE_DISCONNECTED;
        if (g_connectTween)
            g_connectTween->Pause();
        return true; // dont keep restarting
    }
    
    if (g_fixedTarget)
        g_appState = STATE_CONNECTING;
    else
        g_appState = STATE_FINDING_RECEIVERS;

    if (s3eSocketGetInt(S3E_SOCKET_NETWORK_AVAILABLE) == 0)
    {
        IwAssertMsg(REMOTE, false, ("No network found"));
        ClearConnectivity();
        return false;
    }
    
    int networkType = s3eSocketGetInt(S3E_SOCKET_NETWORK_TYPE);
    if (networkType != S3E_NETWORK_TYPE_LAN && networkType != S3E_NETWORK_TYPE_WLAN)
    {
        IwAssertMsg(REMOTE, false, ("Network is not WiFi or LAN (type=%d)", networkType));
        ClearConnectivity();
        return false;
    }
    
    if(!GetPrimaryAddr(g_localAddr))
    {
        IwAssertMsg(REMOTE, false, ("Failed to get local address for this device"));
        ClearConnectivity();
        return false;
    }
    
    char* addrString = inet_ntoa(g_localAddr);
    
    if (!addrString)
    {
        IwAssertMsg(REMOTE, false, ("Failed to convert local address to string"));
        ClearConnectivity();
        return false;
    }
    
    if (!g_socket.Open(CIwGameControllerMarmaladeRemote::MARMALADE_REMOTE_PORT))
    {
        IwAssertMsg(REMOTE, false, ("Failed to open socket to receive data"));
        ClearConnectivity();
        return false;
    }
    
    strcpy(g_localAddrString, addrString);
    
    if (!g_fixedTarget)
    {
        if(s3eZeroConfAvailable() == S3E_TRUE)
        {
            if (!StartReceiverSearch())
            {
                IwAssertMsg(REMOTE, false, ("Failed to start ZeroConf"));
                ClearConnectivity();
                return false;
            }
        }
        else
        {
            IwAssertMsg(REMOTE, false, ("No IP set and ZeroConf not supported"));
            ClearConnectivity();
            return false;
        }
    }
    
    return true;
}


int32 Connect(void* systemData, void* userData)
{
    g_fullRestartTime = s3eTimerGetUST();
    g_needsRestart = !RestartConnectivity();

    return 0;
}


int32 Help(void* systemData, void* userData)
{
    g_appState = STATE_HELP;

    return 0;
}

int32 HelpClose(void* systemData, void* userData)
{
    g_appState = STATE_DISCONNECTED;// STATE_INFO;

    return 0;
}

int32 PrivacyPolicy(void* systemData, void* userData)
{
    s3eOSExecExecute("https://www.madewithmarmalade.com/legal/privacy-policy/app", false);

    return 0;
}


// ----------------------------- Rendering ---------------------------------

void Render(eAppState appState, CNode* scene, CNode* connect, CNode* labels)
{
    if (appState == STATE_CONNECTED)
    {
        scene->Render();
        //Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
        //Iw2DSetColour(0xffffffff);
    }
    else
    {
        //printf("!!!!!! %f\n", g_connectAnim);
        //Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
        connect->Render();
        //Iw2DSetAlphaMode(IW_2D_ALPHA_HALF);
        //Iw2DSetColour(0x33ffffff);
    }

    //Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
    //Iw2DSetColour(0xffffffff);

    if (g_debugDisplay)
        labels->Render();

    Iw2DSurfaceShow();
    s3eDeviceYield(0);
}

// ---------------------------------------------------------------------------

void SetUpTextBox(CComponent** background_out, CLabel** label_out, CNode* parent, int width, int y, const char* text, CIw2DFont* font = NULL)
{
    int x = (Iw2DGetSurfaceWidth() - width) / 2.0;
    CComponent* background = new CComponent("textures/TextHolder.png", parent, x, y, width, 0);

    int padText = background->GetH() * 0.1; // note no padding at top just since it looks more central
    CLabel* label = SetUpLabel(background->GetSprite(), padText*3, padText*0.5, background->GetW() - padText*6, background->GetH() - padText,
        font ? font : g_font, text, CColor(0xff, 0xff, 0xff, 0xff), IW_2D_FONT_ALIGN_CENTRE);

    *background_out = background;
    *label_out = label;
}

void TargetButtonAnimOnComplete(CTween* pTween)
{
    g_appState = STATE_SHOWING_RECEIVERS;
    g_currentRecord->m_TargetBtn->Enable();

    for (std::list<Record*>::const_iterator it = g_resultList.begin(); it != g_resultList.end(); ++it)
    {
        Record* record = *it;
        record->m_TargetBtn->SetX(record->m_BtnXAnimDest);
    }
}

int main()
{
    //Initialise graphics system(s)
    Iw2DInit();

    // Create root node
    CNode* labelsRoot = new CNode();
    CNode* sceneRoot = new CNode();
    g_connectRoot = new CNode();
    
    g_tweener = new CTweenManager;
    float buttonMovePos = 0.0f;
    CTween* buttonMoveTween = NULL;
    
    sceneRoot->m_X = 0;
    sceneRoot->m_Y = 0;
    labelsRoot->m_X = 0;
    labelsRoot->m_Y = 0;
    g_connectRoot->m_X = 0;
    g_connectRoot->m_Y = 0;

    g_targetBtnWidth = Iw2DGetSurfaceWidth()*0.55;

    int64 lastFrameTime = s3eTimerGetUST();
    int frameCount = 0;
    const int FRAMES_TO_AVERAGE = 20;
    float deltaTime = 0.016; //60fps
    int64 lastFrameSetTime = 0; //time last FRAMES_TO_AVERAGE group of frames started

    // Load font
    IwResManagerInit();
    IwGetResManager()->LoadGroup("font.group");

    // Iw2D doesnt provide good font scaling so just picking a set
    if (Iw2DGetSurfaceWidth() < 1000)
    {
        g_font = Iw2DCreateFontResource("arial28");
        g_fontSmall = Iw2DCreateFontResource("arial18");
    }
    else
    {
        g_font = Iw2DCreateFontResource("arial48");
        g_fontSmall = Iw2DCreateFontResource("arial28");
    }

    // ---- UI components for connected screens ----

    CComponent* cBackground = new CComponent("textures/Background.png", g_connectRoot, 0, 0, Iw2DGetSurfaceWidth(), Iw2DGetSurfaceHeight(), true);
    CComponent* cLogo = new CComponent("textures/Logo.png", g_connectRoot, 0, 0, 50, 50, false);
    cLogo->SetW(cBackground->GetW() / cBackground->GetImageW() * cLogo->GetImageW() * 0.453);
    cLogo->SetH(cBackground->GetH() / cBackground->GetImageH() * cLogo->GetImageH() * 0.453);
    cLogo->SetX((Iw2DGetSurfaceWidth() - cLogo->GetW()) / 2.0);
    cLogo->SetY((Iw2DGetSurfaceHeight() - cLogo->GetH()) / 2.0);


    // Remote style, bottom-right-aligned layout
    /*
    int touchX, touchW, touchH;
    touchX = touchW = touchH = Iw2DGetSurfaceWidth() / 2;
    int midY = Iw2DGetSurfaceHeight() / 2;
    int touchY = midY - touchH;
    int aX = touchX * 1.25;
    int aY = midY * 1.3;
    int xX = touchX * 1.75;
    int xY = aY;
    int menuX = xX;
    int menuY = midY * 1.6;
    int btnRad = touchX * 0.18;
    */

    // Fill-the-screen layout
    int screenW = Iw2DGetSurfaceWidth();
    float padding = (float)screenW * 0.01;
    float radius = (Iw2DGetSurfaceWidth() - padding*4.0) / 6.0;
    int touchX, touchY, touchW, touchH;
    touchW = touchH = (int)((float)Iw2DGetSurfaceWidth() - padding*2.0);
    touchX = (Iw2DGetSurfaceWidth()-touchW) / 2;
    //touchY = (Iw2DGetSurfaceHeight()-btnRad*2 - touchH) / 2; //centre in area bove buttons
    touchY = touchX;
    int aY, xY, menuY;  // centre points
    aY = xY = menuY = (int)((float)Iw2DGetSurfaceHeight() - radius -padding);
    int xX = radius + padding;
    int aX = radius*3.0 + padding*2.0;
    int menuX = radius*5.0 + padding*3.0;
    int btnRad = (int)radius;

    CTouchComponent* cTouchPad = new CTouchComponent("textures/TouchPad.png", sceneRoot, touchX, touchY, touchW, touchH, true, NULL, true, "textures/TouchSpot.png");
    CTouchComponent* cBtnA = new CTouchComponent("textures/BtnA.png", sceneRoot, aX - btnRad, aY - btnRad, btnRad * 2, btnRad * 2);
    CTouchComponent* cBtnX = new CTouchComponent("textures/BtnX.png", sceneRoot, xX - btnRad, xY - btnRad, btnRad * 2, btnRad * 2);
    CTouchComponent* cBtnMenu = new CTouchComponent("textures/BtnMenu.png", sceneRoot, menuX - btnRad, menuY - btnRad, btnRad * 2, btnRad * 2);

    // ---- UI components for connecting screens ----

    int statusSize = Iw2DGetSurfaceWidth()*0.65;
    int buttonSize = Iw2DGetSurfaceWidth()*0.6;
    int smallButtonSize = Iw2DGetSurfaceWidth()*0.2;
    int infoSize = Iw2DGetSurfaceWidth();
    int conSize = Iw2DGetSurfaceWidth()*0.3;

    CComponent* cInfoTop = NULL; CLabel* cInfoTopLabel = NULL;
    SetUpTextBox(&cInfoTop, &cInfoTopLabel, g_connectRoot, infoSize, 0, //set Y pos later from x...
        "Use this app as a controller for Marmalade powered games");
    cInfoTop->SetY(cInfoTop->GetX());

    CComponent* cStatus = NULL; CLabel* cStatusLabel = NULL;
    SetUpTextBox(&cStatus, &cStatusLabel, g_connectRoot, statusSize, Iw2DGetSurfaceHeight()*0.17,
        "Not connected");
    cStatus->Hide();

    CComponent* cConnectIcon = new CComponent("textures/ConnectIcon.png", g_connectRoot,
        (Iw2DGetSurfaceWidth() - conSize) / 2.0, (cStatus->GetY() + cStatus->GetH()/7.0 - conSize) / 2.0, conSize, conSize);
    cConnectIcon->Hide();

    CButtonComponent* cConnectButton = new CButtonComponent("textures/Button.png", g_connectRoot,
        0, Iw2DGetSurfaceHeight()*0.68, buttonSize, 0, //0 -> scale to match width
        "textures/Button_down.png", "CONNECT", g_font, Connect, NULL);
    cConnectButton->SetX((Iw2DGetSurfaceWidth()- cConnectButton->GetW())/2);
    cConnectButton->Hide();

    CComponent* cInfoBottom = NULL; CLabel* cInfoBottomLabel = NULL;
    SetUpTextBox(&cInfoBottom, &cInfoBottomLabel, g_connectRoot, infoSize, 0,
        "Please connect to the same WiFi network as your Apple TV");

    CButtonComponent* cHelpButton = new CButtonComponent("textures/Help.png", g_connectRoot,
        0, 0, smallButtonSize, 0, //0 -> scale to match width
        "textures/Help_down.png", "HELP", g_font, Help, NULL);
    cHelpButton->SetX((Iw2DGetSurfaceWidth() - cHelpButton->GetW()) / 2);
    //cHelpButton->SetY(Iw2DGetSurfaceHeight() - cHelpButton->GetH() * 1.2);
    cHelpButton->SetY(Iw2DGetSurfaceHeight()*0.22);

    //cInfoBottom->SetY(cHelpButton->GetY() - cInfoBottom->GetH() - cInfoBottom->GetX());
    cInfoBottom->SetY(Iw2DGetSurfaceHeight() - cInfoBottom->GetH() - cInfoBottom->GetX());

    //CComponent* cHelpBox = NULL; CLabel* cHelpBoxLabel = NULL;
    //SetUpTextBox(&cHelpBox, &cHelpBoxLabel, g_connectRoot, infoSize, 0,
    //cHelpBox->SetY((Iw2DGetSurfaceHeight() - cHelpBox->GetH())/2.0 - cHelpButton->GetH());
    //cHelpBox->Hide();
    
    float padText = Iw2DGetSurfaceWidth() * 0.03;

    CButtonComponent* cHelpCloseButton = new CButtonComponent("textures/Help.png", g_connectRoot,
        padText, 0, smallButtonSize, 0, //0 -> scale to match width
        "textures/Help_down.png", "BACK", g_font, HelpClose, NULL);

    cHelpCloseButton->SetY(Iw2DGetSurfaceHeight() - cHelpCloseButton->GetH() * 1.2);
    cHelpCloseButton->Hide();

    float helpHeight = Iw2DGetSurfaceHeight() - cHelpCloseButton->GetY() - padText*2;
    CLabel* cHelpBox = SetUpLabel(g_connectRoot, padText, 0, Iw2DGetSurfaceWidth() - padText, helpHeight,
        g_fontSmall, 
        "WHAT IS MARMALADE APPLE TV MULTIPLAYER GAME CONTROLLER?\n\n"
        "Marmalade Apple TV Multiplayer Game Controller is the new companion app that enables any iPhone, iPad or iPod touch to act as a second game controller in any compatible app for Apple TV!\n\n"
        "WHAT DEVICES CAN BE USED TO RUN THE APP?\n\n"
        "Any iPhone, iPad or iPod touch with iOS 6.0 or later!\n\n"
        "WHAT ARE THE FEATURES OF THE APP?\n\n"
        "No need to buy multiple controllers to play compatible multiplayer games. All you need is your iPhone, iPad or iPod touch and this app installed! "
        "Just launch your compatible Apple TV game; start the Marmalade Apple TV Multiplayer Game Controller app; connect to your Apple TV and you're done!\n\n"
        "QUICK TIPS FOR USING THE APP\n\n"
        "Ensure you're running the Apple TV game before connecting with the Marmalade Apple TV Multiplayer Game Controller app. "
        "Ensure your device (with the app installed) is on the same WiFi network as the Apple TV. "
        "Note: Only one device with the Marmalade Apple TV Multiplayer Game Controller app installed can be used per session of game play. First device to connect will be the controller.",
        //CColor(0x05, 0x55, 0x70, 0xff), IW_2D_FONT_ALIGN_LEFT);
        CColor(0xAA, 0xDD, 0xFF, 0xff), IW_2D_FONT_ALIGN_LEFT);
    cHelpBox->m_Y = (cHelpCloseButton->GetY() - cHelpBox->m_H)/2.0;
    cHelpBox->m_IsVisible = false;

    CButtonComponent* cPrivacyButton = new CButtonComponent("textures/Button.png", g_connectRoot,
        0, 0, buttonSize, 0, //0 -> scale to match width
        "textures/Button_down.png", "PRIVACY POLICY", g_font, PrivacyPolicy, NULL);
    cPrivacyButton->SetX(Iw2DGetSurfaceWidth() - cPrivacyButton->GetW() - padText);
    cPrivacyButton->SetY(cHelpCloseButton->GetY() - ((cPrivacyButton->GetH() - cHelpCloseButton->GetH())/2.0));
    //float privScale = cHelpCloseButton->GetH() / cHelpCloseButton->GetImageH();
    //cPrivacyButton->SetW(privScale * cPrivacyButton->GetImageW());
    //cPrivacyButton->SetH(privScale * cPrivacyButton->GetImageH());
    cPrivacyButton->Hide();


    // ---- Touch input ----

    g_touchComponents.components.push_back(cTouchPad);
    g_touchComponents.components.push_back(cBtnA);
    g_touchComponents.components.push_back(cBtnX);
    g_touchComponents.components.push_back(cBtnMenu);
    g_touchComponents.components.push_back(cConnectButton);
    g_touchComponents.components.push_back(cHelpButton);
    g_touchComponents.components.push_back(cHelpCloseButton);
    g_touchComponents.components.push_back(cPrivacyButton);
    g_touchComponents.useMultiTouch = s3ePointerGetInt(S3E_POINTER_MULTI_TOUCH_AVAILABLE) == 1;

    if (g_touchComponents.useMultiTouch)
    {
        s3ePointerRegister(S3E_POINTER_TOUCH_EVENT, TouchEvent, (void*)&g_touchComponents);
        s3ePointerRegister(S3E_POINTER_TOUCH_MOTION_EVENT, MotionEvent, (void*)&g_touchComponents);
    }
    else
    {
        s3ePointerRegister(S3E_POINTER_BUTTON_EVENT, TouchEvent, (void*)&g_touchComponents);
        s3ePointerRegister(S3E_POINTER_MOTION_EVENT, MotionEvent, (void*)&g_touchComponents);
    }


    // ---- Debugging labels ----

    int lblPos = touchY*2 + touchH;
    const int LINE_HEIGHT = 30;

    CLabel* lblLocalAddr = SetUpLabel(labelsRoot, 0, lblPos, 0, LINE_HEIGHT, g_fontSmall, "", CColor(0x00, 0x30, 0x30, 0xff), IW_2D_FONT_ALIGN_LEFT);
    lblPos += LINE_HEIGHT;

    CLabel* lblTarget = SetUpLabel(labelsRoot, 0, lblPos, 0, LINE_HEIGHT, g_fontSmall, "", CColor(0x00, 0x30, 0x30, 0xff), IW_2D_FONT_ALIGN_LEFT);
    lblPos += LINE_HEIGHT;

    CLabel* lblLastPacket = SetUpLabel(labelsRoot, 0, lblPos, 0, LINE_HEIGHT, g_fontSmall, "", CColor(0x00, 0x30, 0x30, 0xff), IW_2D_FONT_ALIGN_LEFT);
    lblPos += LINE_HEIGHT;

    CLabel* lblLastData = SetUpLabel(labelsRoot, 0, lblPos, 0, LINE_HEIGHT, g_fontSmall, "", CColor(0x00, 0x30, 0x30, 0xff), IW_2D_FONT_ALIGN_LEFT);
    lblPos += LINE_HEIGHT;

    CLabel* lblLastSender = SetUpLabel(labelsRoot, 0, lblPos, 0, LINE_HEIGHT, g_fontSmall, "", CColor(0x00, 0x30, 0x30, 0xff), IW_2D_FONT_ALIGN_LEFT);
    lblPos += LINE_HEIGHT;


    // ---------- connection setup -----------
    
    // For UDP we just push to an IP address.
    // This needs to be done through zeroconf in final version, for now just hard code
    char targetIP[50]; //IP sting max length = 45
    
    int debugUI = 0;
    s3eConfigGetInt("ControllerRemote", "DebugDisplay", &g_debugDisplay);
    s3eConfigGetInt("ControllerRemote", "NoConnectDebugUI", &debugUI);

    if (s3eConfigGetString("ControllerRemote", "TargetIPAddress", targetIP) == S3E_RESULT_SUCCESS)
    {
        g_fixedTarget = true;
        g_gotTarget = true;
        g_targetAddress = SocketAddress(targetIP, CIwGameControllerMarmaladeRemote::MARMALADE_REMOTE_PORT);
    }
    
    g_needsRestart = false;

    bool ignoreAllTimeouts = false;
    bool dontSendData = false;
    
    // ----------------------

    // For now, just packing controller state info as simple text string. In reality will want to compress...

    unsigned int packetSize = CIwGameControllerMarmaladeRemote::MARMALADE_REMOTE_PACKET_SIZE;
    
    char* packetToSend = new char[packetSize];
    packetToSend[0] = '\0';

    int precision = CIwGameControllerMarmaladeRemote::AXIS_VALUE_LENGTH - 2; // length=whole number: x.<precision>
    char formatter[11];
    sprintf(formatter, "%%1.%df%%1.%df", precision, precision);

    s3eConfigGetInt("ControllerRemote", "KeepAliveTimeout", &g_KeepAliveTimeout);

    int menuIsQuit = 0;
    bool lastMenuState = false;
    s3eConfigGetInt("ControllerRemote", "MenuIsQuit", &menuIsQuit);

    // ----------------------

    // Loop forever, until the user or the OS performs some action to quit the app
    while (!s3eDeviceCheckQuitRequest())
    {
        //Update the input systems
        s3eKeyboardUpdate();
        s3ePointerUpdate();

        //Update anims etc
        frameCount++;
        if (frameCount == FRAMES_TO_AVERAGE)
        {
            int elapsed = s3eTimerGetUST() - lastFrameSetTime;
            lastFrameSetTime = s3eTimerGetUST();
            deltaTime = float(elapsed / FRAMES_TO_AVERAGE) / 1000.0f;
            frameCount = 0;
            if (deltaTime > 0.03)
                deltaTime == 0.03;
        }
        sceneRoot->Update(deltaTime);
        labelsRoot->Update(deltaTime);
        g_connectRoot->Update(deltaTime);
        g_tweener->Update(deltaTime);

        // Clear
        Iw2DSurfaceClear(0xffffffff);
        //Iw2DSetFont(g_font);

        // Debug text rendering
        char labelBuf[200];

        lblLocalAddr->m_Text = "";
        lblTarget->m_Text = "";
        lblLastPacket->m_Text = "";
        lblLastData->m_Text = "";
        lblLastSender->m_Text = "";

        cConnectIcon->SetAlpha(g_connectAnim);

        if (debugUI)
        {
            Render(STATE_CONNECTED, sceneRoot, g_connectRoot, labelsRoot);
            continue;
        }

        // ---- Restart trying to find any connection at all regularly while seraching
        // Restart on any error - picks up Wifi network changes etc.
        if (g_needsRestart)
        {
            strcpy(labelBuf, "Can't find network... will retry");
            lblLocalAddr->m_Text = labelBuf;
            Render(g_appState, sceneRoot, g_connectRoot, labelsRoot);

            if (s3eTimerGetUST() - g_networkCheckStartTime > NETWORK_CHECK_TIMEOUT)
                g_needsRestart = !RestartConnectivity();

            continue;
        }
        else if ((g_appState == STATE_FINDING_RECEIVERS || g_appState == STATE_CONNECTING) && s3eTimerGetUST() - g_connectStartTime > CONNECT_TIMEOUT)
        {
            g_needsRestart = !RestartConnectivity();

            Render(g_appState, sceneRoot, g_connectRoot, labelsRoot);
            continue;
        }

        if ((int)g_appState < STATE_CONNECTING)
        {
            if (g_appState == STATE_DISCONNECTED) //STATE_INFO)
            {
                cInfoTop->Show();
                cStatus->Hide();
                //cConnectButton->Hide();
                cConnectButton->Show();
                cInfoBottom->Show();
                cHelpButton->Show();
                cHelpCloseButton->Hide();
                cPrivacyButton->Hide();
                cLogo->Show();
                cConnectIcon->Hide();
                cHelpBox->m_IsVisible = false;

                //cInfoBottom->SetY(cHelpButton->GetY() - cInfoBottom->GetH() - cInfoBottom->GetX());

                /*if (!cHelpButton->m_touchArea.pressed && s3ePointerGetState(S3E_POINTER_BUTTON_SELECT) == S3E_POINTER_STATE_DOWN)
                {
                    cInfoTop->Hide();
                    cHelpButton->Hide();
                    cStatus->Show();
                    cConnectButton->Show();
                    cInfoBottom->SetY(Iw2DGetSurfaceHeight() - cInfoBottom->GetH() - cInfoBottom->GetX());
                    g_appState = STATE_DISCONNECTED;
                }*/
            }
            else if (g_appState == STATE_HELP)
            {
                cInfoTop->Hide();
                cStatus->Hide();
                cInfoBottom->Hide();
                cHelpButton->Hide();
                cLogo->Hide();
                cConnectButton->Hide();
                //cHelpBox->Show();
                cHelpBox->m_IsVisible = true;
                cHelpCloseButton->Show();
                cPrivacyButton->Show();
            }

            //STATE_DISCONNECTED -> waiting for connect (TODO: or FAQ?) button press

            if (g_appState == STATE_FINDING_RECEIVERS)
            {
                cInfoTop->Hide();
                cConnectIcon->Show();
                cConnectButton->Hide();
                cStatus->Show();
                cHelpButton->Hide();
                cStatusLabel->m_Text = "Searching...";

                //SearchAddCallBack(NULL, NULL, NULL);
            }

            if (g_appState == STATE_SHOWING_RECEIVERS)
            {
                cConnectIcon->Show();   // when debugging this might not turn up without this if no update loop happens before receiver is found!
                cConnectButton->Hide(); // ditto
                cInfoBottom->Hide();
                cStatusLabel->m_Text = "Choose an Apple TV";
                // wait for TV button to be pressed

                if (g_currentRecord && s3ePointerGetState(S3E_POINTER_BUTTON_SELECT) == S3E_POINTER_STATE_DOWN)
                {
                    int animate = 0;
                    float targetMove = 0.0;
                    if (g_resultList.back() != g_currentRecord && s3ePointerGetX() > g_currentRecord->m_TargetBtn->GetX() + g_currentRecord->m_TargetBtn->GetW())
                    {
                        animate = -1;
                    }
                    else if (g_resultList.front() != g_currentRecord && s3ePointerGetX() < g_currentRecord->m_TargetBtn->GetX())
                    {
                        animate = 1;
                    }

                    if (animate != 0)
                    {
                        buttonMovePos = 0.0f;
                        bool updatedCurrent = false;

                        float targetDelta = g_targetBtnWidth * 1.05 * (float)animate;

                        buttonMoveTween = g_tweener->Tween(0.3f, FLOAT, &buttonMovePos, targetDelta, ONCOMPLETE, TargetButtonAnimOnComplete, END);

                        g_appState = STATE_CHANGE_RECEIVER;

                        for (std::list<Record*>::const_iterator it = g_resultList.begin(); it != g_resultList.end(); ++it)
                        {
                            Record* record = *it;
                            record->m_BtnXAnimStart = record->m_TargetBtn->GetX();
                            record->m_BtnXAnimDest = record->m_BtnXAnimStart + targetDelta;

                            if (!updatedCurrent && g_currentRecord == record)
                            {
                                updatedCurrent = true;
                                std::list<Record*>::const_iterator next = it;
                                if (animate == -1)
                                    ++next;
                                else
                                    --next;

                                g_currentRecord = *next;
                                g_touchComponents.components[g_targetBtnTouchIndex] = g_currentRecord->m_TargetBtn;
                                g_currentRecord->m_TargetBtn->Disable();
                            }
                        }
                    }
                }
            }

            if (g_appState == STATE_CHANGE_RECEIVER)
            {
                for (std::list<Record*>::const_iterator it = g_resultList.begin(); it != g_resultList.end(); ++it)
                {
                    Record* record = *it;
                    record->m_TargetBtn->SetX(record->m_BtnXAnimStart + buttonMovePos);
                }
            }

            Render(g_appState, sceneRoot, g_connectRoot, labelsRoot);
            continue;
        }

        if (g_appState == STATE_CONNECTING)
        {
            cConnectIcon->Show();
            cConnectButton->Hide();
            cStatusLabel->m_Text = "Connecting...";

            // Destroy all receiver buttons. Cant destroy in touch event
            // (looping thorugh buttons) so do here
            if (g_targetBtnTouchIndex != -1)
            {
                g_touchComponents.components.erase(g_touchComponents.components.begin() + g_targetBtnTouchIndex);
                g_targetBtnTouchIndex = -1;

                DestroyTargetRecords(); //records destory their buttons too
            }
        }

        // ---- Process any responses from receiver ----
        
        if (g_socket.IsOpen())
        {
            int packetsRead = 0;

            while (MAX_PACKETS == 0 || packetsRead <= MAX_PACKETS) // better to have timeout than maxPackets value?
            {
                sockaddr_in from;
                socklen_t fromLength = sizeof(from);
                
                SocketAddress sender = SocketAddress();
                int bytes = g_socket.ReceiveFrom(sender, (char*)g_LastData, packetSize+1);
                
                if (bytes <= 0)
                    break;
                
                packetsRead++;
                
                sender.GetAddressString(g_LastSender); //todo: remove this in non debug
                
                // Always only respond if sender was who we sent connect token to earlier
                if (!g_targetAddress.IsValid() || g_targetAddress != sender)
                    continue;
                
                // Waiting to connect
                if (g_connecting)
                {
                    //Check if packet is our own address being sent back
                    if(strncmp(g_LastData, g_localAddrString, strlen(g_localAddrString)) == 0)
                    {
                        // ---- CONNECTION SUCCESS ----
                        
                        g_connecting = false;
                        g_gotTarget = true;
                        g_appState = STATE_CONNECTED;
                        g_keepAliveCheckTime = s3eTimerGetUST();
                        g_keepAliveLastCheck = g_keepAliveCheckTime;
                        
                        if (g_connectTween)
                            g_connectTween->Pause();
                    }
                }
                
                // Checking for disconnect/stay alive
                else if (g_gotTarget)
                {
                    IwTrace(REMOTE, ("got data while connected: %s", g_LastData));
                    
                    if(strncmp(g_LastData, CIwGameControllerMarmaladeRemote::DISCONNECT_TOKEN, strlen(CIwGameControllerMarmaladeRemote::DISCONNECT_TOKEN)) == 0)
                    {
                        IwTrace(REMOTE, ("got disconnect notification"));
                        // Got disconnected notification. Restart everything
                        g_fullRestartTime = 0; //force complete restart
                        g_needsRestart = !RestartConnectivity();
                    }
                    
                    else if(strncmp(g_LastData, CIwGameControllerMarmaladeRemote::KEEPCONNECT_TOKEN, strlen(CIwGameControllerMarmaladeRemote::KEEPCONNECT_TOKEN)) == 0)
                    {
                        IwTrace(REMOTE, ("got keep connected notification"));
                        g_keepAliveLastCheck = s3eTimerGetUST();
                        ignoreAllTimeouts = false; //receiver only sends KEEPCONNECT_TOKEN if its not ignoring timeouts
                        dontSendData = false;
                    }

                    else if (!ignoreAllTimeouts && strncmp(g_LastData, CIwGameControllerMarmaladeRemote::IGNORETIMEOUT_TOKEN,
                        strlen(CIwGameControllerMarmaladeRemote::IGNORETIMEOUT_TOKEN)) == 0)
                    {
                        IwTrace(REMOTE, ("got ignore timeouts notification"));
                        //g_keepAliveCheckTime = s3eTimerGetUST();
                        ignoreAllTimeouts = true;
                        dontSendData = false;
                    }

                    else if (!ignoreAllTimeouts && strncmp(g_LastData, CIwGameControllerMarmaladeRemote::NOSEND_TOKEN,
                        strlen(CIwGameControllerMarmaladeRemote::NOSEND_TOKEN)) == 0)
                    {
                        IwTrace(REMOTE, ("got ignore timeouts and dont send data notification"));
                        //g_keepAliveCheckTime = s3eTimerGetUST();
                        ignoreAllTimeouts = true;
                        dontSendData = true;
                    }
                }
            }
        }
        else
        {
            IwAssertMsg(REMOTE, false, ("Waiting for data, but no open socket!"));
        }
        
        if (!g_gotTarget)
        {
            // ---- Connecting ----
            
            if (g_connecting)
            {
                g_socket.SendTo(g_targetAddress, (const char*)CIwGameControllerMarmaladeRemote::CONNECT_TOKEN,
                              strlen(CIwGameControllerMarmaladeRemote::CONNECT_TOKEN));
                
                sprintf(labelBuf, "Connecting to target: %s:%d", g_targetAddressString,
                        g_targetAddress.GetPort());
                lblLocalAddr->m_Text = labelBuf;
            }
            else
            {
                lblLocalAddr->m_Text = "Looking for app to connect to...";
            }
        }
        else
        {
            // ---- Connected - Now send data over UDP ----

            Iw2DSetColour(0xffffffff);
            Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
            
            // Update button/pad states
            int writer = 0;
            for (int i = 0; i < RemoteButtonIDs::BUTTON_COUNT; ++i)
            {
                TouchState* touchArea = &g_touchComponents.components[i]->m_touchArea;

                if (menuIsQuit && (RemoteButtonIDs::eRemoteButtonIDs)i == RemoteButtonIDs::START)
                {
                    if (!touchArea->pressed && lastMenuState)
                    {
                        g_socket.SendTo(g_targetAddress, CIwGameControllerMarmaladeRemote::DISCONNECT_TOKEN,
                            strlen(CIwGameControllerMarmaladeRemote::DISCONNECT_TOKEN) + 1);

                        g_fullRestartTime = 0;
                        g_needsRestart = !RestartConnectivity();
                    }

                    lastMenuState = touchArea->pressed;

                    packetToSend[writer] = '0';
                    ++writer;
                    continue;
                }

                if(touchArea->pressed)
                    packetToSend[writer] = '1';
                else
                    packetToSend[writer] = '0';
                ++writer;
            }
            
            // send
            if (!dontSendData)
            {
                sprintf(packetToSend + writer, formatter, cTouchPad->m_touchArea.touchX, cTouchPad->m_touchArea.touchY);
                packetToSend[packetSize] = '\0'; //formatter does this anyway but keeping in case way that gets written changes
                IwTrace(REMOTE, ("SEND PACKET: %s", packetToSend));
                g_socket.SendTo(g_targetAddress, (const char*)packetToSend, packetSize - 1); //-1 as we dont want the null terminator
            }
            
            int64 timeStamp = s3eTimerGetUST();
            
            if (!ignoreAllTimeouts && timeStamp - g_keepAliveLastCheck > g_KeepAliveTimeout)
            {
                // Timeout - Disconnect and restart everything
                g_fullRestartTime = s3eTimerGetUST(); //give some tome to reconnect without user input
                g_needsRestart = !RestartConnectivity();
                IwTrace(REMOTE,("Disconnecting sender on timeout"));
            }
            else if (timeStamp - g_keepAliveCheckTime > KEEP_ALIVE_CHECK_TIMEOUT)
            {
                // check periodically if the receiver has disconnected us

                g_keepAliveCheckTime = s3eTimerGetUST();
                g_socket.SendTo(g_targetAddress, CIwGameControllerMarmaladeRemote::KEEPCONNECT_TOKEN,
                              strlen(CIwGameControllerMarmaladeRemote::KEEPCONNECT_TOKEN));
                IwTrace(REMOTE,("sending keep alive token"));
            }
            
            
            // Text display

            sprintf(labelBuf, "Local host address: %s", g_localAddrString);
            lblLocalAddr->m_Text = labelBuf;

            char ip[50] = { 0 };
            g_targetAddress.GetAddressString(ip);
            sprintf(labelBuf, "Sending data to: %s:%d", ip, g_targetAddress.GetPort());
            lblTarget->m_Text = labelBuf;

            sprintf(labelBuf, "Last sent packet: '%s'", packetToSend);
            lblLastPacket->m_Text = labelBuf;

            sprintf(labelBuf, "Last received packet: '%s'", g_LastData);
            lblLastData->m_Text = labelBuf;

            sprintf(labelBuf, "    from: '%s'", g_LastSender);
            lblLastSender->m_Text = labelBuf;
        }
        
        // ------- Rendering --------

        Render(g_appState, sceneRoot, g_connectRoot, labelsRoot);

    }
    
    if (g_Search)
    {
        s3eZeroConfStopSearch(g_Search);
        g_Search = NULL;
    }
    
    delete packetToSend;

    if (g_socket.IsOpen())
        g_socket.Close();

    DestroyTargetRecords();

    s3ePointerUnRegister(S3E_POINTER_TOUCH_EVENT, TouchEvent);
    s3ePointerUnRegister(S3E_POINTER_TOUCH_MOTION_EVENT, MotionEvent);

    g_touchComponents.components.clear();

    //Graphics clean up
    
/*
    delete cTouchPad;
    delete cBtnA;
    delete cBtnX;
    delete cBtnMenu;
    delete cConnectIcon;
    delete cBackground;
    delete cLogo;
    delete cConnectButton;
    delete cHelpButton;
    delete cHelpCloseButton;
    delete cPrivacyButton;

    labelsRoot->RemoveChild(lblLocalAddr);
    labelsRoot->RemoveChild(lblTarget);
    labelsRoot->RemoveChild(lblLastPacket);
    labelsRoot->RemoveChild(lblLastData);
    labelsRoot->RemoveChild(lblLastSender);
    
    delete lblLocalAddr;
    delete lblTarget;
    delete lblLastPacket;
    delete lblLastData;
    delete lblLastSender;
*/
    // Root nodes delete all their children
    delete sceneRoot;
    delete labelsRoot;
    delete g_connectRoot;
    delete g_font;
    delete g_fontSmall;

    IwResManagerTerminate(); //clears up font resources
    
    //delete g_SceneRoot;
    Iw2DTerminate();
    
    // Return
    return 0;
}
