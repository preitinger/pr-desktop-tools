#ifndef __EXAMPLE_ACTION_H__
#define __EXAMPLE_ACTION_H__

#include "Macros.H"
#include "BaseAction.H"
#include "WidgetAccess.H"

class Fl_Group;
class Fl_Button;
class Fl_Output;

SCLASS(ExampleAction) : public BaseAction
{
public:
    ExampleAction(const std::weak_ptr<BaseAction> &parent, const WidgetAccessSP<Fl_Group> &group);
    virtual ~ExampleAction();
    void start() override final;

private:
    void enableAction();
    void disableAction();
    void doAction();
    void onTestTimeout();

private:
    WidgetAccessSP<Fl_Group> group;
    using B = BaseAction::WidgetSP<Fl_Button>;
    B exitB;
    B enableDisableB;
    B actionB;
    using O = WidgetAccessSP<Fl_Output>;
    O numActionsO;
    int count;
    TimeoutInActionSP testTimeout;
};

#endif // __EXAMPLE_ACTION_H__
