#include "SButtonAccess.H"
#include "Button.H"


class ButtonAccessImpl : public SButtonAccess
{

    private:
    Button* button;
    std::mutex m;
    std::condition_variable cv;
    bool locked;

    public:
    ButtonAccessImpl(Button* b);
    virtual ~ButtonAccessImpl();
    virtual std::shared_ptr<Button> tryLock();

    protected:
    virtual void destroyingButton();
};

ButtonAccessImpl::ButtonAccessImpl(Button *b)
    : button(b),
      m(),
      cv(),
      locked(false)
{
}

ButtonAccessImpl::~ButtonAccessImpl()
{
}

void ButtonAccessImpl::destroyingButton()
{
    {
        std::unique_lock l(m);
        while (locked) {
            cv.wait(l);
        }
        this->button = nullptr;
    }
}

std::shared_ptr<Button> ButtonAccessImpl::tryLock()
{
    std::unique_lock l(m);
    
    if (!button) return std::shared_ptr<Button>();

    locked = true;
    return std::shared_ptr<Button>(this->button, [this](Button *b) {
        std::unique_lock l2(m);
        locked = false;
        cv.notify_all();
    });
}

SButtonAccessSP SButtonAccess::create(Button* b)
{
    return SButtonAccessSP(new ButtonAccessImpl(b));
}
