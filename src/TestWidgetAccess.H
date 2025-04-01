#ifndef __TEST_WIDGET_ACCESS_H__
#define __TEST_WIDGET_ACCESS_H__

#include "OnFinish.H"
#include "WidgetAccess.H"

#include <FL/Fl_Output.H>

class Fl_Window;
class Fl_Button;

class TestWidgetAccess
{
public:
    TestWidgetAccess(Fl_Window *window, const OnFinish &onFinish);
    virtual ~TestWidgetAccess();

    private:
    void onExit();

private:
    Fl_Window *window;
    OnFinish onFinish;
    WidgetAccessSP<Fl_Output> output;
    WidgetAccessSP<Fl_Button> exit;
    WidgetAccessSP<Fl_Button> deleteOutput;
};

#endif // __TEST_WIDGET_ACCESS_H__
