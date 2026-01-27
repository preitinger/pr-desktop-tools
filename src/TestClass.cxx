#include "TestClass.H"

#include <iostream>

#include <FL/Fl_Input.H>
#include <FL/Fl_Group.H>

TestClass::TestClass(BaseActionWP &&parent, const WidgetAccessSP<Fl_Group> &group)
    : BaseAction(std::move(parent)),
      group(group),
      cancelB(),
      nameInput(),
      nextTimeout()
{
    std::cout << "TestClass(&&)\n";
}

// virtual
TestClass::~TestClass()
{
}

void TestClass::start() /* override final */
{
    std::cout << "BEGIN TestClass::start\n";
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
        auto innerGroup = widget<TestClass, Fl_Group>(x, y += 10, w, gh - 30, "GenActionCode", nullptr);
        innerGroup->get()->box(Fl_Boxtype::FL_DOWN_BOX);
        x += 10;
        w -= 20;
        y += 10;

        cancelB = button(x, y, w, h, "Cancel", &TestClass::onCancel);
        // typedef void (TestClass::*MyMethodPtr)();
        nameInput = widget<TestClass, Fl_Input>(x + 100, y += lh, w - 100, h, "Klassenname", &TestClass::onInput);
        nameInput->get()->when(FL_WHEN_ENTER_KEY | FL_WHEN_NOT_CHANGED);
        innerGroup->get()->end();
        std::cout << "end TestClass::start\n";
    }
}

void TestClass::onCancel()
{
    exit();
}

void TestClass::onInput()
{
    // TODO your action on callback to nameInput
}

void TestClass::onTimeout()
{

    // TODO your timer action
    // maybe set next timer
    double seconds = 0.5;
    nextTimeout = timeout(seconds, &TestClass::onTimeout);
}
