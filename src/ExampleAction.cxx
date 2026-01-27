#include "ExampleAction.H"

#include <FL/Fl_Group.H>
#include <FL/Fl_Output.H>

#include <iostream>

#include <cassert>

ExampleAction::ExampleAction(const std::weak_ptr<BaseAction> &parent, const WidgetAccessSP<Fl_Group> &group)
    : BaseAction(parent),
      group(group),
      exitB(),
      enableDisableB(),
      actionB(),
      numActionsO(),
      count(0)
{
}

void ExampleAction::start()
{
    std::cout << "ExampleAction::start()\n";
    int gx = group->get()->x();
    int gy = group->get()->y();
    int gw = group->get()->w();
    // int gh = group->get()->h();
    exitB = button(gx + 10, gy + 10, gw - 20, 20, "Exit Example Action", &ExampleAction::exit);
    enableDisableB = button(gx + 10, gy + 30, gw - 20, 20, "Enable Action", &ExampleAction::enableAction);
    numActionsO = WidgetAccess<Fl_Output>::create(200, gy + 70, gw - 200 - 10, 20, "# Actions");
    numActionsO->get()->value(count);
    testTimeout = timeout(0.5, &ExampleAction::onTestTimeout);
}

// virtual
ExampleAction::~ExampleAction()
{
    std::cout << "~ExampleAction()\n";
}

void ExampleAction::enableAction()
{
    if (!actionB)
    {
        std::cout << "!actionB\n";
        Fl_Group *w = group->get();
        assert(w);
        w->begin();
        int gx = group->get()->x();
        int gy = group->get()->y();
        int gw = group->get()->w();
        // int gh = group->get()->h();
        actionB = button(gx + 10, gy + 50, gw - 20, 20, "Do Action", &ExampleAction::doAction);
        w->end();
        w->redraw();
    }
    else
    {
        actionB->callback(&ExampleAction::doAction);
    }
    enableDisableB->get()->label("Disable Action");
    enableDisableB->callback(&ExampleAction::disableAction);
}

void ExampleAction::disableAction()
{
    Fl_Button *b = actionB->get();
    assert(b);
    delete b;
    std::cout << "before actionB.reset\n";
    // actionB->callback<ExampleAction>(nullptr);
    actionB.reset();
    std::cout << "after actionB.reset\n";
    enableDisableB->get()->label("Enable Action");
    enableDisableB->callback(&ExampleAction::enableAction);

    Fl_Group *g = group->get();
    assert(g);
    g->redraw();
}

void ExampleAction::doAction()
{
    ++count;
    assert(numActionsO);
    Fl_Output *o = numActionsO->get();
    assert(o);
    o->value(count);
}

void ExampleAction::onTestTimeout()
{
    std::cout << "ExampleAction::onTestTimeout\n";
    testTimeout = timeout(2, &ExampleAction::onTestTimeout);
}