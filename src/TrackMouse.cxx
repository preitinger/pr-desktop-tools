#include "TrackMouse.H"
#include "DbgButton.H"
#include "Macros.H"

#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Output.H>
#include <FL/fl_callback_macros.H>

#include <iostream>
#include <functional>

static void cbOnTimeout(void* data) {
    CB_CAST(TrackMouse, trackMouse);
    trackMouse->onTimeout();
}

TrackMouse::TrackMouse(Fl_Window* window, const OnFinish &onFinish)
    : window(window),
    onFinish(onFinish),
      output(nullptr),
      cancelButton(nullptr)
{
    window->clear();
    window->begin();

    output = new Fl_Output(200, 0, 100, 20, "Mausposition");
    cancelButton = new DbgButton(200, 20, 100, 20, "Abbrechen");
    FL_METHOD_CALLBACK_0(cancelButton, TrackMouse, this, cancel);

    window->end();
    window->redraw();
    Fl::add_timeout(0.1, cbOnTimeout, this);
}

TrackMouse::~TrackMouse()
{
    std::cout << "~TrackMouse\n";
    Fl::remove_timeout(cbOnTimeout, this);
    delete output;
    output = nullptr;
    delete cancelButton;
    cancelButton = nullptr;

    if (onFinish)
    {
        onFinish();
        onFinish = OnFinish();
    }
}

void TrackMouse::onTimeout() {
    int x, y;
    Fl::get_mouse(x, y);
    char text[256];
    snprintf(text, 256, "%d, %d", x, y);
    output->value(text);
    Fl::add_timeout(0.1, cbOnTimeout, this);
}

void TrackMouse::cancel()
{
    delete this;
}
