#ifndef __BLA_BLUBB_H__
#define __BLA_BLUBB_H__

#include "BaseAction.H"
#include "WidgetAccess.H"

class Fl_Input;

SCLASS(BlaBlubb) : public BaseAction
{
public:
    BlaBlubb(BaseActionWP&& parent, const WidgetAccessSP<Fl_Group> &group);
    virtual ~BlaBlubb();
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

#endif // __BLA_BLUBB_H__
