#include "TestWidgetAccess.H"
#include "WidgetAccess.H"

#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/fl_callback_macros.H>

#include <iostream>

#include <cassert>

TestWidgetAccess::TestWidgetAccess(Fl_Window *window, const OnFinish &onFinish)
    : window(window),
      onFinish(onFinish),
      output(),
      exit(),
      deleteOutput()
{
    window->clear();
    window->begin();
    output = WidgetAccess<Fl_Output>::create(200, 0, 200, 20, "Test Output");
    exit = WidgetAccess<Fl_Button>::create(200, 20, 100, 20, "Exit");
    Fl_Button *exitButton = exit->get();
    assert(exitButton);
    FL_METHOD_CALLBACK_0(exitButton, TestWidgetAccess, this, onExit);
    deleteOutput = WidgetAccess<Fl_Button>::create(200, 40, 100, 20, "Delete output");
    Fl_Button *deleteOutputButton = deleteOutput->get();
    assert(deleteOutputButton);
    FL_INLINE_CALLBACK_3(deleteOutputButton, WidgetAccessSP<Fl_Output>, output, output, Fl_Window*, window, window, std::ostream&, cout, std::cout, {
        cout << "output->get() " << output->get() << "\n";
        delete output->get();
        window->redraw();
    });

    window->end();
    window->redraw();
}

void TestWidgetAccess::onExit()
{
    delete this;
}

TestWidgetAccess::~TestWidgetAccess()
{
    std::cout << "~TestWidgetAccess\n";

    if (onFinish)
        onFinish();
}