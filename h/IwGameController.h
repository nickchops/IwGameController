/*
 * (C) 2015 Nick Smith. All Rights Reserved.
 */
#ifndef IW_GAMECONTROLLER_H
#define IW_GAMECONTROLLER_H

#include <string>
#include "s3e.h"

/**
 IwGameController is a singleton class. All public functionality should be
 accessed via static namespace functions (IwGameController::Init(),
 IwGameController(), etc.)
 Each controller extension is exposed via a class that inherits from the
 IwGameController virtual class. The multiple classes are hidden from the
 user by the static interface. This avoids run time checks for which
 extension to call functions for while providing a very simple interface.
 You need to call Terminate() to clean-up resources.
*/

namespace IwGameController
{
    // TODO: might move these all into the class itself
    struct Type
    {
        enum eType
        {
            ANY = 0,                 // Input only - check for any type available. Use default type for the device.
            ANDROID_ANY,             //Input only - use best android controller type available.
            ANDROID_GENERIC,
            ANDROID_OUYA_EVERYWHERE,
            ANDROID_AMAZON,
            IOS,
            DESKTOP_HID,
            NONE                     // Output only - indicates no controller type has been initialised. TODO: prob remove this now droppign simgleton style
        };
    };
    
    struct Axis
    {
        enum eAxis
        {
            DPAD_X = 0,
            DPAD_Y,
            STICK_LEFT_X
            STICK_LEFT_Y,
            STICK_RIGHT_X,
            STICK_RIGHT_Y,
            TRIGGER_LEFT,
            TRIGGER_RIGHT,
            MAX
        };
    };

    struct Button
    {
        enum eButton
        {
            A = 0,
            B,
            X,
            Y,
            DPAD_CENTER,
            DPAD_UP,
            DPAD_DOWN,
            DPAD_LEFT,
            DPAD_RIGHT,
            SHOULDER_LEFT,
            SHOULDER_RIGHT,
            STICK_LEFT,
            STICK_RIGHT,
            TRIGGER_LEFT,
            TRIGGER_RIGHT,
            SELECT,
            START,
            LEFT_STICK_UP,
            LEFT_STICK_DOWN,
            LEFT_STICK_LEFT,
            LEFT_STICK_RIGHT,
            RIGHT_STICK_UP,
            RIGHT_STICK_DOWN,
            RIGHT_STICK_LEFT,
            RIGHT_STICK_RIGHT,
            MAX
        };
    };
    
    // Opaque controller reference
    typedef struct CIwGameControllerHandle CIwGameControllerHandle;

    typedef void (*IwGameControllerConnectCallback)(void* userdata, CIwGameControllerHandle* data);
    typedef void (*IwGameControllerDisconnectCallback)(void* userdata, CIwGameControllerHandle* data);
    typedef void (*IwGameControllerPauseCallback)(void* userdata, CIwGameControllerHandle* data);
    
    /**
    Button press/release callback data
    */
    struct CIwGameControllerButtonEvent
    {
        int         Button;
        int         Pressed;
        CIwGameControllerButtonEvent();
    };

    /**
    Button press/release callback
    */
    typedef void (*IwGameControllerButtonCallback)(void* userdata, CIwGameControllerButtonEvent* data);
    
    //TODO: axis callback
    
    /**
    Abstract class that is implemented for each extension. Create class of desired type
    directly or use the static Create() function to get default controller class for
    current platform.
    */
    class CIwGameController
    {
    public:
        
    protected:
        CIwGameController();
        virtual ~CIwGameController() {}
        
        Type::eType                           m_Type; // TODO: might move this out to the _Any helper
        
        IwGameControllerConnectCallback       m_ConnectCallback;
        IwGameControllerDisconnectCallback    m_DisconnectCallback;
        IwGameControllerPauseCallback         m_PauseCallback;
        IwGameControllerButtonCallback        m_ButtonCallback;
        
        void*                                 m_ConnectCallbackUserdata;
        void*                                 m_DisconnectCallbackUserdata;
        void*                                 m_PauseCallbackUserdata;
        void*                                 m_ButtonCallbackUserdata;
        
        void                                  NotifyConnect(CIwGameControllerHandle* data);
        void                                  NotifyDisconnect(CIwGameControllerHandle* data);
        void                                  NotifyPause(CIwGameControllerHandle* data);
        void                                  NotifyButton(CIwGameControllerButtonEvent* data);
        
        //TODO - axis callbacks?
        
    public:
        void setConnectCallback(IwGameControllerConnectCallback callback, void* userdata)       { m_ConnectCallback = callback; m_ConnectCallbackUserdata = userdata}
        void setDisconnectCallback(IwGameControllerDisconnectCallback callback, void* userdata) { m_DisconnectCallback = callback; m_DisconnectCallbackUserdata = userdata}
        void setPauseCallback(IwGameControllerPauseCallback callback, void* userdata)           { m_PauseCallback = callback; m_PauseCallbackUserdata = userdata}
        void SetButtonCallback(IwGameControllerButtonCallback callback, void* userdata)         { m_ButtonCallback = callback; m_ButtonCallbackUserdata = userdata}
    
        // Call every frame before querying to make sure states are up to date
        // Does nothing on some platforms
        virtual void      StartFrame() = 0;
        
        virtual int       GetControllerCount() = 0;
        virtual int       GetMaxControllers() = 0;
        
        // Get opaque handle to controller. Index can be from 0 to GetControllerCount-1.
        // Handles are needed to query controller values
        virtual CIwControllerHandle* GetControllerByIndex(int index) = 0;
        
        // As with controller, but by player ID (0,1,2,etc) if supported
        // If the platform does not support player IDs then NULL
        virtual CIwControllerHandle* GetControllerByPlayer(int player) = 0;
        
        // Get state (true = pressed/down, false = released/up). You must get a valid handle using GetControllerByIndex
        // or GetControllerByPlayer. Does not check internally for NULL controller etc. Listen for
        // connect/disconnect events to keep handles to valid controllers.
        virtual bool      GetButtonState(CIwControllerHandle* handle, Button::eButton button) = 0;
        
        // Get position of axis control. Axes are centred at 0.0,
        // with -1.0 left/bottom and +1.0 top/right.
        // Same logic for controllers as GetButtonState
        // Returns 0.0 for unsupported axis
        virtual float     GetAxisValue(CIwControllerHandle* handle, Axis::eAxis axis) = 0;
        
        // Check if keys are supported. Passing NULL for the controller returns whether the OS
        // supports that value at all.
        virtual bool      IsButtonSupported(CIwControllerHandle* handle, Button::eButton button) = 0;
        virtual bool      IsAxisSupported(CIwControllerHandle* handle, Axis::eAxis axis) = 0;
        
        // Enable/disable forwarding button presses to s3eKeyboard for any keys the device supports that for
        virtual void      SetPropagateButtonsToKeyboard(bool propagate) = 0;

        static bool GetButtonDisplayName(char* dst, Button::eButton button, bool terminateString);
        static bool GetAxisDisplayName(char* dst, Axis::eAxis axis, bool terminateString);
        
        Type::eType               GetType() const;
    };
    
}   // namespace IwGameController

#endif
