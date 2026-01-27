#include "WidgetAccess.H"

#include <iostream>
#include <string>

template <class Widget>
class WidgetAccessImpl;

template <class Widget>
using WidgetAccessImplSP = std::shared_ptr<WidgetAccessImpl<Widget>>;

template <class Widget>
class AccessedWidget : public Widget
{
public:
    AccessedWidget(int X, int Y, int W, int H, const char *L, const WidgetAccessImplSP<Widget> &access);
    virtual ~AccessedWidget();

private:
    WidgetAccessImplSP<Widget> access;
};

template <class Widget>
class WidgetAccessImpl : public WidgetAccess<Widget>
{
public:
    WidgetAccessImpl(const char *debugLabel);
    virtual ~WidgetAccessImpl();
    virtual Widget *get() override;

    // intern only in Impl
    void set(Widget *);

private:
    Widget *widget;
    std::string label;
};

template <class Widget, class OriginWidget>
class DerivedWidgetAccess : public WidgetAccess<Widget>
{
public:
    DerivedWidgetAccess(const WidgetAccessWP<OriginWidget> &origin);
    virtual ~DerivedWidgetAccess();
    virtual Widget *get() override;

private:
    WidgetAccessWP<OriginWidget> origin;
};

///////////////////////////////////////////////////

// static
template <class Widget>
WidgetAccessSP<Widget> WidgetAccess<Widget>::create(int X, int Y, int W, int H, const char *L)
{
    WidgetAccessImplSP<Widget> access(new WidgetAccessImpl<Widget>(L));
    /* AccessedWidget<Widget> *widget = */ new AccessedWidget<Widget>(X, Y, W, H, L, access);
    return access;
}

template <class Widget>
AccessedWidget<Widget>::AccessedWidget(int X, int Y, int W, int H, const char *L, const WidgetAccessImplSP<Widget> &access)
    : Widget(X, Y, W, H, L),
      access(access)
{
    access->set(this);
}

template <class Widget>
AccessedWidget<Widget>::~AccessedWidget()
{
    access->set(nullptr);
    const char *label = this->label();
    std::cout << "~AccessedWidget " << this << " [" << (label ? label : "") << "]" << "\n";
}

template <class Widget>
WidgetAccessImpl<Widget>::WidgetAccessImpl(const char *L)
    : widget(nullptr), label()
{
    if (L)
        label = L;
}

template <class Widget>
WidgetAccessImpl<Widget>::~WidgetAccessImpl()
{
    std::cout << "~WidgetAccessImpl [" << label << "]\n";
}

template <class Widget>
Widget *WidgetAccessImpl<Widget>::get()
{
    return this->widget;
}

template <class Widget>
void WidgetAccessImpl<Widget>::set(Widget *widget)
{
    this->widget = widget;
}

template <class Widget>
template <class Widget2>
WidgetAccessSP<Widget2> WidgetAccess<Widget>::convert()
{
    auto weakThis = std::enable_shared_from_this<WidgetAccess<Widget>>::weak_from_this();
    return WidgetAccessSP<Widget2>(new DerivedWidgetAccess<Widget2, Widget>(weakThis));
}

template <class Widget, class OriginWidget>
DerivedWidgetAccess<Widget, OriginWidget>::DerivedWidgetAccess(const WidgetAccessWP<OriginWidget> &origin)
    : WidgetAccess<Widget>(), origin(origin)
{
}

// virtual
template <class Widget, class OriginWidget>
DerivedWidgetAccess<Widget, OriginWidget>::~DerivedWidgetAccess()
{
}

template <class Widget, class OriginWidget>
Widget *DerivedWidgetAccess<Widget, OriginWidget>::get()
{
    if (auto lockedOrigin = origin.lock())
    {
        OriginWidget *w = lockedOrigin->get();
        return dynamic_cast<Widget *>(w);
    }
    else
    {
        return nullptr;
    }
}
