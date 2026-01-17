#include "StocksAction.H"

#include <FL/Fl_Group.H>

StocksAction::StocksAction(const BaseActionWP &parent, const WidgetAccessSP<Fl_Group> &group)
    : BaseAction(parent),
      group(group),
      nextTimeout(),
      coordsToClick(),
      nextCoords(),
      clickState(ClickState::MOVE),
      display(nullptr),
      robot(nullptr)
{
    display = XOpenDisplay(NULL);
}

void StocksAction::start()
{
    nextTimeout = timeout(0.5, &StocksAction::onTimeout);
    coordsToClick = {
        1800,
        160,
        1800,
        200,
        1600,
        545,
        1600,
        365,
    };
    nextCoords = coordsToClick.begin();
    clickState = ClickState::MOVE;
    robot = std::make_unique<awt::Robot>(display);
}

void StocksAction::onTimeout()
{

    if (coordsToClick.end() - nextCoords < 2)
    {
        delete this;
    }

    int x = nextCoords[0];
    int y = nextCoords[1];

    //std::cout << "x " << x << " y " << y << "\n";
    switch (clickState)
    {
    case ClickState::MOVE:
        robot->mouseMove(x, y);
        clickState = ClickState::PRESS;
        nextTimeout = timeout(0.05, &StocksAction::onTimeout);
        break;

    case ClickState::PRESS:
        robot->mousePress(Button1);
        clickState = ClickState::RELEASE;
        nextTimeout = timeout(0.05, &StocksAction::onTimeout);
        break;

    case ClickState::RELEASE:
        robot->mouseRelease(Button1);
        robot->mouseMove(0, 0);
        nextCoords += 2;
        if (coordsToClick.end() - nextCoords >= 2) {
            clickState = ClickState::MOVE;
            nextTimeout = timeout(0.25, &StocksAction::onTimeout);
        } else {
            // all clicks done
            this->exit();
        }
        break;
    }
}

StocksAction::~StocksAction()
{
    XCloseDisplay(display);
    display = nullptr;
}