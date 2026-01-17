#include "TestWidgetAccess.H"
#include "WidgetAccess.H"
#include "Callback.H" // TODO really?

#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/fl_callback_macros.H>

#include <iostream>

#include <cassert>

// WidgetAccessSP<Fl_Button> testButtonAccess = WidgetAccess<Fl_Button>::create(0, 0, 0, 0);
// auto testLambda = []() { std::cout << "testAction\n"; };
// std::shared_ptr<std::function<void()>> testAction(new std::function<void()>(testLambda));

// CallbackSP<Fl_Button> test = Callback<Fl_Button>::create(testButtonAccess, testAction);

TestWidgetAccess::TestWidgetAccess(Fl_Window *window, const OnFinish &onFinish)
    : window(window),
      onFinish(onFinish),
      output(),
      exit(),
      deleteOutput(),
      setRandomOutput(),
      setRandomOutputAction(),
      setRandomOutputCb()
{
    window->clear();
    window->begin();
    output = WidgetAccess<Fl_Output>::create(200, 0, 200, 20, "Test Output");
    exit = WidgetAccess<Fl_Button>::create(200, 20, 200, 20, "Exit");
    exitCb = Callback<Fl_Button>::create(exit, (exitAction = createAction(&TestWidgetAccess::onExit)));
    deleteOutput = WidgetAccess<Fl_Button>::create(200, 40, 200, 20, "Delete output");
    this->deleteAction = std::make_shared<std::function<void()>>([this]() {
        std::cout << "output->get() " << this->output->get() << "\n";
        delete output->get();
        this->window->redraw();

    });
    std::cout << "deleteAction: " << deleteAction << "\n";
    deleteCb = Callback<Fl_Button>::create(deleteOutput, deleteAction);
    setRandomOutput = WidgetAccess<Fl_Button>::create(200, 60, 200, 20, "Set random output");
    setRandomOutputAction = createAction(&TestWidgetAccess::onSetRandomOutput);
    setRandomOutputCb = Callback<Fl_Button>::create(setRandomOutput, setRandomOutputAction);
    window->end();
    window->redraw();


    // // TODO BEGIN test
    // setCallback(&TestWidgetAccess::onExit);
    // // TODO END test
}

void TestWidgetAccess::onExit()
{
    delete this;
    std::cout << "leave TestWidgetAccess::onExit()\n";
}

void TestWidgetAccess::onSetRandomOutput()
{
    auto lockedOutput = output->get();
    if (lockedOutput) {
        lockedOutput->value(std::rand());
        std::cout << "Output set to random value\n";
    } else {
        std::cout << "Warning: output already deleted!\n";
    }
}


TestWidgetAccess::~TestWidgetAccess()
{
    std::cout << "~TestWidgetAccess\n";

    // aus Spass und zum Testen callbacks erst resetten
    deleteCb.reset();
    setRandomOutputCb.reset();
    exitCb.reset();

    // nun auch noch access-Objekte rest resetten
    deleteOutput.reset();
    setRandomOutput.reset();
    exit.reset();

    if (onFinish) {
        std::cout << "before onFinish\n";
        onFinish();
        std::cout << "nach onFinish\n";
    }
}