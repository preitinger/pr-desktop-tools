#include "Stocks.H"
#include "Macros.H"

#include <FL/Fl_Window.H>

#include <cassert>

static void cbOnTimeout(void *data)
{
    CB_CAST(Stocks, stocks);
    std::cout << "cbOnTimeout for Stocks\n";
    stocks->onTimeout();
}

Stocks::Stocks(Fl_Window *window, const OnFinish &onFinish)
    : window(window),
      onFinish(onFinish),
      coordsToClick({
          1800,
          160,
          1800,
          200,
          1600,
          545,
          1600,
          365,
      }),
      nextCoords(coordsToClick.begin()),
      clickState(ClickState::MOVE),
      display(nullptr),
      robot(nullptr)
{
    window->clear();
    window->redraw();
    Fl::add_timeout(0.5, cbOnTimeout, this);
    display = XOpenDisplay(NULL);
    assert(display);
    robot = new awt::Robot(display);
}

Stocks::~Stocks()
{
    Fl::remove_timeout(cbOnTimeout, this);
    delete robot;
    robot = nullptr;
    XCloseDisplay(display);
    display = nullptr;

    if (onFinish)
    {
        onFinish();
    }
}

void Stocks::onTimeout()
{
    std::cout << "Stocks::onTimeout " << (coordsToClick.end() - nextCoords) / 2 << " remaining - state " << clickState << "\n";
    if (coordsToClick.end() - nextCoords < 2)
    {
        delete this;
    }

    int x = nextCoords[0];
    int y = nextCoords[1];

    std::cout << "x " << x << " y " << y << "\n";
    switch (clickState)
    {
    case ClickState::MOVE:
        robot->mouseMove(x, y);
        clickState = ClickState::PRESS;
        Fl::add_timeout(0.050, cbOnTimeout, this);
        break;

    case ClickState::PRESS:
        robot->mousePress(Button1);
        clickState = ClickState::RELEASE;
        Fl::add_timeout(0.050, cbOnTimeout, this);
        break;

    case ClickState::RELEASE:
        robot->mouseRelease(Button1);
        robot->mouseMove(0, 0);
        nextCoords += 2;
        if (coordsToClick.end() - nextCoords >= 2) {
            clickState = ClickState::MOVE;
            Fl::add_timeout(0.25, cbOnTimeout, this);
        } else {
            // all clicks done
            delete this;
        }
        break;
    }
}