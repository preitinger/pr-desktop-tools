#include "Activity.H"

Activity::Activity(Fl_Window *window, Display *display, const std::function<void()>& onFinish) : window(window), display(display), onFinish(onFinish)
{
}
