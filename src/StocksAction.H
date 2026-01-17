#ifndef __STOCKS_ACTION_H__
#define __STOCKS_ACTION_H__

#include "BaseAction.H"
#include "Robot.H"

#include <vector>

SCLASS(StocksAction);

class StocksAction : public BaseAction
{
public:
    StocksAction(const BaseActionWP &parent, const WidgetAccessSP<Fl_Group> &group);
    StocksAction(const StocksAction &) = delete;
    virtual ~StocksAction();

    void start() override;

private:
    void onTimeout();

private:
    WidgetAccessSP<Fl_Group> group;
    TimeoutInActionSP nextTimeout;
    std::vector<int> coordsToClick;
    std::vector<int>::const_iterator nextCoords;
    ClickState clickState;
    Display *display;
    std::unique_ptr<awt::Robot> robot;

};

#endif // __STOCKS_ACTION_H__
