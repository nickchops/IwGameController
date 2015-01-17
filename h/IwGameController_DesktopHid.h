/*
 * (C) 2015 Nick Smith
 *
 * DesktopHid Implementation of IwGameController
 */
 
#ifndef IW_GAMECONTROLLER_DESKTOP_HID_H
#define IW_GAMECONTROLLER_DESKTOP_HID_H

#include "s3e.h"
#include "s3eHidController.h"
#include "IwGameController.h"

namespace IwGameController
{
    
class CIwGameControllerDesktopHid : public IwGameController
{
private:
    static CIwGameControllerDesktopHid* _instance;
    CIwGameControllerDesktopHid() {}
    virtual ~CIwGameControllerDesktopHid() {}
    CIwGameControllerDesktopHid(const CIwGameControllerDesktopHid &);
    CIwGameControllerDesktopHid& operator=(const CIwGameControllerDesktopHid &);

public:
    static void Create();
    static void Destroy();
    static CIwGameControllerDesktopHid* getInstance();

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

#define IW_GAMECONTROLLER_DESKTOP_HID (CIwGameControllerDesktopHid::getInstance())

}   // namespace IwGameController

#endif
