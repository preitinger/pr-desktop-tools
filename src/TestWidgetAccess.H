#ifndef __TEST_WIDGET_ACCESS_H__
#define __TEST_WIDGET_ACCESS_H__

#include "OnFinish.H"
#include "WidgetAccess.H"
#include "Callback.H"

#include <FL/Fl_Output.H>

class Fl_Window;
class Fl_Button;

class TestWidgetAccess /* : public CallbackOwner<TestWidgetAccess> */
{
public:
    TestWidgetAccess(Fl_Window *window, const OnFinish &onFinish);
    virtual ~TestWidgetAccess();

private:
    void onExit();
    void onSetRandomOutput();
    std::shared_ptr<std::function<void()>> createAction(void (TestWidgetAccess::*memberFunc)(void))
    {
        return std::make_shared<std::function<void()>>(std::bind(memberFunc, this));
    }

private:
    Fl_Window *window;
    OnFinish onFinish;
    WidgetAccessSP<Fl_Output> output;
    WidgetAccessSP<Fl_Button> exit;
    WidgetAccessSP<Fl_Button> deleteOutput;
    using ActionSP = std::shared_ptr<std::function<void()>>;
    ActionSP exitAction, deleteAction;
    CallbackSP<Fl_Button> exitCb, deleteCb;
    WidgetAccessSP<Fl_Button> setRandomOutput;
    ActionSP setRandomOutputAction;
    CallbackUP<Fl_Button> setRandomOutputCb;
};

#endif // __TEST_WIDGET_ACCESS_H__
