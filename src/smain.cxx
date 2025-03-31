#include "CountDown.H"
#include "TrackMouse.H"
#include "Stocks.H"
#include "Macros.H"
#include "DbgButton.H"

#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Int_Input.H>
#include <FL/fl_callback_macros.H>
#include <FL/Fl_Float_Input.H>

#include <iostream>
#include <functional>

template <class Activity>
void addActivity(Fl_Window* window, const OnFinish& onFinish, int& Y, const char* L)
{
    Fl_Button *button = new DbgButton(0, Y, 400, 20, L);
    Y += 20;
    FL_INLINE_CALLBACK_2(button, Fl_Window *, window, window, OnFinish, onFinish, onFinish, {
        new Activity(window, onFinish); // deleted sich selbst und ruft im Destructor den onFinish-Callback auf, falls er gesetzt ist.
    });

}

void setMainMenu(void *data)
{
    CB_CAST(Fl_Window, window);

    // Nach jeder Activity wieder das Hauptmenu setzen.
    OnFinish onFinish = std::bind(setMainMenu, window);

    window->clear();
    window->begin();

    int y = 0;
    addActivity<CountDown>(window, onFinish, y, "Count-Down");
    addActivity<TrackMouse>(window, onFinish, y, "Track Mouse Position");
    addActivity<Stocks>(window, onFinish, y, "Stocks");

    window->end();
    window->redraw();
}

int main()
{
    Fl_Window *window = new Fl_Window(400, 400, "pr-desktop-tools");
    setMainMenu(window);
    window->end();
    window->show();
    return Fl::run();
}