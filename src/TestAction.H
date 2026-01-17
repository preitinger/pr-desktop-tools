#ifndef __TEST_ACTION_H__
#define __TEST_ACTION_H__

#include "BaseAction.H"
#include "WidgetAccess.H"

class Fl_Input;

SCLASS(TestAction) : public BaseAction
{
public:
    TestAction(BaseActionWP&& parent, const WidgetAccessSP<Fl_Group> &group);
    virtual ~TestAction();
    void start() override final;

    private:
    void onCancel();
    void onInput();

    private:
    WidgetAccessSP<Fl_Group> group;
    ButtonSP cancelB;
    WidgetInActionSP<Fl_Input> nameInput;


};

#endif // __TEST_ACTION_H__
