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

namespace IwGameController
{
    
class CIwGameControllerIOS : public IwGameController
{
private:
    static CIwGameControllerIOS* _instance;
    CIwGameControllerIOS() {}
    virtual ~CIwGameControllerIOS() {}
    CIwGameControllerIOS(const CIwGameControllerIOS &);
    CIwGameControllerIOS& operator=(const CIwGameControllerIOS &);

public:
    static void Create();
    static void Destroy();
    static CIwGameControllerIOS* getInstance();

    bool                Init();
    void                Release();

    void                StartFrame();
    bool                SelectControllerByPlayer(int player);
    int                 GetPlayerCount();
    int                 GetMaxControllers();
    bool                GetButtonState(Button::eButton button);
    float               GetAxisValue(Axis::eAxis axis);
    void                SetPropagateButtonsToKeyboard(bool propagate);
};

#define IW_GAMECONTROLLER_IOS (CIwGameControllerIOS::getInstance())

}   // namespace IwGameController

#endif
