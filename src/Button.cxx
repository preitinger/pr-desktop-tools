#include "Button.H"

#include <iostream>

Button::Button(int X, int Y, int W, int H, const char *L)
    : Fl_Button(X, Y, W, H, L),
      m_access()
{
}

SButtonAccessSP Button::access()
{
    auto sp = SButtonAccess::create(this);
    m_access = sp;
    return sp;
}

Button::~Button()
{
    auto p = m_access.lock();
    if (p)
    {
        p->destroyingButton();
    }
}
