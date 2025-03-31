#ifndef __BUTTON_H__
#define __BUTTON_H__

#include <FL/Fl_Button.H>

#include "SButtonAccess.H"

#include <functional>

typedef std::shared_ptr<SButtonAccess> SButtonAccessSP;

class Button : public Fl_Button
{
public:
    Button(int X, int Y, int W, int H, const char *L);
    virtual ~Button();

    SButtonAccessSP access();

private:
    SButtonAccessWP m_access;
};

#endif
