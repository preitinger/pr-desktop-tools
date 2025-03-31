#ifndef __DBG_BUTTON_H__
#define __DBG_BUTTON_H__

#include <FL/Fl_Button.H>
#include <iostream>

class DbgButton : public Fl_Button
{
public:
    DbgButton(int X, int Y, int W, int H, const char *L)
        : Fl_Button(X, Y, W, H, L)
    {
    }
    virtual ~DbgButton()
    {
        std::cout << "~DbgButton()\n";
    }
};


#endif
