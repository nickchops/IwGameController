/*
 * (C) 2015 Nick Smith. All Rights Reserved.
 */
#ifndef IW_GAMECONTROLLER_ANY_H
#define IW_GAMECONTROLLER_ANY_H

#include <string>
#include "s3e.h"

/**
 This header provides static helpers to allow using IwGameController
 without touching explicit controller type classes. These are separated from
 the main header so that this can be updated for new controllers without
 touching generic code.
*/

//TODO: mght want to store type info totally separately to class so it doesnt pollute it

namespace IwGameController
{
    /**
    Checks if any controller is supported by the current device.
    This does not indicate if the controller is currently present.
    */
    bool IsSupported(Type::eType type = Type::ANY);
    
    /**
    Creats a controller object. You can set the type explicitly or leave as
    ANY for it to pick the default for the current device.
     
    @return  new controller object if it succeeds, NULL if it fails.
    */
    CIwGameController*    Create(Type::eType type = Type::ANY);

}   // namespace IwGameController

#endif
