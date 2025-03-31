#include "SleepActivity.H"

SleepActivity::SleepActivity(Fl_Window *window, Display *display, const std::function<void()> &onFinish) : Activity(window, display, onFinish), t(), destroying(false)
{
}

SleepActivity::~SleepActivity()
{
    destroying = true;
    if (t.joinable()) t.join();
}
void SleepActivity::start()
{
    window->clear();
    window->redraw();
    if (t.joinable()) t.join();
    t = std::thread(&SleepActivity::run, this);
}

void SleepActivity::run()
{

    std::this_thread::sleep_for(std::chrono::seconds(3));
    if (!destroying) onFinish();
}