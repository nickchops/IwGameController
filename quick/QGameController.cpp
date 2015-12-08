#include "QGameController.h"
#include "IwGameController_Any.h"
#include "QLuaHelpers.h"

using namespace quick;
using namespace IwGameController;

//-----------------------------------------------------
// Missing function needed for pushing void* light user data from C++ to Quick events
// There is a ticket to add this to the SDK, after which this should be removed.
// Include Lua headers
#ifdef __cplusplus
   extern "C" {
#endif
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#ifdef __cplusplus
}
#endif

#include "QDefines.h"
#include "QSystem.h"
#include "tolua++.h"

QUICK_NAMESPACE_BEGIN;

extern lua_State* g_L;
inline void _LUA_EVENT_SET_TOLUA_LIGHT_PTR(const char* name, void* ptr)
{
    lua_pushlightuserdata(g_L, ptr);
    lua_setfield(g_L, -2, name);
}

QUICK_NAMESPACE_END;
//-----------------------------------------------------

namespace gameController {

    CIwGameController* s_controller = NULL;

    //---- Callbacks ----

    // Currently only supporting one type of controller, so controller always == s_controller
    // We need to fix that soon....

    void ConnectHandler (CIwGameControllerHandle* controllerHandle, void* controller)
    {
        LUA_EVENT_PREPARE("controller");
        LUA_EVENT_SET_STRING("type", "connect");
        _LUA_EVENT_SET_TOLUA_LIGHT_PTR("controllerHandle", controllerHandle);
        LUA_EVENT_SEND();
    }

    void DisconnectHandler (CIwGameControllerHandle* controllerHandle, void* controller)
    {
        LUA_EVENT_PREPARE("controller");
        LUA_EVENT_SET_STRING("type", "disconnect");
        _LUA_EVENT_SET_TOLUA_LIGHT_PTR("controllerHandle", controllerHandle);
        LUA_EVENT_SEND();
    }

    void PauseHandler (CIwGameControllerHandle* controllerHandle, void* controller)
    {
        LUA_EVENT_PREPARE("controller");
        LUA_EVENT_SET_STRING("type", "pause");
        _LUA_EVENT_SET_TOLUA_LIGHT_PTR("controllerHandle", controllerHandle);
        LUA_EVENT_SEND();
    }

    // TODO Below is copied from s3eAndroidController - needs updating to use generic
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

    //-----------------------------------------------------------------------

    bool isSupported()
    {
        return IwGameController::IsSupported();
    }

    bool init(unsigned int type)
    {
        s_controller = IwGameController::Create((IwGameController::Type::eType)type);

        if (!s_controller)
            return false;
        else
        {
            s_controller->SetConnectCallback(ConnectHandler, s_controller);
            s_controller->SetDisconnectCallback(DisconnectHandler, s_controller);
            s_controller->SetPauseCallback(PauseHandler, s_controller);
            //s_controller->SetButtonCallback(ButtonHandler, s_controller);
            return true;
        }
    }

    void terminate()
    {
        if (s_controller)
        {
            delete s_controller;
            s_controller = NULL;
        }
    }

    unsigned int getType()
    {
        if (s_controller)
            return s_controller->GetType();
        else
            return gameController::typeNone;
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
            return s_controller->GetMaxControllers();
        else
            return 0;
    }

    void* getControllerByIndex(unsigned int index)
    {
        if (!s_controller || index < 1 || index > s_controller->GetMaxControllers())
            return NULL;
        else
            return s_controller->GetControllerByIndex(index-1);

    }

    void* getControllerByPlayer(unsigned int player)
    {
        if (s_controller)
            return s_controller->GetControllerByPlayer(player);
        else
            return NULL;
    }

    bool getButtonState(void* handle, unsigned int button)
    {
        if (s_controller)
            return s_controller->GetButtonState((CIwGameControllerHandle*)handle, (IwGameController::Button::eButton)button);
        else
            return false;
    }

    float getAxisValue(void* handle, unsigned int axis)
    {
        if (s_controller)
            return s_controller->GetAxisValue((CIwGameControllerHandle*)handle, (IwGameController::Axis::eAxis)axis);
        else
            return 0.0;
    }

    const char* getButtonDisplayName(unsigned int button)
    {
        if (button > IwGameController::Button::MAX)
            return "";

        return CIwGameController::s_ButtonNames[button];
    }

    const char* getAxisDisplayName(unsigned int axis)
    {
        if (axis > IwGameController::Axis::MAX)
            return "";

        return CIwGameController::s_AxisNames[axis];
    }

    //todo: make lua return a pair of (bool=success?, int=value)
    int getProperty(void* handle, unsigned int property)
    {
        if (property > propertyMax)
            return -1;

        if (!s_controller)
            return -1;

        return s_controller->GetProperty((CIwGameControllerHandle*)handle,
                (IwGameController::Property::eProperty)property);
    }

    //todo: make return true/false for success
    void setProperty(void* handle, unsigned int property, int value)
    {
        if (property > propertyMax)
            return;

        if (!s_controller)
            return;

        s_controller->SetProperty((CIwGameControllerHandle*)handle,
                (IwGameController::Property::eProperty)property, value);
    }

    unsigned int getControllerType(void* handle)
    {
        if (!s_controller)
            return controllerTypeUnknown;

        return (unsigned int)s_controller->GetControllerType((CIwGameControllerHandle*)handle);
    }

    bool isButtonSupported(void* handle, unsigned int button)
    {
        if (!s_controller)
            return controllerTypeUnknown;

        return s_controller->IsButtonSupported((CIwGameControllerHandle*)handle,
                (IwGameController::Button::eButton)button);
    }

    bool isAxisSupported(void* handle, unsigned int axis)
    {
        if (!s_controller)
            return controllerTypeUnknown;

        return s_controller->IsAxisSupported((CIwGameControllerHandle*)handle,
                (IwGameController::Axis::eAxis)axis);
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


    bool connect(bool dontBroadcast, const char* appName)
    {
        if (s_controller && s_controller->GetType() == IwGameController::Type::MARMALADE_REMOTE)
            return ((CIwGameControllerMarmaladeRemote*)s_controller)->Connect(dontBroadcast, appName);

        return false;
    }

    void disconnect()
    {
        if (s_controller && s_controller->GetType() == IwGameController::Type::MARMALADE_REMOTE)
            ((CIwGameControllerMarmaladeRemote*)s_controller)->Disconnect();
    }

    bool isConnecting()
    {
        if (s_controller && s_controller->GetType() == IwGameController::Type::MARMALADE_REMOTE)
            return ((CIwGameControllerMarmaladeRemote*)s_controller)->IsConnecting();

        return false;
    }

    bool isConnected()
    {
        if (s_controller && s_controller->GetType() == IwGameController::Type::MARMALADE_REMOTE)
            return ((CIwGameControllerMarmaladeRemote*)s_controller)->IsConnected();

        return false;
    }

    void setConnectTimeout(float seconds)
    {
        if (s_controller && s_controller->GetType() == IwGameController::Type::MARMALADE_REMOTE)
            ((CIwGameControllerMarmaladeRemote*)s_controller)->SetConnectTimeout(seconds);
    }

    void setKeepAliveTimeout(float seconds)
    {
        if (s_controller && s_controller->GetType() == IwGameController::Type::MARMALADE_REMOTE)
            ((CIwGameControllerMarmaladeRemote*)s_controller)->SetKeepAliveTimeout(seconds);
    }

    void resetValues()
    {
        if (s_controller && s_controller->GetType() == IwGameController::Type::MARMALADE_REMOTE)
            ((CIwGameControllerMarmaladeRemote*)s_controller)->ResetValues();
    }

    void setIgnoreTimeouts(bool ignore, bool blockSender)
    {
        if (s_controller && s_controller->GetType() == IwGameController::Type::MARMALADE_REMOTE)
            ((CIwGameControllerMarmaladeRemote*)s_controller)->SetIgnoreTimeouts(ignore, blockSender);
    }

} // namespace gameController
