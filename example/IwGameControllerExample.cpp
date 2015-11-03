#include "s3e.h"
#include "IwDebug.h"
#include "IwGameController.h"
#include "IwGameController_Any.h"
#include <string.h>

using namespace IwGameController;

//Based on s3eAndroidController and s3eKeyboard examples.
//We're displaying keyboard keys as well as controller for comparison.

// Last 5 controller button presses
#define NUM_EVENTS_TO_SHOW 5
static int g_ButtonsPresesed[NUM_EVENTS_TO_SHOW];
static int g_NumButtons = 0;
static int g_ButtonsReleased[NUM_EVENTS_TO_SHOW];
static int g_NumButtonsReleased = 0;

// Last 5 generic key presses
static s3eKey g_Keys[NUM_EVENTS_TO_SHOW];
static int g_NumKeys = 0;
static s3eKey g_KeysReleased[NUM_EVENTS_TO_SHOW];
static int g_NumKeysReleased = 0;


static int32 controllerHandler(void* sys, void*)
{
    /*
    TODO
    s3eAndroidControllerButtonEvent* event = (s3eAndroidControllerButtonEvent*)sys;

    if (event->m_Pressed) // a key state changed from up to down
    {
        if (g_NumButtons < NUM_EVENTS_TO_SHOW)
            g_NumButtons++;

        // Move previous entries down through the array and add new one at end
        memmove(g_ButtonsPresesed+1, g_ButtonsPresesed, (NUM_EVENTS_TO_SHOW - 1) * sizeof(int));
        g_ButtonsPresesed[0] = event->m_Button;
    }
    else // state changed from down to up
    {
        if (g_NumButtonsReleased < NUM_EVENTS_TO_SHOW)
            g_NumButtonsReleased++;

        memmove(g_ButtonsReleased+1, g_ButtonsReleased, (NUM_EVENTS_TO_SHOW - 1) * sizeof(int));
        g_ButtonsReleased[0] = event->m_Button;
    }
    */
    return 0;
}

static int32 keyHandler(void* sys, void*)
{
    s3eKeyboardEvent* event = (s3eKeyboardEvent*)sys;

    if (event->m_Pressed) // a key state changed from up to down
    {
        if (g_NumKeys < NUM_EVENTS_TO_SHOW)
            g_NumKeys++;

        // Move previous entries down through the array and add new one at end
        memmove(g_Keys+1, g_Keys, (NUM_EVENTS_TO_SHOW - 1) * sizeof(s3eKey));
        g_Keys[0] = event->m_Key;
    }
    else // state changed from down to up
    {
        if (g_NumKeysReleased < NUM_EVENTS_TO_SHOW)
            g_NumKeysReleased++;

        memmove(g_KeysReleased+1, g_KeysReleased, (NUM_EVENTS_TO_SHOW - 1) * sizeof(s3eKey));
        g_KeysReleased[0] = event->m_Key;
    }

    return 0;
}

int main()
{
	// Scale font up to be easier to read
	int scale;
	if (s3eSurfaceGetInt(S3E_SURFACE_WIDTH) < 320 || s3eSurfaceGetInt(S3E_SURFACE_HEIGHT) < 320)
        scale = 1;
    else if (s3eSurfaceGetInt(S3E_SURFACE_WIDTH) < 480 || s3eSurfaceGetInt(S3E_SURFACE_HEIGHT) < 480)
        scale = 2;
    else
        scale = 3;

    int fontScale = scale > 1 ? scale-1 : 1;
    s3eDebugSetInt(S3E_DEBUG_FONT_SCALE, fontScale);

    //TODO: use IwGameController equivalents here
	//s3eAndroidControllerRegister(S3E_ANDROIDCONTROLLER_CALLBACK_BUTTON, controllerHandler,  NULL);
    s3eKeyboardRegister(S3E_KEYBOARD_KEY_EVENT, keyHandler, NULL);

	CIwGameController* controller = IwGameController::Create();
	CIwGameControllerHandle* controllerHandle = NULL;
    
	
	// Use to disable s3eKeyboard events
	//if (controller)
	//    controller->SetPropagateButtonsToKeyboard(false);

    while (!s3eDeviceCheckQuitRequest())
    {
        s3eKeyboardUpdate();
        s3ePointerUpdate();

	    s3eSurfaceClear(0,0,0);

		int fontHeight = s3eDebugGetInt(S3E_DEBUG_FONT_HEIGHT);
		int fontWidth = s3eDebugGetInt(S3E_DEBUG_FONT_WIDTH);
		int lineHeight = fontHeight + 2;
		int x = 20;
		int y = 20;
		char name[128];

		if (!controller)
		{
			s3eDebugPrintf(x, y, 1, "No controller extension available :(");
			y += lineHeight;
		}
		else
		{
			s3eDebugPrint(x, y, "Please mash controller number 1 (and 2)!", 1);
			y += lineHeight;

			controller->StartFrame();

			int numControllers = controller->GetControllerCount();

			s3eDebugPrintf(x, y, 1, "Controllers found: %d", numControllers);
			y += lineHeight;
            
            int yStart = y;
            int xStart = x;

            // Realistically you wouldn't do this controller discovery on every loop!
			int n = 0;
			while (n < numControllers && n < 2) //just two will fit on most screens..
			{
                y = yStart;
                
				controllerHandle = controller->GetControllerByIndex(n);
                
                if (controllerHandle)
                {
                    s3eDebugPrintf(x, y, 1, "Using controller at index: %d", n);
                    
                    controller->SetProperty(controllerHandle, IwGameController::Property::REPORTS_ABSOLUTE_DPAD_VALUES, true);
                }
                else
                    s3eDebugPrintf(x, y, 1, "Could not get a controller to use :(");
                
                y += lineHeight*2;
                
                // Display state of each axis
                s3eDebugPrint(x, y, "Axis positions:", 0);
                x += 20;
                y += lineHeight;
                for (int i = 0; i < IwGameController::Axis::MAX; i++)
                {
                    Axis::eAxis axisId = (Axis::eAxis)i;
                    
                    if (!CIwGameController::GetAxisDisplayName(name, axisId, true))
                        strcpy(name, "error");
                    
                    s3eDebugPrintf(x, y, 1, "Axis: %s (%d) = %f", name, i, controller->GetAxisValue(controllerHandle, axisId));
                    
                    y += lineHeight;
                }
                
                y += lineHeight;
                x = xStart;
                
                // Display state of each controller button
                s3eDebugPrint(x, y, "Controller buttons pressed:", 0);
                x += 20;
                y += lineHeight;
                for (int i = 0; i < IwGameController::Button::MAX; i++)
                {
                    Button::eButton buttonId = (Button::eButton)i;
                    
                    if (!CIwGameController::GetButtonDisplayName(name, buttonId, true))
                        strcpy(name, "error");
                    
                    s3eDebugPrintf(x, y, 1, "Button: %s (%d) is %s", name, i, controller->GetButtonState(controllerHandle, buttonId) ? "down" : "up");
                    
                    y += lineHeight;
                }
                
                n++;
                x += 500;
                xStart = x;
            }
		}

        int listStartY;
        //int maxY;
        
		// ----------------- Controller button states from event/callbacks not polling - not yet supported in API ---------------------
        
        /*
        y += lineHeight;
        listStartY = y;
        x = 20;

		// Display last few Buttons that were pressed down
		s3eDebugPrint(x, y, "Buttons pressed:", 0);
		x += 20;
		y += lineHeight;
		for (int j = g_NumButtons-1; j >= 0; j--)
		{
			int button = g_ButtonsPresesed[j];
			IwGameController::GetButtonDisplayName(name, button);
			s3eDebugPrintf(x, y, 1, "Button: %s (%d)", name, button);

			y += lineHeight;
		}
        y += lineHeight;
		
        maxY = y;

		y = listStartY;
		x = s3eSurfaceGetInt(S3E_SURFACE_WIDTH)/2 + 20;

		// Display last few Buttons that were released
		s3eDebugPrint(x, y, "Buttons released:", 0);
		x += 20;
		y += lineHeight;
		for (int k = g_NumButtonsReleased-1; k >= 0; k--)
		{
			int button = g_ButtonsReleased[k];
			IwGameController::GetButtonDisplayName(name, button);

			s3eDebugPrintf(x, y, 1, "Button: %s (%d)", name, button);

			y += lineHeight;
		}
        y += lineHeight;
		
        if (y > maxY) maxY = y;
        

		// ----------------- Normal keys states for comparison ---------------------

		y = maxY;
        */

        x = 20;
        y += lineHeight;
		listStartY = y;

		// Display last few keys that were pressed down
		s3eDebugPrint(x, y, "Keys pressed:", 0);
		x += 20;
		y += lineHeight;
		for (int j = g_NumKeys-1; j >= 0; j--)
		{
			s3eKey key = g_Keys[j];
			s3eKeyboardGetDisplayName(name, key);

			if (key >= s3eKeyAbsGameA && key <= s3eKeyAbsBSK)
				s3eDebugPrintf(x, y, 1, "Key: %s (%d - abstract %d)", name, key, key - s3eKeyAbsGameA);
			else
				s3eDebugPrintf(x, y, 1, "Key: %s (%d)", name, key);

			y += lineHeight;
		}

		y = listStartY;
		x = s3eSurfaceGetInt(S3E_SURFACE_WIDTH)/2 + 20;

		// Display last few keys that were released
		s3eDebugPrint(x, y, "Keys released:", 0);
		x += 20;
		y += lineHeight;
		for (int k = g_NumKeysReleased-1; k >= 0; k--)
		{
			s3eKey key = g_KeysReleased[k];
			s3eKeyboardGetDisplayName(name, key);

			if (key >= s3eKeyAbsGameA && key <= s3eKeyAbsBSK)
				s3eDebugPrintf(x, y, 1, "Key: %s (%d - abstract %d)", name, key, key - s3eKeyAbsGameA);
			else
				s3eDebugPrintf(x, y, 1, "Key: %s (%d)", name, key);

			y += lineHeight;
		}

		s3eSurfaceShow();

        // Sleep for 0ms to allow the OS to process events etc.
        s3eDeviceYield(0);
    }

    return 0;
}
