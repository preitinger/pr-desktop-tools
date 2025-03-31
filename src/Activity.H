#ifndef __ACTIVITY_H__
#define __ACTIVITY_H__

#include <FL/Fl_Window.H>
#include <X11/Xlib.h>

#include <functional>

typedef std::function<void()> OnFinish;

class Activity
{
protected:
    Fl_Window* window;
    Display* display;
    OnFinish onFinish;

public:
    Activity(Fl_Window *window, Display *display, const OnFinish& onFinish);
    virtual void start() = 0;
};

#endif