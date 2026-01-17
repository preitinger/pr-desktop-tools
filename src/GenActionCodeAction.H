#ifndef __GEN_ACTION_CODE_ACTION_H__
#define __GEN_ACTION_CODE_ACTION_H__

#include "BaseAction.H"
#include "WidgetAccess.H"

class Fl_Input;

SCLASS(GenActionCodeAction) : public BaseAction
{
public:
    GenActionCodeAction(BaseActionWP&& parent, const WidgetAccessSP<Fl_Group> &group);
    virtual ~GenActionCodeAction();
    void start() override final;

    private:
    void onCancel();
    void onInput();

    private:
    WidgetAccessSP<Fl_Group> group;
    ButtonSP cancelB;
    WidgetInActionSP<Fl_Input> nameInput;


};

#endif // __GEN_ACTION_CODE_ACTION_H__
