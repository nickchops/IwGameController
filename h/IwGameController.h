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
    // TODO: might move these into the class itself if I remove the probably
    // unnecessary static namespace functions and just have the user
    // directly access an IwBilling object... 
    struct Type
    {
        enum eType
        {
            ANY = 0,                        // Input only - check for any type available. Use the default type for the platform.
            ANDROID_ANY,                //Input only - use best android controller type available.
            ANDROID_GENERIC,
            ANDROID_OUYA_EVERYWHERE,
            ANDROID_AMAZON,
            IOS,
            DESKTOP_HID,
            NONE                        // Output only - indicates no controller type has been initialised.
        };
    };
    
    struct Axis
    {
        enum eAxis
        {
            STICK_LEFT_X = 0,
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
            MAX
        };
    };
    
    /**
    EVENTS NOT YET IMPLEMENTED
    Button press/release callback data
    */
    struct CIwGameControllerButtonEvent
    {
        int         Button;
        int         Pressed;
        CIwGameControllerButtonEvent();
    };

    /**
    Button callback
    */
    typedef void (*IwGameControllerButtonCallback)(void* userdata, CIwGameControllerButtonEvent* data);

    /**
    Abstract class that is implemented for each extension. Do not use the
    classes directly; use the static namespace functions. Class objects are
    instantiated and destroyed as needed.
    */
    class IwGameController
    {
    public:
        
    protected:
        IwGameController();
        virtual ~IwGameController() {}
        
        static IwGameController*    m_CurrentGameController;
        Type::eType                 m_Type;
        
        IwGameControllerButtonCallback    ButtonCallback;
        void*                             ButtonCallbackData;
        
    public:
        virtual void      StartFrame() = 0;
        virtual bool      SelectControllerByPlayer(int player) = 0;
        virtual int       GetPlayerCount() = 0;
        virtual int       GetMaxControllers() = 0;
        virtual bool      GetButtonState(Button::eButton button) = 0;
        virtual float     GetAxisValue(Axis::eAxis axis) = 0;
        virtual void      SetPropagateButtonsToKeyboard(bool propagate) = 0;

        void      SetButtonCallback(IwGameControllerButtonCallback callback) { ButtonCallback = callback; }
        void      NotifyButtonEvent(CIwGameControllerButtonEvent* data);
        
        bool                      Init(Type::eType type);
        void                      Release();
        
        Type::eType               GetType() const;
    
        static IwGameController*  Create(Type::eType type);
        static void               Destroy();
        static IwGameController*  GetGameController() { return IwGameController::m_CurrentGameController; }
    };
    
    #define IW_GAMECONTROLLER  (IwGameController::GetGameController())

    /**
    Check if specified controller type is supported. Does not indicate
    whether an actual controller is connected.
    
    Type::ANY returns true if any extension is supported on the
    current device.
    
    Note that this call will initialise the internal s3e extension used
    and any memory associated with that, which is only freed on app exit.
    Typically you just want to call with the default "any" param and then
    call Init()
    
    @return  true if available, false if not.
    */
    bool    IsAvailable(Type::eType type = Type::ANY);

    /**
    Initialises the IwGameController system.
    
    Set the controller type (extension) to use or use the best fit for the
    device via the default.
    
    @return  true if it succeeds, false if it fails.
    */
    bool    Init(Type::eType type = Type::ANY);

    /**
    Terminates the IwGameController system.
    
    Clears any data/objects used. Cannot be used again until it has been
    re-created with Init().
    
    This will not terminate the s3e extension used internally but will
    unregister callbacks etc.
    
    */
    void    Terminate();
    
    /**
    Get the current controller type.
    
    Returns CONTROLLER_TYPE_NONE if system hasn't been initialised.
    
    @return  Current controller type.
    */
    Type::eType GetType();

    void                  StartFrame();
    bool                  SelectControllerByPlayer(int player);
    int                   GetPlayerCount();
    int                   GetMaxControllers();
    bool                  GetButtonState(Button::eButton button);
    float                 GetAxisValue(Axis::eAxis axis);
    bool                  GetButtonDisplayName(char* dst, Button::eButton button, bool terminateString=true);
    bool                  GetAxisDisplayName(char* dst, Axis::eAxis axis, bool terminateString=true);
    void                  SetPropagateButtonsToKeyboard(bool propagate);


}   // namespace IwGameController

#endif
