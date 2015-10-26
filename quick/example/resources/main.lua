
--require("mobdebug").start() -- Uncomment for ZeroBrain IDE debuger support

--We're using these visual pads and buttons to show real pad and button states!
dofile("helpers/OnScreenDPad.lua")
dofile("helpers/OnScreenButton.lua")
dofile("helpers/VirtualResolution.lua")

appWidth = 640
appHeight = 480
virtualResolution:initialise{userSpaceW=appWidth, userSpaceH=appHeight}

function orientation()
    virtualResolution:update()
    virtualResolution:applyToScene(director:getCurrentScene())
    screenWidth = virtualResolution:winToUserSize(director.displayWidth)
    screenHeight = virtualResolution:winToUserSize(director.displayHeight)
    
    if lblSupported then
        local leftEdge = virtualResolution.userWinMinX + 10
        lblSupported.x = leftEdge
        lblStickDbgL.x = leftEdge
        lblStickDbgR.x = lblStickDbgL.x + 300
        lblNumPlayers.x = leftEdge
        lblcontrollerSelected.x = leftEdge
        lblKeyBtns.x = leftEdge
        
        btnPropOn.x = leftEdge + btnPropX
        btnPropOff.x = leftEdge + btnPropX + touchBtnW + 10
        
        touchPad.x = virtualResolution.userWinMaxX - 70
    end
end
orientation()
system:addEventListener({"orientation"}, orientation)

numControllers = 0
controllerSelected = "none"
controllerHandle = nil

fontScale = 0.7

-- Update event does actual controller state handling ------------------------

dbg.print(gameController.axisStickLeftX)
dbg.print(gameController.axisStickLeftY)
dbg.print(gameController.axisStickRightX)
dbg.print(gameController.axisStickRightY)

function triggerState(triggger, pressed)
    if type(pressed) ~= "number" then
        if pressed == true then
            pressed = 1
        else
            pressed = 0
        end
    end
    triggger.alpha = 0.5 + pressed * 0.5
    triggger.strokeAlpha = 0.5 + pressed * 0.5
    triggger.xScale = 0.7 + pressed * 0.3
    triggger.yScale = 0.7 + pressed * 0.3
end

function update(event)
    --using the polling api. TODO: also show event results
    gameController.startFrame()
    
    -- NB, flipping Y axis as opposite to Quick's visual axis!
    
    local lx = gameController.getAxisValue(controllerHandle, gameController.axisStickLeftX)
    local ly = -gameController.getAxisValue(controllerHandle, gameController.axisStickLeftY)
    local rx = gameController.getAxisValue(controllerHandle, gameController.axisStickRightX)
    local ry = -gameController.getAxisValue(controllerHandle, gameController.axisStickRightY)
    
    leftPad:setTopAsFraction(lx,ly)
    rightPad:setTopAsFraction(rx,ry)
    
    lblStickDbgL.text = string.format("Left Stick: (%.5f,%.5f)", lx, ly)
    lblStickDbgR.text = string.format("Right Stick: (%.5f,%.5f)", rx, ry)
    
    px = gameController.getAxisValue(controllerHandle, gameController.axisDpadX)
    py = -gameController.getAxisValue(controllerHandle, gameController.axisDpad)
    
    touchPad.point.x = touchPad.pad.w/2*px
    touchPad.point.y = -touchPad.pad.h/2*py
    
    if gameController.getButtonState(controllerHandle, gameController.buttonDPadTouch) then
        touchPad.point.isVisible = true
    else
        touchPad.point.isVisible = false
    end
    
    if gameController.getButtonState(controllerHandle, gameController.buttonA) then
        touchPad.pad.color={100,100,100}
    else
        touchPad.pad.color=grey
    end
    
    if gameController.getButtonState(controllerHandle, gameController.buttonX) then
        touchPad.playPause.color={100,100,100}
    else
        touchPad.playPause.color=grey
    end
    
    btnA:setState(gameController.getButtonState(controllerHandle, gameController.buttonA))
    btnB:setState(gameController.getButtonState(controllerHandle, gameController.buttonB))
    btnX:setState(gameController.getButtonState(controllerHandle, gameController.buttonX))
    btnY:setState(gameController.getButtonState(controllerHandle, gameController.buttonY))
    
    btnUp:setState(gameController.getButtonState(controllerHandle, gameController.buttonDPadUp))
    btnDown:setState(gameController.getButtonState(controllerHandle, gameController.buttonDPadDown))
    btnLeft:setState(gameController.getButtonState(controllerHandle, gameController.buttonDPadLeft))
    btnRight:setState(gameController.getButtonState(controllerHandle, gameController.buttonDPadRight))
    
    if gameController.getButtonState(controllerHandle, gameController.buttonStickLeft) then
        leftPad.top.color = color.white
    else
        leftPad.top.color = leftPad.topColor
    end
    
    if gameController.getButtonState(controllerHandle, gameController.buttonStickRight) then
        rightPad.top.color = color.white
    else
        rightPad.top.color = rightPad.topColor
    end
    
    triggerState(btnShoulderLeft, gameController.getButtonState(controllerHandle, gameController.buttonShoulderLeft))
    triggerState(btnShoulderRight, gameController.getButtonState(controllerHandle, gameController.buttonShoulderRight))

    triggerState(triggerLeft, gameController.getAxisValue(controllerHandle, gameController.axisTriggerLeft))
    triggerState(triggerRight, gameController.getAxisValue(controllerHandle, gameController.axisTriggerRight))
end


-- Periodic check for new/lost controllers ------------------------------------

function checkControllers(event)
    numControllers = gameController.getControllerCount()

    controllerHandle = nil
    
    if numControllers == 1 and gameController.getMaxControllers() == 1 then
        controllerSelected = 1
    else
        local n = 0
        repeat
            n = n+1
            controllerHandle = gameController.getControllerByIndex(n)
        until controllerHandle or n == gameController.getMaxControllers()
            
        if controllerHandle == nil then
            controllerSelected = "none"
        else
            controllerSelected = n
        end
    end
end


-- Simple buttons for key propagation -----------------------------------------
--TODO: not showing keys yet plus need non-touch way to press buttons!
--This bit is a bit pointless atm...
touchBtnW=appWidth/6
touchBtnH=appHeight/18

function touchKeyEventsOn(event)
    tween:from(event.target, {xScale=0.9})
    gameController.setProperty(controllerHandle, propertyPropagateButtonsToKeyboard, 1)
end
local function touchKeyEventsOff(event)
    tween:from(event.target, {xScale=0.9})
    gameController.setProperty(controllerHandle, propertyPropagateButtonsToKeyboard, 0)
end

function addButton(text, touchEvent, x, y)
    local button = director:createRectangle({xAnchor=0.5, yAnchor=0.5, x=x, y=y, w=touchBtnW, h=touchBtnH, color=color.blue, strokeColor=color.darkBlue})
    button:addChild(director:createLabel({x=0, y=5, hAlignment="centre", vAlignment="centre", text=text, w=touchBtnW/fontScale, h=touchBtnH/fontScale, color=color.lightBlue, xScale=fontScale, yScale=fontScale}))
    button:addEventListener("touch", touchEvent)
    return button
end


-- Main rendering -------------------------------------------------------------

lblSupported = director:createLabel({x=10, y=appHeight-30, w=(appWidth-20)/fontScale, vAlignment="bottom", text="", color=color.white})

lblStickDbgL = director:createLabel({x=10, y=45, w=(appWidth/2)/fontScale, vAlignment="bottom", text="Left Stick: (0.00000,0.00000)", color=color.white, xScale=fontScale, yScale=fontScale})

lblStickDbgR = director:createLabel({x=310, y=45, w=(appWidth/2)/fontScale, vAlignment="bottom", text="Right Stick: (0.00000,0.00000)", color=color.white, xScale=fontScale, yScale=fontScale})

btnPropX = 380

if gameController.isSupported() then
    gameController.init()
    lblSupported.text = "Controller API available :)"
    lblSupported.color = color.green
    system:addEventListener({"update"}, update)
    checkControllers()
    system:addTimer(checkControllers, 5) --check for changes every 5 seconds
    
    lblKeyBtns = director:createLabel({x=10, y=10, w=(appWidth-100)/fontScale, vAlignment="bottom", text="Generate key events? ->", color=color.white})
    
    btnPropOn = addButton("Enable", touchKeyEventsOn, btnPropX, touchBtnH/2+10)
    btnPropOff = addButton("Disable", touchKeyEventsOff, btnPropOn.x+touchBtnW+10, btnPropOn.y)
else
    lblSupported.text = "Controller API NOT available :("
    lblSupported.color = color.red
end

lblNumPlayers = director:createLabel({x=10, y=appHeight-60, w=(appWidth-20)/fontScale, vAlignment="bottom", text="Controllers found: " .. numControllers, color=color.white})

lblcontrollerSelected = director:createLabel({x=10, y=appHeight-90, w=(appWidth-20)/fontScale, vAlignment="bottom", text="Controller selected: " .. controllerSelected, color=color.white})

--pads and buttons to show controller state

topRowY = appHeight*0.3+50
bottomRowY = appHeight*0.3

leftPadX = appWidth/6
dPadX = appWidth/6*2
rightPadX = appWidth/6*4
btnsX = appWidth/6*5

scale3d = 0.6

leftPad =  OnScreenDPad.Create({x=leftPadX,   y=topRowY, baseRadius=appWidth/13, topRadius=appWidth/23})
rightPad = OnScreenDPad.Create({x=rightPadX, y=bottomRowY, baseRadius=appWidth/11, topRadius=appWidth/21})
leftPad.origin.yScale = scale3d
rightPad.origin.yScale = scale3d

btnB = OnScreenButton.Create({x=btnsX+36, y=topRowY, radius=13, topColor=color.red, baseColor=color.darkRed, scale3d=scale3d, depth3d=4.5})
btnA = OnScreenButton.Create({x=btnsX, y=topRowY-17, radius=15, topColor=color.green, baseColor=color.darkGreen, scale3d=scale3d, depth3d=5})
btnX = OnScreenButton.Create({x=btnsX-36, y=topRowY, radius=13, topColor=color.blue, baseColor={0,0,140}, scale3d=scale3d, depth3d=4.5})
btnY = OnScreenButton.Create({x=btnsX, y=topRowY+13, radius=12, topColor=color.yellow, baseColor={90,90,0}, scale3d=scale3d, depth3d=3.5})

grey = {70,70,70}
darkishGrey = {55,55,55}
darkGrey = {50,50,50}
darkerGrey = {30,30,30}

dPadLines = {-58,10, -17,10, -16,26, 16,26, 17,10, 58,10, 61,-12, 19,-12, 20,-34, -20,-34, -19,-12, -61,-12, -58,10}
director:createLines({x=dPadX, y=bottomRowY, coords=dPadLines, strokeWidth=0, color=darkerGrey})
director:createLines({x=dPadX, y=bottomRowY, coords=dPadLines, strokeWidth=3, strokeColor=darkGrey, alpha=0})

btnRight = OnScreenButton.Create({x=dPadX+40, y=bottomRowY, radius=15, topColor=grey, baseColor=darkGrey, scale3d=scale3d, depth3d=5.5})
btnDown = OnScreenButton.Create({x=dPadX, y=bottomRowY-20, radius=17, topColor=grey, baseColor=darkGrey, scale3d=scale3d, depth3d=6})
btnLeft = OnScreenButton.Create({x=dPadX-40, y=bottomRowY, radius=15, topColor=grey, baseColor=darkGrey, scale3d=scale3d, depth3d=5.5})
btnUp = OnScreenButton.Create({x=dPadX, y=bottomRowY+15, radius=14, topColor=grey, baseColor=darkGrey, scale3d=scale3d, depth3d=5})

topTriggerY = appHeight*0.23+120
bottomTriggerY = appHeight*0.23+90

leftPadX = appWidth/6
dPadX = appWidth/6*2
rightPadX = appWidth/6*4
btnsX = appWidth/6*5

btnShoulderLeft = director:createRectangle({xAnchor=0.5, yAnchor=0.5, x=dPadX+15, y=topTriggerY, w=80, h=15, color=grey, strokeColor=darkGrey, strokeWidth=2})
btnShoulderRight = director:createRectangle({xAnchor=0.5, yAnchor=0.5,  x=rightPadX-15, y=topTriggerY, w=80, h=15, color=grey, strokeColor=darkGrey, strokeWidth=2})

triggerLeft = director:createRectangle({xAnchor=0.5, yAnchor=0.5,  x=dPadX+10, y=bottomTriggerY, w=80, h=25, color=grey, strokeColor=darkGrey, strokeWidth=2})
triggerRight = director:createRectangle({xAnchor=0.5, yAnchor=0.5,  x=rightPadX-10, y=bottomTriggerY, w=80, h=25, color=grey, strokeColor=darkGrey, strokeWidth=2})

touchPad = director:createNode({x=appWidth-120, y=appHeight-60})
touchPad.xScale=0.8
touchPad.yScale=0.8

touchPad.pad = director:createRectangle({xAnchor=0.5, yAnchor=0.5, x=0, y=0, w=80, h=80, color=grey, strokeColor=darkGrey, strokeWidth=2})
touchPad:addChild(touchPad.pad)

touchPad:addChild(director:createRectangle({xAnchor=0.5, yAnchor=0.5, x=0, y=-70, w=touchPad.pad.w+10, h=touchPad.pad.h+150, color=darkerGrey, strokeWidth=0, zOrder=-1}))

touchPad.point = director:createCircle({xAnchor=0.5, yAnchor=0.5, x=0, y=0, radius=touchPad.pad.w/8, color={100,100,100}, strokeWidth=0, zOrder=1, isVisible=false})
touchPad:addChild(touchPad.point)

touchPad:addChild(director:createCircle({xAnchor=0.5, yAnchor=0.5, radius=13,
            x=-touchPad.pad.w/4, y=-touchPad.pad.h/2-20, color=darkishGrey, strokeColor=darkGrey, strokeWidth=1}))
touchPad:addChild(director:createCircle({xAnchor=0.5, yAnchor=0.5, radius=13,
            x=touchPad.pad.w/4, y=-touchPad.pad.h/2-20, color=darkishGrey, strokeColor=darkGrey, strokeWidth=1}))
touchPad:addChild(director:createCircle({xAnchor=0.5, yAnchor=0.5, radius=13,
            x=-touchPad.pad.w/4, y=-touchPad.pad.h/2-55, color=darkishGrey, strokeColor=darkGrey, strokeWidth=1}))

touchPad:addChild(director:createCircle({xAnchor=0.5, yAnchor=0.5, radius=13,
            x=touchPad.pad.w/4, y=-touchPad.pad.h/2-55, color=darkishGrey, strokeColor=darkGrey, strokeWidth=1}))
touchPad:addChild(director:createRectangle({xAnchor=0, yAnchor=0, h=35, w=28,
            x=touchPad.pad.w/4-14, y=-touchPad.pad.h/2-90, color=darkGrey, strokeWidth=0, zOrder=1}))
touchPad:addChild(director:createRectangle({xAnchor=0, yAnchor=0, h=35, w=26,
            x=touchPad.pad.w/4-13, y=-touchPad.pad.h/2-90, color=darkishGrey, strokeWidth=0, zOrder=2}))
touchPad:addChild(director:createCircle({xAnchor=0.5, yAnchor=0.5, radius=13,
            x=touchPad.pad.w/4, y=-touchPad.pad.h/2-90, color=darkishGrey, strokeColor=darkGrey, strokeWidth=1}))

touchPad.playPause = director:createCircle({xAnchor=0.5, yAnchor=0.5, radius=15,
            x=-touchPad.pad.w/4, y=-touchPad.pad.h/2-90, color=grey, strokeColor=darkGrey, strokeWidth=1})
touchPad:addChild(touchPad.playPause)


--TODO: register key event and display normal keys when pressed/released

orientation()
