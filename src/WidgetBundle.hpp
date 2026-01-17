#ifndef __WIDGET_BUNDLE_H__
#define __WIDGET_BUNDLE_H__

#include "WidgetAccess.H"
#include "Callback.H"

#include <memory>

template <class Widget>
class WidgetBundle;

template <class Widget>
using WidgetBundleSP = std::shared_ptr<WidgetBundle<Widget>>;

template <class Widget>
using WidgetBundleWP = std::weak_ptr<WidgetBundle<Widget>>;

template <class Widget>
class WidgetBundle
{
public:
    static WidgetBundleSP create(int X, int Y, int W, int H, const char* L = nullptr);
    virtual ~WidgetBundle();
    

private:
    WidgetAccessSP m_access;
    CallbackSP m_callback;
};

#endif //  __WIDGET_BUNDLE_H__