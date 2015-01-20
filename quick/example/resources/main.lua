
--require("mobdebug").start() -- Uncomment for ZeroBrain IDE debuger support

--We're using these visual pads and buttons to show real pad and button states!
dofile("helpers/OnScreenDPad.lua")
dofile("helpers/OnScreenButton.lua")
dofile("helpers/VirtualResolution.lua")

appWidth = 640
appHeight = 480
virtualResolution:initialise{userSpaceW=appWidth, userSpaceH=appHeight}
screenWidth = virtualResolution:winToUserSize(director.displayWidth)
screenHeight = virtualResolution:winToUserSize(director.displayHeight)

propagateKeys = true
numControllers = 0
playerSelected = "none"

fontScale = 0.7

-- Update event to do actual controller state handling ------------------------

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
    
    -- NB, flipping Y axis as opposite to Quick's axis!
    local lx = gameController.getAxisValue(gameController.axisStickLeftX)
    local ly = -gameController.getAxisValue(gameController.axisStickLeftY)
    local rx = gameController.getAxisValue(gameController.axisStickRightX)
    local ry = -gameController.getAxisValue(gameController.axisStickRightY)
    leftPad:setTopAsFraction(lx,ly)
    rightPad:setTopAsFraction(rx,ry)
    
    lblStickDbgL.text = string.format("Left Stick: (%.5f,%.5f)", lx, ly)
    lblStickDbgR.text = string.format("Right Stick: (%.5f,%.5f)", rx, ry)
    
    btnA:setState(gameController.getButtonState(gameController.buttonA))
    btnB:setState(gameController.getButtonState(gameController.buttonB))
    btnX:setState(gameController.getButtonState(gameController.buttonX))
    btnY:setState(gameController.getButtonState(gameController.buttonY))
    
    btnUp:setState(gameController.getButtonState(gameController.buttonDPadUp))
    btnDown:setState(gameController.getButtonState(gameController.buttonDPadDown))
    btnLeft:setState(gameController.getButtonState(gameController.buttonDPadLeft))
    btnRight:setState(gameController.getButtonState(gameController.buttonDPadRight))
    
    if gameController.getButtonState(gameController.buttonStickLeft) then
        leftPad.top.color = color.white
    else
        leftPad.top.color = leftPad.topColor
    end
    
    if gameController.getButtonState(gameController.buttonStickRight) then
        rightPad.top.color = color.white
    else
        rightPad.top.color = rightPad.topColor
    end
    
    triggerState(btnShoulderLeft, gameController.getButtonState(gameController.buttonShoulderLeft))
    triggerState(btnShoulderRight, gameController.getButtonState(gameController.buttonShoulderRight))

    triggerState(triggerLeft, gameController.getAxisValue(gameController.axisTriggerLeft))
    triggerState(triggerRight, gameController.getAxisValue(gameController.axisTriggerRight))
end


-- Periodic check for new/lost controllers ------------------------------------

function checkControllers(event)
    numControllers = gameController.getPlayerCount()

    local gotController = false
    local n = 0
    repeat
        n = n+1
        gotController = gameController.selectControllerByPlayer(n)
    until gotController or n == gameController.maxPlayers
        
    if n == 0 then
        playerSelected = "none"
    else
        playerSelected = n
    end
end


-- Simple buttons for key propagation -----------------------------------------
--TODO: not showing keys yet plus need non-touch way to press buttons!
--This bit is a bit pointless atm...
btnCount = 0
btnX=appWidth-20
btnW=appWidth/5
btnH=appHeight/16

function touchKeyEventsOn(event)
    propagateKeys = not propagateKeys
    gameController.setPropagateButtonsToKeyboard(true)
end
local function touchKeyEventsOff(event)
    gameController.setPropagateButtonsToKenablyboard(false)
end

function addButton(text, touchEvent)
    btnCount = btnCount + 1
    local button = director:createRectangle({x=btnX, y=appHeight - (btnH+10)*btnCount, w=btnW, h=btnH, color=color.blue, xAnchor=1, yAnchor=0})
    button:addChild(director:createLabel({x=0, y=0, hAlignment="centre", vAlignment="centre", text=text, w=btnW/fontScale, h=btnH/fontScale, color=color.white, xScale=fontScale, yScale=fontScale}))
    button:addEventListener("touch", touchEvent)
end


-- Main rendering -------------------------------------------------------------

-- Simple example, just use default scene!
virtualResolution:applyToScene(director:getCurrentScene())

lblSupported = director:createLabel({x=10, y=appHeight-80, w=(appWidth-20)/fontScale, vAlignment="bottom", text="", color=color.white})

lblStickDbgL = director:createLabel({x=10, y=10, w=(appWidth/2)/fontScale, vAlignment="bottom", text="Left Stick: (0.00000,0.00000)", color=color.white, xScale=fontScale, yScale=fontScale})

lblStickDbgR = director:createLabel({x=appWidth/2, y=10, w=(appWidth/2)/fontScale, vAlignment="bottom", text="Right Stick: (0.00000,0.00000)", color=color.white, xScale=fontScale, yScale=fontScale})

if gameController.isAvailable() then
    gameController.init()
    lblSupported.text = "Controller API available"
    system:addEventListener({"update"}, update)
    checkControllers()
    system:addTimer(checkControllers, 5) --check for changes every 5 seconds
    
    lblKeyBtns = director:createLabel({x=10, y=appHeight-30, w=(appWidth-100)/fontScale, vAlignment="bottom", text="Controller generates key events? ->", color=color.white})
    
    --
    addButton("Enable", touchKeyEventsOn)
    addButton("Disable", touchKeyEventsOff)
else
    lblSupported.text = "Controller API NOT available!"
end

lblNumPlayers = director:createLabel({x=10, y=appHeight-110, w=(appWidth-20)/fontScale, vAlignment="bottom", text="Number of controllers found: " .. numControllers, color=color.white})

lblPlayerSelected = director:createLabel({x=10, y=appHeight-140, w=(appWidth-20)/fontScale, vAlignment="bottom", text="Controller selected for player: " .. playerSelected, color=color.white})

--pads and buttons to show controller state

topRowY = appHeight*0.23+50
bottomRowY = appHeight*0.23

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

btnLeft:pressButton()

--TODO: register key event and display normal keys when pressed/released


