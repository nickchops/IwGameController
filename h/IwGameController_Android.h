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

namespace IwGameController
{
    
class CIwGameControllerAndroid : public IwGameController
{
private:
    static CIwGameControllerAndroid* _instance;
    CIwGameControllerAndroid() {}
    virtual ~CIwGameControllerAndroid() {}
    CIwGameControllerAndroid(const CIwGameControllerAndroid &);
    CIwGameControllerAndroid& operator=(const CIwGameControllerAndroid &);

public:
    static void Create();
    static void Destroy();
    static CIwGameControllerAndroid* getInstance();

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

#define IW_GAMECONTROLLER_ANDROID (CIwGameControllerAndroid::getInstance())

}   // namespace IwGameController

#endif
