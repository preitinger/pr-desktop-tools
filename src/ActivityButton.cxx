#include "ActivityButton.H"
#include <FL/fl_callback_macros.H>
#include <iostream>


ActivityButton::ActivityButton(int x, int y, const char *label) : Button(x, y, 150, 20, label), activity(activity)
{
}

void ActivityButton::setActivity(const SActivitySP& activity) {
    this->activity = activity;
}

ActivityButton::~ActivityButton()
{
    std::cout << "~ActivityButton\n";
}

void ActivityButton::onClick()
{
    auto a = activity.lock();

    if (a) {
        a->start();
    }
}