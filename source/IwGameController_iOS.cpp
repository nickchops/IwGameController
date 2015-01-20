/*
 * (C) 2015 Nick Smith.
 */

#include "IwGameController_iOS.h"

namespace IwGameController
{

// ----------- Init terminate etc --------------

CIwGameControllerIOS* CIwGameControllerIOS::_instance = 0;

void CIwGameControllerIOS::Create()
{
    if (_instance == 0)
        _instance = new CIwGameControllerIOS();
}
void CIwGameControllerIOS::Destroy()
{
    if (_instance != 0)
    {
        delete _instance;
        _instance = 0;
    }
}
CIwGameControllerIOS* CIwGameControllerIOS::getInstance()
{
    return _instance;
}

bool CIwGameControllerIOS::Init(Type::eType type)
{
    if (!IwGameController::Init(type))
        return false;

    //extension init here if needed

    return true;
}

void CIwGameControllerIOS::Release()
{
    IwGameController::Release();

    //extension terminate here if appropriate
}


// -------- Extension wrapping functions ---------------

void CIwGameControllerIOS::StartFrame()
{
}

bool CIwGameControllerIOS::SelectControllerByPlayer(int player)
{
    return false;
}

int CIwGameControllerIOS::GetPlayerCount()
{
    return 0;
}

int CIwGameControllerIOS::GetMaxControllers()
{
    return 0;
}

bool CIwGameControllerIOS::GetButtonState(Button::eButton button)
{
    return false;
}

float CIwGameControllerIOS::GetAxisValue(Axis::eAxis axis)
{
    return 0.0;
}

void CIwGameControllerIOS::SetPropagateButtonsToKeyboard(bool propagate)
{
}

}   // namespace IwGameController
