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
            STICK_LEFT_X,
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

	// Properties. Can be generic or controller specific. Possible values are: 0 = false, 1 = true, -1 = unknown
	struct Property
	{
		enum eProperty
		{
			PROPAGATE_BUTTONS_TO_KEYBOARD = 0, // Enable/disable forwarding button presses to s3eKeyboard for any keys the device supports that for. Applied to all constrollers. Android only atm.
			REPORTS_ABSOLUTE_DPAD_VALUES,      // For controllers with touch pads, determines if (0,0) is centre of pad (1) or place finger is first placed (0). Must specify a controller. tvOS only atm.
			ALLOWS_ROTATION,                   // Determines if axis will rotate when controller device is rotated. Must specify a controller. tvOS only atm.
			MAX
		};
	};

	struct ControllerType
	{
		enum eControllerType
		{
			UNKNOWN = 0, // Returned by all non-iOS devices atm. Could be a BASIC or EXTENDED pad, but not known.
			BASIC,       // Basic pad wiht dpad, A B X Y. Returned by iOS only atm.
			EXTENDED,    // Full pad with left and right sticks. Returned by iOS only atm.
			MICRO,       // tvOS controller with analog pad, A and X buttons.
			MAX
		};
	};
    
    // Opaque controller reference
	// For platforms with multiple controller support, you must get a controller handle
	// with GetControllerByIndex, GetControllerByPlayer or a callback and then
	// pass that to button/axis state checking functions. Passing null controllers
	// in this case will return default values (0.0/false).
	// For platforms that don't support multiple controllers, it is valid to pass NULL
	// (or any thing the compiler accepts) as the handle; the handle is just ignored.
	// Use GetMaxControllers() == 1 to check if handles are needed.
    typedef struct CIwGameControllerHandle CIwGameControllerHandle;

	typedef void(*IwGameControllerConnectCallback)(CIwGameControllerHandle* data, void* userdata);
	typedef void(*IwGameControllerDisconnectCallback)(CIwGameControllerHandle* data, void* userdata);
	typedef void(*IwGameControllerPauseCallback)(CIwGameControllerHandle* data, void* userdata);
    
    /**
    Button press/release callback data
    */
    struct CIwGameControllerButtonEvent
    {
		CIwGameControllerHandle* m_Controller;
        int         m_Button;
        int         m_Pressed;
        CIwGameControllerButtonEvent();
    };

    /**
    Button press/release callback
    */
	typedef void(*IwGameControllerButtonCallback)(CIwGameControllerButtonEvent* data, void* userdata);
    
    //TODO: axis callback if any platforms support them
    
    /**
    Abstract class that is implemented for each extension. Create class of desired type
    directly or use the static Create() function to get default controller class for
    current platform.
    */
    class CIwGameController
    {
    public:
        static const char* const s_ButtonNames[Button::MAX];
        static const char* const s_AxisNames[Axis::MAX];

    protected:
        CIwGameController();
        virtual ~CIwGameController() {}
        
        Type::eType                           m_Type; // TODO: might move this out to the _Any helper?
        
        static IwGameControllerConnectCallback       s_ConnectCallback;
		static IwGameControllerDisconnectCallback    s_DisconnectCallback;
		static IwGameControllerPauseCallback         s_PauseCallback;
		static IwGameControllerButtonCallback        s_ButtonCallback;
        
		static void*                                 s_ConnectCallbackUserdata;
		static void*                                 s_DisconnectCallbackUserdata;
		static void*                                 s_PauseCallbackUserdata;
		static void*                                 s_ButtonCallbackUserdata;
        
        static void                           NotifyConnect(CIwGameControllerHandle* data);
		static void                           NotifyDisconnect(CIwGameControllerHandle* data);
        static void                           NotifyPause(CIwGameControllerHandle* data);
        static void                           NotifyButton(CIwGameControllerButtonEvent* data);
        
        //TODO - axis callbacks? if any platforms support them...
        
    public:
		void setConnectCallback(IwGameControllerConnectCallback callback, void* userdata)       { s_ConnectCallback = callback; s_ConnectCallbackUserdata = userdata; }
		void setDisconnectCallback(IwGameControllerDisconnectCallback callback, void* userdata) { s_DisconnectCallback = callback; s_DisconnectCallbackUserdata = userdata; }
		void setPauseCallback(IwGameControllerPauseCallback callback, void* userdata)           { s_PauseCallback = callback; s_PauseCallbackUserdata = userdata; }
		void SetButtonCallback(IwGameControllerButtonCallback callback, void* userdata)         { s_ButtonCallback = callback; s_ButtonCallbackUserdata = userdata; }
    
        // Call every frame before querying to make sure states are up to date
        // Does nothing on some platforms
        virtual void      StartFrame() = 0;
        
		// Counts
        virtual int       GetControllerCount() = 0;
        virtual int       GetMaxControllers() = 0;

		// Properties. handle can be NULL and will be ignored for general properties
		virtual int       GetProperty(CIwGameControllerHandle* handle, Property::eProperty prop) = 0;
		virtual void      SetProperty(CIwGameControllerHandle* handle, Property::eProperty prop, int value) = 0;
		virtual ControllerType::eControllerType GetControllerType(CIwGameControllerHandle* handle) = 0;
        
        // Get opaque handle to controller. Index can be from 0 to GetControllerCount-1.
        // Handles are needed to query controller values
        virtual CIwGameControllerHandle* GetControllerByIndex(int index) = 0;
        
        // As with controller, but by player ID (0,1,2,etc) if supported
        // If the platform does not support player IDs then NULL
        virtual CIwGameControllerHandle* GetControllerByPlayer(int player) = 0;

        // Get state (true = pressed/down, false = released/up). You must get a valid handle using GetControllerByIndex
        // or GetControllerByPlayer. Does not check internally for NULL controller etc. Listen for
        // connect/disconnect events to keep handles to valid controllers.
        virtual bool      GetButtonState(CIwGameControllerHandle* handle, Button::eButton button) = 0;
        
        // Get position of axis control. Axes are centred at 0.0,
        // with -1.0 left/bottom and +1.0 top/right.
        // Same logic for controllers as GetButtonState
        // Returns 0.0 for unsupported axis
        virtual float     GetAxisValue(CIwGameControllerHandle* handle, Axis::eAxis axis) = 0;
        
        // Check if keys are supported. Passing NULL for the controller returns whether the OS
        // supports that value at all.
        virtual bool      IsButtonSupported(CIwGameControllerHandle* handle, Button::eButton button) = 0;
        virtual bool      IsAxisSupported(CIwGameControllerHandle* handle, Axis::eAxis axis) = 0;

        static bool GetButtonDisplayName(char* dst, Button::eButton button, bool terminateString);
        static bool GetAxisDisplayName(char* dst, Axis::eAxis axis, bool terminateString);
        
        Type::eType               GetType() const;
    };
    
}   // namespace IwGameController

#endif
