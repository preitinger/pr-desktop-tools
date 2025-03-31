#include "SActivity.H"

// SActivity::SActivity(const GenericAccess<Fl_Window>& window, Display *display, const OnFinish& onFinish) {

// }

SActivity::SActivity(const GenericAccessSP<Fl_Window> &window, Display *display, const OnFinish &onFinish)
    : window(window),
      display(display),
      onFinish(onFinish)
{
}

SActivity::~SActivity()
{
}