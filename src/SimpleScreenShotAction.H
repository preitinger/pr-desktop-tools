#ifndef __SIMPLE_SCREEN_SHOT_ACTION_H__
#define __SIMPLE_SCREEN_SHOT_ACTION_H__

#include "BaseAction.H"

SCLASS(SimpleScreenShotAction) : public BaseAction
{
    public:
    SimpleScreenShotAction(std::weak_ptr<BaseAction> &&parent, const WidgetAccessSP<Fl_Group> &group);
};



#endif // __SIMPLE_SCREEN_SHOT_ACTION_H__
