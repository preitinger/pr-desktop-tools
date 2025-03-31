#ifndef __SGENERIC_ACCESS_H__
#define __SGENERIC_ACCESS_H__

#include <memory>
#include <functional>

#include <FL/Fl_Window.H>


typedef std::function<void()> OnFinish;

template <class Widget>
class GenericAccess;

template <class Widget>
using GenericAccessSP = std::shared_ptr<GenericAccess<Widget>>;
template <class Widget>
using GenericAccessWP = std::weak_ptr<GenericAccess<Widget>>;

template <class Widget>
class GenericAccess
{
public:
    GenericAccess(Widget *w);
    virtual ~GenericAccess() {}
    virtual std::shared_ptr<Widget> tryLock() = 0;

protected:
    virtual void destroyingWidget() = 0;
};

template <class Widget>
class GenericWidget : public Widget
{
public:
    GenericWidget(int X, int Y, int W, int H, const char *L);
    virtual ~GenericWidget();
    GenericAccessSP<Widget> access();

private:
    GenericAccessWP<Widget> m_access;
};

using WindowAccess = GenericAccess<Fl_Window>;

// implementations

template <class Widget>
GenericWidget<Widget>::GenericWidget(int X, int Y, int W, int H, const char *L)
    : Widget(X, Y, W, H, L),
      m_access()
{
}

template <class Widget>
GenericWidget<Widget>::~GenericWidget()
{
}

#endif
