#ifndef __BASE_ACTION_H__
#define __BASE_ACTION_H__

#include "Macros.H"
#include "WidgetAccess.H"
#include "VerboseObject.H"

#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/fl_callback_macros.H>

#include <list>
#include <memory>
#include <functional>

#include <cassert>

class Fl_Output;

SCLASS(BaseAction);

template <class Widget>
class WidgetInAction;
template <class Widget>
using WidgetInActionSP = std::shared_ptr<WidgetInAction<Widget>>;
template <class Widget>
using WidgetInActionWP = std::weak_ptr<WidgetInAction<Widget>>;
template <class Widget>
using WidgetInActionUP = std::unique_ptr<WidgetInAction<Widget>>;

template <class Widget>
class WidgetInActionImpl;
template <class Widget>
using WidgetInActionImplSP = std::shared_ptr<WidgetInActionImpl<Widget>>;
template <class Widget>
using WidgetInActionImplWP = std::weak_ptr<WidgetInActionImpl<Widget>>;
template <class Widget>
using WidgetInActionImplUP = std::unique_ptr<WidgetInActionImpl<Widget>>;

template <class Widget>
class WidgetInAction
{
public:
    virtual ~WidgetInAction() {}
    template <class T>
    void callback(void (T::*m)());

    virtual Widget *get() = 0;
    virtual WidgetAccessSP<Widget> access() = 0;
};

SCLASS(TimeoutInAction)
{
public:
    virtual ~TimeoutInAction() {}
    virtual void cancel() = 0;
};

typedef std::list<std::function<void()>> TidyUpList;

using ButtonInAction = WidgetInAction<Fl_Button>;

class BaseAction : public std::enable_shared_from_this<BaseAction>
{
public:
    template <class Widget>
    using WidgetSP = WidgetInActionSP<Widget>;
    template <class Widget>
    using WidgetUP = WidgetInActionUP<Widget>;
    using ButtonSP = WidgetInActionSP<Fl_Button>;

    /**
     * not yet create widgets
     */
    BaseAction(const std::weak_ptr<BaseAction> &parent);
    BaseAction(std::weak_ptr<BaseAction> &&parent);
    BaseAction(const BaseAction&) = delete;
    BaseAction(BaseAction&&) = delete;
    virtual ~BaseAction();

    /**
     * create widgets, set Callbacks, and eventually add timeouts.
     */
    virtual void start() = 0;

protected:
    virtual void exit() final;
    virtual void afterSubAction();
    template <class T>
    TimeoutInActionSP timeout(double seconds, void (T::*m)());

    template <class T>
    WidgetUP<Fl_Button> button(int X, int Y, int W, int H, const char *L, void (T::*m)());
    template <class T, class Widget>
    WidgetUP<Widget> widget(int X, int Y, int W, int H, const char *L, void (T::*m)());

private:
    template <class Widget, class T>
    TidyUpList::iterator callback(const WidgetAccessSP<Widget> &access, void (T::*m)());
    void removeTidyUp(TidyUpList::iterator &itTidyUpCb);

private:
    VerboseObject firstMember;
    std::weak_ptr<BaseAction> parent;
    std::list<std::function<void()>> tidyUpTasks;
    VerboseObject lastMember;

    friend class WidgetInActionImpl<Fl_Button>;
    friend class WidgetInActionImpl<Fl_Input>;
    friend class WidgetInActionImpl<Fl_Output>;
    friend class WidgetInActionImpl<Fl_Group>;
};

template <class Widget>
class WidgetInActionImpl : public WidgetInAction<Widget>
{
public:
    WidgetInActionImpl(const std::weak_ptr<BaseAction> &action, int X, int Y, int W, int H, const char *L);
    virtual ~WidgetInActionImpl();

    template <class T>
    void setCallback(void (T::*m)());

    Widget *get() override;
    WidgetAccessSP<Widget> access() override;

private:
    std::weak_ptr<BaseAction> action;
    TidyUpList::iterator itTidyUpCb;
    WidgetAccessSP<Widget> m_access;
};

template <class T>
class TimeoutInActionImpl : public TimeoutInAction
{
public:
    TimeoutInActionImpl(double seconds, const std::weak_ptr<T> &t, void (T::*m)());
    virtual ~TimeoutInActionImpl();
    void cancel() override;
    void run();

private:
    std::weak_ptr<T> t;
    void (T::*m)();
};

//////////////////////////////

// template implementations

template <class Widget, class T>
TidyUpList::iterator BaseAction::callback(const WidgetAccessSP<Widget> &access, void (T::*m)())
{
    std::cout << "BaseAction::callback()\n";
    if (!access)
        return tidyUpTasks.end();
    Widget *widget = access->get();
    if (!widget)
        return tidyUpTasks.end();

    if (m)
    {
        T *t = dynamic_cast<T *>(this);
        if (t)
        {
            std::function<void()> action = std::bind(m, t);
            FL_INLINE_CALLBACK_1(
                widget,
                std::function<void()>,
                action,
                action,
                {
                    action();
                });

            std::cout << "pushing back tidyUpTask\n";
            tidyUpTasks.push_back([access]()
                                  {
            auto widget = access->get();
            if (widget) {
                widget->callback((Fl_Callback*) nullptr);
                std::cout << "removed callback for button " << widget->label() << "\n";
            } else {
                std::cout << "button already deleated, no callback was to remove\n";
            } });
            std::cout << "new size of tidyUpTasks " << tidyUpTasks.size() << "\n";
            auto it = tidyUpTasks.end();
            return --it;
        }
        else
        {
            throw std::bad_cast();
            return tidyUpTasks.end();
        }
    }
    else
    {
        access->get()->callback((Fl_Callback *)nullptr);
        return tidyUpTasks.end();
    }
}

template <class T>
BaseAction::WidgetUP<Fl_Button> BaseAction::button(int X, int Y, int W, int H, const char *L, void (T::*m)())
{
    // std::cout << "BEGIN BaseAction::button()\n";
    // std::weak_ptr<BaseAction> weakThis = weak_from_this();
    // WidgetInActionImplUP<Fl_Button> b(new WidgetInActionImpl<Fl_Button>(std::move(weakThis), X, Y, W, H, L));
    // if (m)
    //     b->callback(m);
    // std::cout << "END BaseAction::button()\n";
    // return b;
    return widget<T, Fl_Button>(X, Y, W, H, L, m);
}

template <class T, class Widget>
BaseAction::WidgetUP<Widget> BaseAction::widget(int X, int Y, int W, int H, const char *L, void (T::*m)())
{
    std::cout << "BEGIN BaseAction::widget()\n";
    std::weak_ptr<BaseAction> weakThis = weak_from_this();
    WidgetInActionImplUP<Widget> b(new WidgetInActionImpl<Widget>(std::move(weakThis), X, Y, W, H, L));
    if (m)
        b->callback(m);
    std::cout << "END BaseAction::widget()\n";
    return b;

}

template <class Widget>
template <class T>
void WidgetInAction<Widget>::callback(void (T::*m)())
{
    std::cout << "WidgetInAction::callback()\n";
    WidgetInActionImpl<Widget> *impl = dynamic_cast<WidgetInActionImpl<Widget> *>(this);
    assert(impl);
    impl->setCallback(m);
}

template <class Widget>
template <class T>
void WidgetInActionImpl<Widget>::setCallback(void (T::*m)())
{
    std::cout << "WidgetInActionImpl::setCallback()\n";
    BaseActionSP lockedAction = this->action.lock();

    if (lockedAction)
    {
        std::cout << "before removeTidyUp\n";
        lockedAction->removeTidyUp(itTidyUpCb);
        std::cout << "after removeTidyUp\n";
        itTidyUpCb = lockedAction->callback(this->m_access, m);
    }
    else
    {
        std::cout << "warning: lockedAction false\n";
    }
    // this->itTidyUpCb = this->action->addTidyUp
}

template <class Widget>
WidgetInActionImpl<Widget>::WidgetInActionImpl(const std::weak_ptr<BaseAction> &action, int X, int Y, int W, int H, const char *L)
    : action(action), itTidyUpCb(), m_access()
{
    m_access = WidgetAccess<Widget>::create(X, Y, W, H, L);
}

// virtual
template <class Widget>
WidgetInActionImpl<Widget>::~WidgetInActionImpl()
{
    std::cout << "~WidgetInActionImpl()\n";
    auto lockedAction = action.lock();
    if (lockedAction)
    {
        std::cout << "before removeTidyUp op\n";
        lockedAction->removeTidyUp(itTidyUpCb);
        std::cout << "after removeTidyUp op\n";
    }
}

template <class Widget>
Widget *WidgetInActionImpl<Widget>::get()
{
    return m_access->get();
}

template <class Widget>
WidgetAccessSP<Widget> WidgetInActionImpl<Widget>::access()
{
    return m_access;
}

template <class T>
TimeoutInActionSP BaseAction::timeout(double seconds, void (T::*m)())
{
    std::shared_ptr<BaseAction> sharedThis = shared_from_this();
    std::shared_ptr<T> sharedT = std::dynamic_pointer_cast<T>(sharedThis);

    return TimeoutInActionSP(new TimeoutInActionImpl<T>(seconds, sharedT, m));
}

template <class T>
static void cbTimeout(void *data)
{
    CB_CAST(TimeoutInActionImpl<T>, timeout);
    timeout->run();
}

template <class T>
TimeoutInActionImpl<T>::TimeoutInActionImpl(double seconds, const std::weak_ptr<T> &t, void (T::*m)())
    : t(t), m(m)
{
    Fl::add_timeout(seconds, &cbTimeout<T>, this);
}

// virtual
template <class T>
TimeoutInActionImpl<T>::~TimeoutInActionImpl()
{
    std::cout << "~TimeoutInActionImpl\n";
    Fl::remove_timeout(&cbTimeout<T>, this);
    std::cout << "called Fl_remove_timeout\n";
}

template <class T>
void TimeoutInActionImpl<T>::cancel()
{
    Fl::remove_timeout(&cbTimeout<T>, this);
    std::cout << "called Fl_remove_timeout\n";
}

template <class T>
void TimeoutInActionImpl<T>::run()
{
    if (auto t1 = t.lock())
    {
        std::bind(m, t1)();
    }
}

#endif // __BASE_ACTION_H__
