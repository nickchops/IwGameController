#include "QGameController.h"
#include "IwGameController_Any.h"
#include "QLuaHelpers.h"

using namespace quick;
using namespace IwGameController;

namespace gameController {

    //---- Callbacks ----
    // TODO copied out of s3eAndroidController - needs updating to use generic
    //IwGameController wrapper callbacks and not specific extension
    /* 
    int32 onButtonEvent(void* systemData, void* userData)
    {
        QTrace("gameController.onButtonEvent");
        
        IwGameController::ButtonEvent* data = static_cast<IwGameController::ButtonEvent*>(systemData);

        if (data == NULL)
        {
            QTrace("onButtonEvent error: callback data is NULL.");
            return 1;
        }
        
        // Note that we are still just using the android key codes.
        // When we extend this api to be cross platform, we'll need to map them
        // (likely that will happen in C++ interface though)
        // Could have reused the "key" event but decided not to to avoid potential
        // clashes in future updates
        LUA_EVENT_PREPARE("controller");
        LUA_EVENT_SET_INTEGER("keyCode", data->m_Button);
        if (data->m_Pressed)
        {
            LUA_EVENT_SET_STRING("phase", "pressed");
        }
        else
        {
            LUA_EVENT_SET_STRING("phase", "released");
        }
        LUA_EVENT_SET_INTEGER("device", data->m_Device);
        LUA_EVENT_SET_INTEGER("player", data->m_Player);
        
        LUA_EVENT_SEND();
        lua_pop(g_L, 1);
        return 0;
    }
    */
    
    CIwGameController* s_controller = NULL;

    bool isSupported()
    {
        return IwGameController::IsSupported();
    }
    
    bool init(int type)
    {
        s_controller = IwGameController::Create((IwGameController::Type::eType)type);
        
        if (!s_controller)
            return false;
        else
            return true;
    }
    
    void terminate()
    {
        if (s_controller)
        {
            delete s_controller;
            s_controller = NULL;
        }
    }

    void startFrame()
    {
        if (s_controller)
            s_controller->StartFrame();
    }

    int getControllerCount()
    {
        if (s_controller)
            return s_controller->GetControllerCount();
        else
            return 0;
    }
    
    int getMaxControllers()
    {
        if (s_controller)
            s_controller->GetMaxControllers();
        else
            return 0;
    }
    
    void* getControllerByIndex(int player)
    {
        return NULL;
    }
    
    void* getControllerByPlayer(int player)
    {
        return NULL;
    }

    bool getButtonState(void* handle, int button)
    {
        if (s_controller)
            s_controller->GetButtonState((CIwGameControllerHandle*)handle, (IwGameController::Button::eButton)button);
        else
            return false;
    }

    float getAxisValue(void* handle, int axis)
    {
        if (s_controller)
            s_controller->GetAxisValue((CIwGameControllerHandle*)handle, (IwGameController::Axis::eAxis)axis);
        else
            return 0.0;
    }

    const char* getButtonDisplayName(int button)
    {
        if (button < 0 || button < IwGameController::Button::MAX)
            return "";
        
        return CIwGameController::s_ButtonNames[button];
    }

    const char* getAxisDisplayName(int axis)
    {
        if (axis < 0 || axis < IwGameController::Axis::MAX)
            return "";
        
        return CIwGameController::s_AxisNames[axis];
    }
    
    int getProperty(void* handle, char* property)
    {
        return 0;
    }
    
    void setProperty(void* handle, char* property, int value)
    {
    }
    
    int getControllerType(void* handle)
    {
        return 0;
    }

    bool isButtonSupported(void* handle, int button)
    {
        return false;
    }
    
    bool isAxisSupported(void* handle, int axis)
    {
        return false;
    }
    
    void useButtonEvents(bool enabled)
    {
        /*
        if (enabled)
        {
            IwGameController::Register(S3E_ANDROIDCONTROLLER_CALLBACK_BUTTON, onButtonEvent,  NULL);
        }
        else
        {
            IwGameController::UnRegister(S3E_ANDROIDCONTROLLER_CALLBACK_BUTTON, onButtonEvent);
        }
        */
    }
    
} // namespace gameController
