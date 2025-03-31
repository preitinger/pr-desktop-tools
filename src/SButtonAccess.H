#ifndef __SBUTTON_ACCESS_H__
#define __SBUTTON_ACCESS_H__

#include "Macros.H"

#include <atomic>
#include <mutex>
#include <condition_variable>

class Button;

SCLASS(SButtonAccess)
{
public:
    static SButtonAccessSP create(Button * b);

    virtual ~SButtonAccess() {}

    virtual std::shared_ptr<Button> tryLock() = 0;

    protected:
    
    /**
     * called by friend class Button
     */
    virtual void destroyingButton() = 0;

    friend class Button;
};

#endif