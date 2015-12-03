
#include "TouchComponents.h"
#include "s3ePointer.h"

CLabel* SetUpLabel(CNode* parent, int x, int y, int width, int height, CIw2DFont* font, const char* text, CColor color, CIw2DFontAlign alignHor)
{
    CLabel* lbl = new CLabel();
    lbl->m_X = x;
    lbl->m_Y = y;
    lbl->m_W = width > 0 ? width : (float)Iw2DGetSurfaceWidth() * 0.95;
    lbl->m_H = height > 0 ? height : (float)Iw2DGetSurfaceHeight() * 0.95;
    lbl->m_Text = text;
    lbl->m_AlignHor = alignHor;
    lbl->m_AlignVer = IW_2D_FONT_ALIGN_CENTRE;
    lbl->m_Font = font;
    lbl->m_Color = color;
    lbl->m_ScaleX = 1.0;
    lbl->m_ScaleY = 1.0;
    parent->AddChild(lbl);
    return lbl;
}

//---------------------------------------------------------------------------

CComponent::CComponent(const char* texture, CNode* parent, int x, int y, int w, int h, bool bestFit)
{
    m_parent = parent;
    m_sprite = new CSprite();
    m_img = Iw2DCreateImage(texture);
    m_sprite->SetImage(m_img);

    if (bestFit)
    {
        float scaler = w / m_sprite->m_W;
        if (h / scaler > m_sprite->m_H)
        {
            int oldW = w;
            w = m_sprite->m_W / (m_sprite->m_H / h);
            x = (oldW - w) / 2.0;
        }
        else
        {
            int oldH = h;
            h = m_sprite->m_H / (m_sprite->m_W / w);
            y = (oldH - h) / 2.0;
        }
    }
    else
    {
        if (w == 0)
        {
            w = h / m_sprite->m_H * m_sprite->m_W;
        }
        else if (h == 0)
        {
            h = w / m_sprite->m_W * m_sprite->m_H;
        }
    }

    m_sprite->m_X = x;
    m_sprite->m_Y = y;
    m_sprite->m_W = w;
    m_sprite->m_H = h;
    parent->AddChild(m_sprite);
}

CComponent::~CComponent()
{
    if (m_sprite)
    {
        if (m_parent->IsChild(m_sprite))
            m_parent->RemoveChild(m_sprite);
        delete m_sprite;
    }
    if (m_img)
        delete m_img;
}
    
void CComponent::SetAlpha(float alpha)
{
    if (m_sprite)
        m_sprite->m_Alpha = alpha;
}

// Annoyingly, CSprite will resize itself to the image size when you set it normally!
void CComponent::SetSpriteImage(CSprite * sprite, CIw2DImage* image)
{
    int w = sprite->m_W;
    int h = sprite->m_H;
    sprite->SetImage(image);
    sprite->m_W = w;
    sprite->m_H = h;
}

//---------------------------------------------------------------------------

CTouchComponent::CTouchComponent(const char* texture, CNode* parent, int x, int y, int w, int h, bool trackPosition,
                                 const char* pressedTexture, bool noPress, const char* touchHighlight, int highlightW, int highlightH)
                                    : CComponent(texture, parent, x, y, w, h)
{
    // Pick up values that may have changed in paraent constructor
    h = m_sprite->m_H;
    w = m_sprite->m_W;

    m_releasedCallback = NULL;
    m_callbackData = NULL;
    m_enabled = true;

    if (noPress)
    {
        m_pressedImg = NULL;
    }
    else
    {
        if (pressedTexture)
            m_pressedImg = Iw2DCreateImage(pressedTexture);
        else
        {
            char* newTex = new char[strlen(texture) + 5 + 1];
            strcpy(newTex, texture);
            sprintf(newTex + strlen(texture) - 4, "_down%s", texture + strlen(texture) - 4);
            m_pressedImg = Iw2DCreateImage(newTex);
            delete newTex;
        }
    }

    if (touchHighlight)
    {
        m_highlightSprite = new CSprite();
        m_highlightImg = Iw2DCreateImage(touchHighlight);
        m_highlightSprite->SetImage(m_highlightImg);

        if (highlightW == 0)
            highlightW = w / 3;
        if (highlightH == 0)
            highlightH = h / 3;

        m_highlightSprite->m_X = x;
        m_highlightSprite->m_Y = y;
        m_highlightSprite->m_W = highlightW;
        m_highlightSprite->m_H = highlightH;
        m_highlightSprite->m_IsVisible = false;
        m_parent->AddChild(m_highlightSprite);
    }
    else
    {
        m_highlightSprite = NULL;
        m_highlightImg = NULL;
    }

    m_touchArea.pressed = false;
    m_touchArea.touchID = -1;
    m_touchArea.touchX = 1.0f;
    m_touchArea.touchY = 1.0f;

    m_touchArea.x = x;
    m_touchArea.y = y;
    m_touchArea.w = w;
    m_touchArea.h = h;
    
    m_trackPosition = trackPosition;
}
    
CTouchComponent::~CTouchComponent()
{
    if (m_highlightSprite)
    {
        if (m_parent->IsChild(m_highlightSprite))
            m_parent->RemoveChild(m_highlightSprite);
        delete m_highlightSprite;
    }
    if (m_highlightImg)
        delete m_highlightImg;
}

bool CTouchComponent::TracksPosition()
{
    return m_trackPosition;
}

bool CTouchComponent::CanAnimateRelease()
{
    return m_pressedImg != NULL;
}

void CTouchComponent::SetReleased()
{
    CComponent::SetSpriteImage(m_sprite, m_img);
}

void CTouchComponent::SetPressed()
{
    if (m_pressedImg)
        CComponent::SetSpriteImage(m_sprite, m_pressedImg);
}

void CTouchComponent::SetHighlightPos(int x, int y)
{
    if (!m_highlightSprite)
        return;

    m_highlightSprite->m_X = x - m_highlightSprite->m_W/2;
    m_highlightSprite->m_Y = y - m_highlightSprite->m_W / 2;
}

void CTouchComponent::SetHighlightVisibility(bool visible)
{
    if (m_highlightSprite)
        m_highlightSprite->m_IsVisible = visible;
}

void CTouchComponent::ReleaseEvent()
{
    if (m_releasedCallback)
        m_releasedCallback(m_callbackData, NULL);
}

void CTouchComponent::Enable()
{
    m_enabled = true;
}
void CTouchComponent::Disable()
{
    m_enabled = false;
}

bool CTouchComponent::IsEnabled()
{
    return m_enabled;
}

//---------------------------------------------------------------------------

int32 TouchEvent(void* systemData, void* userData)
{
    s3ePointerTouchEvent* event;
    TouchComponents* touchComponents = (TouchComponents*)userData;
    
    if (touchComponents->useMultiTouch)
        event = (s3ePointerTouchEvent*)systemData;
    else
    {
        s3ePointerEvent* singleTouchEvent = (s3ePointerEvent*)systemData;

        if (singleTouchEvent->m_Button != S3E_POINTER_BUTTON_SELECT)
            return 0;

        event = new s3ePointerTouchEvent;
        event->m_x = singleTouchEvent->m_x;
        event->m_y = singleTouchEvent->m_y;
        event->m_TouchID = 0;
        event->m_Pressed = singleTouchEvent->m_Pressed;
    }

    //todo replace with vector iterator
    for (std::vector<CTouchComponent*>::const_iterator it = touchComponents->components.begin(); it != touchComponents->components.end(); ++it)
    {
        //CTouchComponent* touchComp = touchComponents->components[i];
        CTouchComponent* touchComp = *it;

        if (!touchComp->GetSprite()->m_IsVisible || !touchComp->IsEnabled())
            continue;

        TouchState* touchArea = &touchComp->m_touchArea;

        bool released = touchArea->touchID == event->m_TouchID && touchArea->pressed && !event->m_Pressed;

        if (event->m_x > touchArea->x && event->m_x < touchArea->x + touchArea->w &&
            event->m_y > touchArea->y && event->m_y < touchArea->y + touchArea->h)
        {
            if (!touchArea->pressed && event->m_Pressed)
            {
                touchArea->pressed = true;
                touchArea->touchID = event->m_TouchID;
                touchComp->SetPressed();

                if (touchComp->TracksPosition())
                {
                    touchArea->touchX = (float)(event->m_x - touchArea->x) / (float)touchArea->w * 2.0; // scale to be 0 -> 2 (instead of 1 -> -1 as the minus adds a digit)
                    touchArea->touchY = (float)(event->m_y - touchArea->y) / (float)touchArea->h * 2.0;

                    touchComp->SetHighlightPos(event->m_x, event->m_y);
                    touchComp->SetHighlightVisibility(true);
                }
            }
            else if (released)
            {
                if (touchComp->TracksPosition())
                {
                    touchArea->touchX = (float)(event->m_x - touchArea->x) / (float)touchArea->w * 2.0;
                    touchArea->touchY = (float)(event->m_y - touchArea->y) / (float)touchArea->h * 2.0;
                    //Leaving the positions at last set after release. A game would typically ignore them
                    //Could reset to zero in next update loop (in StarFrame in IwGameController).
                    touchComp->SetHighlightPos(event->m_x, event->m_y);
                }

                touchComp->ReleaseEvent();
            }
        }

        if (released)
        {
            touchArea->pressed = false;
            touchArea->touchID = -1;
            touchComp->SetReleased();
            touchComp->SetHighlightVisibility(false);
        }
    }

    if (!touchComponents->useMultiTouch)
        delete event;

    return 0;
}

int32 MotionEvent(void* systemData, void* userData)
{
    s3ePointerTouchMotionEvent* event;
    TouchComponents* touchComponents = (TouchComponents*)userData;

    if (touchComponents->useMultiTouch)
        event = (s3ePointerTouchMotionEvent*)systemData;
    else
    {
        s3ePointerMotionEvent* singleTouchEvent = (s3ePointerMotionEvent*)systemData;
        event = new s3ePointerTouchMotionEvent;
        event->m_x = singleTouchEvent->m_x;
        event->m_y = singleTouchEvent->m_y;
        event->m_TouchID = 0;
    }

    for (std::vector<CTouchComponent*>::const_iterator it = touchComponents->components.begin(); it != touchComponents->components.end(); ++it)
    //for (int i = 0; i < touchComponents->count; ++i)
    {
        //CTouchComponent* touchComp = touchComponents->components[i];
        CTouchComponent* touchComp = *it;

        if (!touchComp->IsEnabled() || (!touchComp->TracksPosition() && !touchComp->CanAnimateRelease()))
            continue;
        
        TouchState* touchArea = &touchComp->m_touchArea;

        if (touchArea->pressed && touchArea->touchID == event->m_TouchID)
        {
            if (event->m_x > touchArea->x && event->m_x < touchArea->x + touchArea->w &&
                event->m_y > touchArea->y && event->m_y < touchArea->y + touchArea->h)
            {
                if (touchComp->TracksPosition())
                {
                    touchArea->touchX = (float)(event->m_x - touchArea->x) / (float)touchArea->w * 2.0;
                    touchArea->touchY = (float)(event->m_y - touchArea->y) / (float)touchArea->h * 2.0;

                    touchComp->SetHighlightPos(event->m_x, event->m_y);
                }
                touchComp->SetPressed();
            }
            else
                touchComp->SetReleased();
        }
    }
    
    if (!touchComponents->useMultiTouch)
        delete event;

    return 0;
}

// -------------------------------------------------------------------

CButtonComponent::CButtonComponent(const char* buttonTexture, CNode* parent, int x, int y, int w, int h,
    const char* pressedTexture, const char* text, CIw2DFont* font, s3eCallback releasedCallback, void* callbackData,
    const char* iconTexture, float iconHeightScale)
    : CTouchComponent(buttonTexture, parent, x, y, w, h, false, pressedTexture)
{
    m_releasedCallback = releasedCallback;
    m_callbackData = callbackData;

    // Pick up values that may have changed in parent constructor
    h = m_sprite->m_H;
    w = m_sprite->m_W;

    if (iconTexture)
    {
        m_iconSprite = new CSprite();
        m_iconImg = Iw2DCreateImage(iconTexture);
        m_iconSprite->SetImage(m_iconImg);
        float imgH = m_iconSprite->m_H;
        m_iconSprite->m_H = m_sprite->m_H * iconHeightScale;
        m_iconSprite->m_W = m_iconSprite->m_H / imgH * m_iconSprite->m_W;
        m_iconSprite->m_X = m_iconSprite->m_Y = h * (1.0 - iconHeightScale) / 2.0;
        m_iconSprite->m_X += h*0.1; // looks better padded a little 
        m_sprite->AddChild(m_iconSprite);
    }
    else
    {
        m_iconSprite = NULL;
        m_iconImg = NULL;
    }

    if (text)
    {
        int xLbl;
        if (iconTexture)
            xLbl = m_iconSprite->m_X + m_iconSprite->m_W; //assume icon has some padding so dont pad
        else
            xLbl = h*0.1;
        
        m_label = SetUpLabel(m_sprite, xLbl, h*0.1, w - xLbl - h*0.1, h*0.8, font, text, CColor(0xff, 0xff, 0xff, 0xff), IW_2D_FONT_ALIGN_CENTRE);
    }
}

CButtonComponent::~CButtonComponent()
{
    if (m_iconSprite)
    {
        if (m_sprite->IsChild(m_iconSprite))
            m_sprite->RemoveChild(m_iconSprite);
        delete m_iconSprite;
    }

    if (m_iconImg)
        delete m_iconImg;
}

void CButtonComponent::SetText(const char* text)
{
    m_label->m_Text = text;
}
