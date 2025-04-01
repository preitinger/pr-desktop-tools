#include "WidgetAccess.H"

#include <iostream>

template <class Widget>
class WidgetAccessImpl;

template <class Widget>
using WidgetAccessImplSP = std::shared_ptr<WidgetAccessImpl<Widget>>;

template <class Widget>
class AccessedWidget : public Widget
{
public:
    AccessedWidget(int X, int Y, int W, int H, const char *L, const WidgetAccessImplSP<Widget>& access);
    virtual ~AccessedWidget();

private:
    WidgetAccessImplSP<Widget> access;
};

template <class Widget>
class WidgetAccessImpl : public WidgetAccess<Widget>
{
public:
    WidgetAccessImpl();
    virtual ~WidgetAccessImpl();
    virtual Widget* get();
    
    // intern only in Impl
    void set(Widget*);


private:
    Widget *widget;
};

///////////////////////////////////////////////////


// static
template <class Widget>
WidgetAccessSP<Widget> WidgetAccess<Widget>::create(int X, int Y, int W, int H, const char *L)
{
    WidgetAccessImplSP<Widget> access(new WidgetAccessImpl<Widget>());
    AccessedWidget<Widget>* widget = new AccessedWidget<Widget>(X, Y, W, H, L, access);
    return access;
}

template <class Widget>
AccessedWidget<Widget>::AccessedWidget(int X, int Y, int W, int H, const char *L, const WidgetAccessImplSP<Widget>& access)
    : Widget(X, Y, W, H, L),
    access(access)
{
    access->set(this);
}

template <class Widget>
AccessedWidget<Widget>::~AccessedWidget()
{
    access->set(nullptr);
    std::cout << "~AccessedWidget\n";
}

template <class Widget>
WidgetAccessImpl<Widget>::WidgetAccessImpl()
: widget(nullptr)
{
}

template <class Widget>
WidgetAccessImpl<Widget>::~WidgetAccessImpl()
{
    std::cout << "~WidgetAccessImpl\n";

}

template <class Widget>
Widget* WidgetAccessImpl<Widget>::get()
{
    return this->widget;
}

template <class Widget>
void WidgetAccessImpl<Widget>::set(Widget* widget)
{
    this->widget = widget;
}
