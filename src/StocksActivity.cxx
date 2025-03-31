#include "StocksActivity.H"
#include "Robot.H"
#include "Button.H"

#include <FL/Fl_Button.H>
#include <FL/fl_callback_macros.H>

#include <iostream>

StocksActivity::StocksActivity(Fl_Window *window, Display *display, const std::function<void()> &onFinish)
    : Activity(window, display, onFinish), t(), button()
{
}

StocksActivity::~StocksActivity()
{
    if (t.joinable())
        t.join();
}

void StocksActivity::start()
{
    window->clear();
    window->redraw();
    if (t.joinable())
        t.join();
    t = std::thread(&StocksActivity::run, this);
}

static void cbShowDone(Fl_Widget *widget, void *arg)
{
    StocksActivity *a = (StocksActivity *)arg;
    a->onDoneClick();
}

static void cbShowReallyDone(Fl_Widget *widget, void *arg)
{
    std::cout << "cbShowReallyDone: event_is_click()=" << Fl::event_is_click() << "\n";
    StocksActivity *a = (StocksActivity *)arg;
    a->onReallyDoneClick();
}

void StocksActivity::run()
{
    std::this_thread::sleep_for(std::chrono::seconds(3));

    awt::Robot *r = new awt::Robot(display);
    // 1800 200
    int coords[] = {
        1800, 160,
        1800, 200,
        1600, 545,
        1600, 365};

    for (int i = 0; i < 4; ++i)
    {
        r->delay(500);
        int x = coords[i * 2];
        int y = coords[i * 2 + 1];
        r->mouseMove(x, y);
        // ln("mouseMove");
        r->delay(200);
        r->mousePress(Button1);
        // ln("mousePress");
        r->delay(200);
        r->mouseRelease(Button1);
        // ln("mouseRelease");
        r->mouseMove(0, 0);
        r->delay(500);
    }
    // r->mouseMove()

    delete r;

    Button *b = new Button(0, 0, 100, 20, "Fertig");
    this->button = b->access();

    {
        auto lockedButton = button->tryLock();
        if (lockedButton)
        {
            FL_METHOD_CALLBACK_0(lockedButton.get(), StocksActivity, this, onDoneClick);
        }
    }

    window->add(b);
    Fl::awake();
}

void StocksActivity::onDoneClick()
{
    auto b = button->tryLock();

    if (b)
    {
        FL_METHOD_CALLBACK_0(b, StocksActivity, this, onReallyDoneClick);
        b->label("Wirklich fertig");
    }
}
void StocksActivity::onReallyDoneClick()
{
    button.reset();
    onFinish();
}