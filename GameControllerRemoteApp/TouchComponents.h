
#ifndef TOUCH_COMPONENTS_H
#define TOUCH_COMPONENTS_H

#include "s3eTypes.h"
#include "IwDebug.h"
#include "Iw2DSceneGraph.h"
#include "IwGx.h"
#include "IwTween.h"
#include <vector>

using namespace IwTween;
using namespace Iw2DSceneGraphCore;
using namespace Iw2DSceneGraph;

CLabel* SetUpLabel(CNode* parent, int x, int y, int width, int height, CIw2DFont* font, const char* text, CColor color, CIw2DFontAlign alignHor);

typedef struct TouchState
{
    int x;
    int y;
    int w;
    int h;
    float touchX;
    float touchY;
    bool pressed;
    unsigned int touchID;
} TouchState;

class CComponent
{
protected:
    CNode* m_parent;
    CSprite* m_sprite;
    CIw2DImage* m_img;

public:
    CComponent(const char* texture, CNode* parent, int x, int y, int w, int h, bool bestFit = false);
    ~CComponent();
    void SetAlpha(float alpha);
    float GetX() { return m_sprite->m_X; }
    float GetY() { return m_sprite->m_Y; }
    float GetW() { return m_sprite->m_W; }
    float GetH() { return m_sprite->m_H; }
    float GetImageW() { return m_img->GetWidth(); }
    float GetImageH() { return m_img->GetHeight(); }
    void SetX(float x) { m_sprite->m_X = x; }
    void SetY(float y) { m_sprite->m_Y = y; }
    void SetW(float w) { m_sprite->m_W = w; }
    void SetH(float h) { m_sprite->m_H = h; }
    CSprite* GetSprite() { return m_sprite; }
    void Show() { m_sprite->m_IsVisible = true; }
    void Hide() { m_sprite->m_IsVisible = false; }

    static void SetSpriteImage(CSprite * sprite, CIw2DImage* image);
};

class CTouchComponent : public CComponent
{
protected:
    bool m_trackPosition;
    CIw2DImage* m_pressedImg;
    CSprite* m_highlightSprite;
    CIw2DImage* m_highlightImg;
    s3eCallback m_releasedCallback;
    void* m_callbackData;
    bool m_enabled;

public:
    TouchState m_touchArea;
    
    CTouchComponent(const char* texture, CNode* parent, int x, int y, int w, int h, bool trackPosition = false,
                    const char* pressedTexture = NULL, bool noPress = false, const char* touchHighlight = NULL, int highlightW = 0, int highlightH = 0);
    ~CTouchComponent();
    bool TracksPosition();
    bool CanAnimateRelease();
    void SetReleased();
    void SetPressed();
    void SetHighlightPos(int x, int y);
    void SetHighlightVisibility(bool visible);
    void ReleaseEvent();
    void SetX(float x) { m_sprite->m_X = x; m_touchArea.x = x; }
    void SetY(float y) { m_sprite->m_Y = y; m_touchArea.y = y; }
    void Enable();
    void Disable();
    bool IsEnabled();
};

struct TouchComponents
{
    std::vector<CTouchComponent*> components;
    bool useMultiTouch;
};

int32 TouchEvent(void* systemData, void* userData);

int32 MotionEvent(void* systemData, void* userData);

class CButtonComponent : public CTouchComponent
{
private:
    CSprite* m_iconSprite;
    CIw2DImage* m_iconImg;
    CLabel* m_label;

public:

    CButtonComponent(const char* buttonTexture, CNode* parent, int x, int y, int w, int h,
        const char* pressedTexture, const char* text, CIw2DFont* font, s3eCallback releasedCallback, void* callbackData,
        const char* iconTexture = NULL, float iconHeightScale = 1.0);
    ~CButtonComponent();

    void SetText(const char* text);
};

#endif //TOUCH_COMPONENTS_H