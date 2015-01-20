#include "QGameController.h"
#include "IwGameController.h"
#include "QLuaHelpers.h"

using namespace quick;

namespace gameController {

    //---- Callbacks ----
    // TODO copied out of s32eAndroidController - needs updating to use generic
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

    bool isAvailable()
    {
        return IwGameController::IsAvailable();
    }
    
    bool init(int type)
    {
        return IwGameController::Init((IwGameController::Type::eType)type);
    }

    void startFrame()
    {
        IwGameController::StartFrame();
    }

    bool selectControllerByPlayer(int player)
    {
        return IwGameController::SelectControllerByPlayer(player);
    }

    int getPlayerCount()
    {
        return IwGameController::GetPlayerCount();
    }
    
    int getMaxControllers()
    {
        return IwGameController::GetMaxControllers();
    }

    bool getButtonState(int button)
    {
        return IwGameController::GetButtonState((IwGameController::Button::eButton)button);
    }

    float getAxisValue(int axis)
    {
        return IwGameController::GetAxisValue((IwGameController::Axis::eAxis)axis);
    }

    char* getButtonDisplayName(int button)
    {
        char name[20];
        IwGameController::GetButtonDisplayName(name, (IwGameController::Button::eButton)button, true);
        return name;
    }

    char* getAxisDisplayName(int axis)
    {
        char name[20];
        IwGameController::GetAxisDisplayName(name, (IwGameController::Axis::eAxis)axis, true);
        return name;
    }

    void setPropagateButtonsToKeyboard(bool propagate)
    {
        IwGameController::SetPropagateButtonsToKeyboard(propagate);
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
