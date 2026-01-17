#ifndef __WIDGET_ACCESS_H__
#define __WIDGET_ACCESS_H__

///
/// FLTK widgets can be deleted at any time by their parents. This is why they cannot be packed into smart pointers.
/// The data structures enables safe management of pointers to FLTK widgets.
/// Instead of creating a widget of type Widget directly, an instance of WidgetAccess<Widget> is created which creates a sub-classed widget and provides access to it by the
/// method get(). The helper class ensures that get() returns only non-null if the Widget has not been destructed meanwhile by others, i.e. a parent of the widget.
///

#include <memory>


template <class Widget>
class WidgetAccess;

template <class Widget>
using WidgetAccessSP = std::shared_ptr<WidgetAccess<Widget>>;
template <class Widget>
using WidgetAccessWP = std::weak_ptr<WidgetAccess<Widget>>;

template <class Widget>
class WidgetAccess : public std::enable_shared_from_this<WidgetAccess<Widget>>
{
public:
    static WidgetAccessSP<Widget> create(int X, int Y, int W, int H, const char* L = nullptr);
    template <class Widget2>
    WidgetAccessSP<Widget2> convert();
    virtual Widget* get() = 0;
};

//////////////////////////////////////////////////////////////////////////////////

#include "WidgetAccessImpl.H"

#endif // __WIDGET_ACCESS_H__
