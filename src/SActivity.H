#ifndef __SACTIVITY_H__
#define __SACTIVITY_H__

#include "SGenericAccess.H"
#include "Macros.H"
#include <FL/Fl_Window.H>
#include <X11/Xlib.h>

#include <memory>

typedef std::function<void()> OnFinish;

SCLASS(SActivity)
{
public:
    SActivity(const GenericAccessSP<Fl_Window> &window, Display *display, const OnFinish &onFinish);
    virtual ~SActivity();

public:
    virtual void start() = 0;

protected:
    GenericAccessSP<Fl_Window> window;
    Display *display;
    OnFinish onFinish;
};

#endif
