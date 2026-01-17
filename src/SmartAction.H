#ifndef __SMART_ACTION_H__
#define __SMART_ACTION_H__

#include "Macros.H"
#include "OnFinish.H"

SCLASS(SmartAction)
{
    public:
    static SmartActionUP create(const OnFinish& onFinish);
    SmartAction() {}
    virtual ~SmartAction() {}
};

#endif // __SMART_ACTION_H__
