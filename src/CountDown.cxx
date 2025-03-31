#include "DbgButton.H"
#include "CountDown.H"
#include "Macros.H"

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Int_Input.H>
#include <FL/fl_callback_macros.H>
#include <FL/Fl_Float_Input.H>

#include <iostream>
#include <functional>

static void cbOnTimeout(void* data) {
    CB_CAST(CountDown, countDown);
    countDown->onTimeout();
}

CountDown::CountDown(Fl_Window* window, const OnFinish &onFinish)
    : window(window),
    onFinish(onFinish),
      rest(10),
      countInput(nullptr),
      startButton(nullptr),
      output(nullptr),
      cancelButton(nullptr)
{
    window->clear();
    window->begin();

    countInput = new Fl_Int_Input(200, 0, 100, 20, "Zähler");
    countInput->value(10);
    stepInput = new Fl_Float_Input(200, 20, 100, 20, "Zeit pro Schritt in Sek.");
    stepInput->value(0.5);
    startButton = new DbgButton(200, 40, 100, 20, "Start");
    FL_METHOD_CALLBACK_0(startButton, CountDown, this, start);
    cancelButton = new DbgButton(200, 60, 100, 20, "Abbrechen");
    FL_METHOD_CALLBACK_0(cancelButton, CountDown, this, cancel);

    window->end();
    window->redraw();
}

CountDown::~CountDown()
{
    // std::cout << "~CountDown\n";
    Fl::remove_timeout(cbOnTimeout, this);

    delete output;
    output = nullptr;
    delete startButton;
    startButton = nullptr;
    delete countInput;
    countInput = nullptr;
    delete stepInput;
    stepInput = nullptr;
    delete cancelButton;
    cancelButton = nullptr;

    if (onFinish)
    {
        onFinish();
        onFinish = OnFinish();
    }
}

void CountDown::start()
{
    // std::cout << "CountDown::start()\n";
    rest = countInput->ivalue();
    step = stepInput->dvalue();

    delete countInput;
    countInput = nullptr;
    delete startButton;
    startButton = nullptr;

    window->begin();
    output = new Fl_Output(200, 0, 100, 20, "Count");
    // std::cout << "rest " << rest << "\n";
    output->value(rest);
    window->end();
    window->redraw();
    Fl::add_timeout(step, cbOnTimeout, this);

}

void CountDown::onTimeout()
{
    //std::cout << "onTimeout " << rest << "\n";
    if (--rest <= 0)
    {
        delete this;
    }
    else
    {
        decrement();
        Fl::add_timeout(step, cbOnTimeout, this);
    }
}

void CountDown::cancel()
{
    //std::cout << "CountDown::cancel\n";
    delete this;
}

void CountDown::decrement()
{
    output->value(rest);
}
