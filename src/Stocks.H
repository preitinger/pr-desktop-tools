#ifndef __STOCKS_H__
#define __STOCKS_H__

#include "OnFinish.H"
#include "Robot.H"

#include <vector>

class Fl_Window;

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