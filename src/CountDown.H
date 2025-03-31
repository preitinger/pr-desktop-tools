#ifndef __COUNT_DOWN_H__
#define __COUNT_DOWN_H__

#include "OnFinish.H"

#include <FL/Fl.H>

#include <functional>

class Fl_Window;
class Fl_Int_Input;
class Fl_Float_Input;
class Fl_Output;
class Fl_Button;

class CountDown
{
public:
    /**
     * startet eine CountDown-Activity.
     * Widgets werden wie benoetigt in Window hinzugefuegt, welches zu Beginn geleert wird.
     * Nach Ablauf der Activity wird das Objekt zerstoert und im Destruktor onFinish() aufgerufen, falls onFinish nicht leer ist.
     */
    CountDown(Fl_Window *window, const OnFinish &onFinish);
    virtual ~CountDown();
    /**
     * If count-down has finished, deletes itself
     */
    void onTimeout();

private:
    void start();
    void cancel();
    void decrement();

private:
    Fl_Window *window;
    OnFinish onFinish;
    int rest;
    float step;
    Fl_Int_Input *countInput;
    Fl_Float_Input *stepInput;
    Fl_Button *startButton;
    Fl_Output *output;
    Fl_Button *cancelButton;
};

#endif
