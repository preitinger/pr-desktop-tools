#include "TrackMouseActivity.H"
#include "Button.H"
#include <FL/Fl_Button.H>
#include <FL/fl_callback_macros.H>

#include <iostream>

TrackMouseActivity::TrackMouseActivity(Fl_Window *window, Display *display, const OnFinish &onFinish)
    : Activity(window, display, onFinish),
      t(), output(nullptr),
      finishB(nullptr),
      stopped(false),
      m(),
      cv(),
      destroying(false)
{
}

void TrackMouseActivity::start()
{
    if (t.joinable())
        t.join();

    window->clear();
    window->begin();
    output = new Fl_Output(0, 0, 100, 20, "Mauspos.");
    finishB = new Fl_Button(0, 20, 100, 20, "Stop");
    FL_METHOD_CALLBACK_0(finishB, TrackMouseActivity, this, onStop);
    window->end();
    window->redraw();
    t = std::thread(&TrackMouseActivity::run, this);
}

TrackMouseActivity::~TrackMouseActivity()
{
    std::cout << "~TrackMouseActivity\n";
    destroying = true;
    onStop();
    if (t.joinable()) {
        std::cout << "~TrackMouseActivity: before join\n";
        t.join();
        std::cout << "~TrackMouseActivity: after join\n";
    }
    std::cout << "ENDE ~TrackMouseActivity\n";
}

void TrackMouseActivity::run()
{
    std::cout << "Enter TrackMouseActivity::run()\n";
    stopped = false;

    {
        std::unique_lock l(m);

        while (!stopped)
        {
            // std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            // std::cout << "while before wait_for\n";
            cv.wait_for(l, std::chrono::milliseconds(50));
            // std::cout << "while after wait_for\n";
            if (stopped)
                break;
            int x, y;
            Fl::get_mouse(x, y);
            char text[256];
            snprintf(text, 256, "%d, %d", x, y);
            Fl::lock();
            output->value(text);
            output->redraw();
            Fl::unlock();
            Fl::awake();
        }
    }

    if (!destroying) onFinish();
    std::cout << "Leave TrackMouseActivity::run()\n";
}

void TrackMouseActivity::onStop()
{
    std::cout << "onStop\n";
    std::unique_lock l(m);
    std::cout << "onStop after lock\n";

    stopped = true;
    cv.notify_all();
}