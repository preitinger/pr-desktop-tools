#include "BlaBlubb.H"

#include <iostream>

#include <FL/Fl_Input.H>
#include <FL/Fl_Group.H>

BlaBlubb::BlaBlubb(BaseActionWP &&parent, const WidgetAccessSP<Fl_Group> &group)
    : BaseAction(std::move(parent)),
      group(group),
      cancelB(),
      nameInput(),
      nextTimeout()
{
    std::cout << "BlaBlubb(&&)\n";
}

// virtual
BlaBlubb::~BlaBlubb()
{
}

void BlaBlubb::start() /* override final */
{
    std::cout << "BEGIN BlaBlubb::start\n";
    assert(group);
    if (auto g = group->get())
    {
        int gx = g->x();
        int gy = g->y();
        int gw = g->w();
        int gh = g->h();
        if (g->as_window())
        {
            gx = 0;
            gy = 0;
        }
        int x = gx + 10;
        int y = gy + 10;
        int w = gw - 20;
        int h = 20;
        int lh = h + 5;
        auto innerGroup = widget<BlaBlubb, Fl_Group>(x, y += 10, w, gh - 30, "GenActionCode", nullptr);
        innerGroup->get()->box(Fl_Boxtype::FL_DOWN_BOX);
        x += 10;
        w -= 20;
        y += 10;

        cancelB = button(x, y, w, h, "Cancel", &BlaBlubb::onCancel);
        // typedef void (BlaBlubb::*MyMethodPtr)();
        nameInput = widget<BlaBlubb, Fl_Input>(x + 100, y += lh, w - 100, h, "Klassenname", &BlaBlubb::onInput);
        nameInput->get()->when(FL_WHEN_ENTER_KEY | FL_WHEN_NOT_CHANGED);
        innerGroup->get()->end();
        std::cout << "end BlaBlubb::start\n";
    }
}

void BlaBlubb::onCancel()
{
    exit();
}

void BlaBlubb::onInput()
{
    // TODO your action on callback to nameInput
}

void BlaBlubb::onTimeout()
{

    // TODO your timer action
    // maybe set next timer
    double seconds = 0.5;
    nextTimeout = timeout(seconds, &BlaBlubb::onTimeout);
}
