#ifndef __ACTIVITY_BUTTON_H__
#define __ACTIVITY_BUTTON_H__

#include "SActivity.H"
#include "Button.H"

#include <FL/Fl_Window.H>


class ActivityButton : public Button
{
private:
    SActivityWP activity;

public:
    ActivityButton(int x, int y, const char* label);
    void setActivity(const SActivitySP& activity);
    virtual ~ActivityButton();

private:
    void onClick();
};

#endif
