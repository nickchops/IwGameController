/*
 * (C) 2015 Nick Smith
 *
 * iOS Implementation of IwGameController
 */
 
#ifndef IW_GAMECONTROLLER_IOS_H
#define IW_GAMECONTROLLER_IOS_H

#include "s3e.h"
#include "s3eIOSController.h"
#include "IwGameController.h"

namespace IwGameController {
    
class CIwGameControllerIOS : public CIwGameController
{
    
private:
    bool m_ButtonState[Button::MAX] = {false};
    
public:
    CIwGameControllerIOS();
    virtual ~CIwGameControllerIOS();

    void    StartFrame();
    bool    SelectControllerByPlayer(int player);
    int     GetControllerCount();
    int     GetMaxControllers();
    bool    GetButtonState(Button::eButton button);
    float   GetAxisValue(Axis::eAxis axis);
    bool    IsButtonSupported(CIwControllerHandle* handle, Button::eButton button);
    bool    IsAxisSupported(CIwControllerHandle* handle, Axis::eAxis axis);
    void    SetPropagateButtonsToKeyboard(bool propagate);
};

}   // namespace IwGameController

#endif
