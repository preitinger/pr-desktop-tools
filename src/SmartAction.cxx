#include "SmartAction.H"
#include "WidgetAccess.H"
#include "OnFinish.H"

#include <FL/Fl_Button.H>
#include <FL/fl_callback_macros.H>

#include <vector>
#include <functional>
#include <iostream>

class SmartActionImpl : public SmartAction
{
public:
    SmartActionImpl(const OnFinish &onFinish);
    virtual ~SmartActionImpl();

private:
    void onExit();
    void buttonWithCb(int X, int Y, int W, int H, const char *L, void (SmartActionImpl::*m)());

private:
    OnFinish onFinish;
    std::vector<std::function<void()>> tidyUpTasks;
};

SmartActionUP SmartAction::create(const OnFinish &onFinish)
{
    return std::unique_ptr<SmartActionImpl>(new SmartActionImpl(onFinish));
}

SmartActionImpl::SmartActionImpl(const OnFinish &onFinish)
    : onFinish(onFinish)
{
    buttonWithCb(0, 0, 40, 20, "Exit", &SmartActionImpl::onExit);
}

void SmartActionImpl::onExit()
{
    if (onFinish)
    {
        onFinish();
    }
}

SmartActionImpl::~SmartActionImpl()
{
    std::cout << "~SmartActionImpl\n";
    for (auto &task : tidyUpTasks)
    {
        task();
        std::cout << "did tidy up task\n";
    }
}

// typedef

void SmartActionImpl::buttonWithCb(int X, int Y, int W, int H, const char *L, void (SmartActionImpl::*m)())
{
    auto b = WidgetAccess<Fl_Button>::create(X, Y, W, H, L);
    std::function<void()> action = std::bind(m, this);
    FL_INLINE_CALLBACK_1(
        b->get(),
        std::function<void()>,
        action,
        action,
        {
            action();
        });

    tidyUpTasks.push_back([b]()
                          {
        auto widget = b->get();
        if (widget) {
            widget->callback((Fl_Callback*) nullptr);
            std::cout << "removed callback for button " << widget->label() << "\n";
        } else {
            std::cout << "button already deleated, no callback was to remove\n";
        } });
}
