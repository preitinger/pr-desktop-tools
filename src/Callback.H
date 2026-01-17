#ifndef __CALLBACK_H__
#define __CALLBACK_H__

#include "WidgetAccess.H"
#include "Macros.H"

#include <FL/Fl_Widget.H>

#include <memory>
#include <functional>

#include <type_traits>

#include <cassert>

template <class Widget>
class Callback;

template <class Widget>
using CallbackSP = std::shared_ptr<Callback<Widget>>;

template <class Widget>
using CallbackWP = std::weak_ptr<Callback<Widget>>;

template <class Widget>
using CallbackUP = std::unique_ptr<Callback<Widget>>;

template <class Widget>
void genericCb(Fl_Widget *w, void *data);

template <class Widget>
class Callback
{
public:
    static CallbackUP<Widget> create(const WidgetAccessSP<Widget> &access, const std::weak_ptr<std::function<void()>> &action);
    Callback(const WidgetAccessSP<Widget> &access, const std::weak_ptr<std::function<void()>> &action);
    Callback(Callback&& cb) = delete;
    virtual ~Callback();

private:
    void run();
    friend void genericCb<Widget>(Fl_Widget *, void *);

private:
    WidgetAccessSP<Widget> access;
    std::weak_ptr<std::function<void()>> action;
};

// TODO Klasse ActionOwner, die Actions erzeugt fuer Member-Funktionen einer Subklasse von ActionOwner und sich diese merkt.
// Im Destruktor des ActionOwners werden alle gemerkten Actions deaktiviert, so dass dann kein illegaler Pointer verwendet werden kann bei einem verspaeteten Callback.

// template <class T>
// class CallbackOwner
// {
//     public:
//     virtual ~CallbackOwner();

//     void setCallback(void (T::*member)()) {
//         (dynamic_cast<T*>(this)->*member)();
//     }
// };

template <class Widget>
void Callback<Widget>::run()
{
    std::cout << "Callback::run\n";
    auto func = this->action.lock();
    std::cout << "func: " << func << "\n";
    std::cout << "!!func " << !!func << "\n";
    if (func) std::cout << " !!*func " << (!!*func) << "\n";
    if (func && *func) {
        auto refFunc = *func;
        refFunc();
    }
    std::cout << "leave Callback::run\n";
}

template <class Widget>
CallbackUP<Widget> Callback<Widget>::create(const WidgetAccessSP<Widget> &access, const std::weak_ptr<std::function<void()>> &action)
{
    return CallbackUP<Widget>(new Callback<Widget>(access, action));
}

template <class Widget>
Callback<Widget>::Callback(const WidgetAccessSP<Widget> &access, const std::weak_ptr<std::function<void()>> &action)
    : access(access), action(action)
{
    assert(access);
    assert(action.lock());
    Widget *widget = access->get();
    if (widget)
    {
        const char* label = widget->label();
        std::cout << "Callback() for widget " << widget << " [" << (label ? label : "") << "] " << "\n" << std::endl;
        widget->callback(&genericCb<Widget>, this);
    } else {
        std::cout << "no widget?!\n";
    }
}

template <class Widget>
Callback<Widget>::~Callback()
{
    Widget *widget = access->get();
    if (widget)
    {
        const char* label = widget->label();
        std::cout << "~Callback: resetting callback for widget " << widget << " [" << (label ? label : "") << "] " << "\n" << std::endl;
        widget->callback(nullptr, nullptr);
    } else {
        std::cout << "~Callback: widget already deleted, so no callback to reset\n";
    }
}

template <class Widget>
void genericCb(Fl_Widget *w, void *data)
{
    CB_CAST(Callback<Widget>, cb);
    cb->run();
}

#endif // __CALLBACK_H__
