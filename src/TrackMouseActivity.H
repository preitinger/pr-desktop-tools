#ifndef __TRACK_MOUSE_ACTIVITY_H__
#define __TRACK_MOUSE_ACTIVITY_H__

#include "Activity.H"
#include "Button.H"

#include <FL/Fl_Output.H>
#include <FL/Fl_Button.H>

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

class TrackMouseActivity : public Activity
{
public:
    TrackMouseActivity(Fl_Window *window, Display *display, const OnFinish &onFinish);
    virtual ~TrackMouseActivity();

    void start();

    // intern
    void onStop();

private:
    std::thread t;
    Fl_Output* output;
    Fl_Button* finishB;
    bool stopped;
    std::mutex m;
    std::condition_variable cv;
    bool destroying;


    void run();
};

#endif
