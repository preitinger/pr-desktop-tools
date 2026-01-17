#ifndef __TEST_CLASS_H__
#define __TEST_CLASS_H__

#include "BaseAction.H"
#include "WidgetAccess.H"

class Fl_Input;

SCLASS(TestClass) : public BaseAction
{
public:
    TestClass(BaseActionWP&& parent, const WidgetAccessSP<Fl_Group> &group);
    virtual ~TestClass();
    void start() override final;

    private:
    void onCancel();
    void onInput();
    void onTimeout();

    private:
    WidgetAccessSP<Fl_Group> group;
    ButtonSP cancelB;
    WidgetInActionSP<Fl_Input> nameInput;
    TimeoutInActionSP nextTimeout;


};

#endif // __TEST_CLASS_H__
