#include "Activity.H"

#include <thread>
#include <atomic>

class SleepActivity : public Activity
{
public:
    SleepActivity(Fl_Window *window, Display *display, const std::function<void()> &onFinish);
    virtual ~SleepActivity();
    void start();

private:
    std::thread t;
    std::atomic<bool> destroying;

    void run();
};
