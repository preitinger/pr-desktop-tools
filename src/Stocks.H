#ifndef __STOCKS_H__
#define __STOCKS_H__

#include "OnFinish.H"
#include "Robot.H"

#include <vector>

class Fl_Window;

enum class ClickState
{
    MOVE,
    PRESS,
    RELEASE,
};

inline std::ostream &operator<<(std::ostream &o, const ClickState &s)
{
    switch (s)
    {
    case ClickState::MOVE:
        o << "MOVE";
        break;
    case ClickState::PRESS:
        o << "PRESS";
        break;
    case ClickState::RELEASE:
        o << "RELEASE";
        break;
    }
    return o;
}

class Stocks
{
public:
    Stocks(Fl_Window *window, const OnFinish &onFinish);
    virtual ~Stocks();
    /**
     * @return true iff activity has stopped.
     */
    void onTimeout();

private:
    Fl_Window *window;
    OnFinish onFinish;
    std::vector<int> coordsToClick;
    std::vector<int>::const_iterator nextCoords;
    ClickState clickState;
    Display *display;
    awt::Robot *robot;
};

#endif // __STOCKS_H__