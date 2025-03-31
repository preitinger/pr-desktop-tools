#ifndef __STOCKS_ACTIVITY_H__
#define __STOCKS_ACTIVITY_H__

#include "Activity.H"
#include "SButtonAccess.H"

#include <thread>

class StocksActivity : public Activity
{
public:
    StocksActivity(Fl_Window *window, Display *display, const std::function<void()>& onFinish);
    virtual ~StocksActivity();
    void start();

    // intern:
    void onDoneClick();
    void onReallyDoneClick();

    private:
    std::thread t;
    SButtonAccessSP button;

    void run();
};

#endif
