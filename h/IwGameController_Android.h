/*
 * (C) 2015 Nick Smith
 *
 * Android Implementation of IwGameController
 */
 
#ifndef IW_GAMECONTROLLER_ANDROID_H
#define IW_GAMECONTROLLER_ANDROID_H

#include "s3e.h"
#include "s3eAndroidController.h"
#include "IwGameController.h"

namespace IwGameController {
    
class CIwGameControllerAndroid : public CIwGameController
{
public:
    CIwGameControllerAndroid(Type::eType type);
    virtual ~CIwGameControllerAndroid();

    void    StartFrame();
    bool    SelectControllerByPlayer(int player);
    int     GetControllerCount();
    int     GetMaxControllers();
    bool    GetButtonState(CIwControllerHandle* handle, Button::eButton button);
    float   GetAxisValue(CIwControllerHandle* handle, Axis::eAxis axis);
    bool    IsButtonSupported(CIwControllerHandle* handle, Button::eButton button);
    bool    IsAxisSupported(CIwControllerHandle* handle, Axis::eAxis axis);
    void    SetPropagateButtonsToKeyboard(bool propagate);
};

}   // namespace IwGameController

#endif
