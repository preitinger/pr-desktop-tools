#ifndef __TRACK_MOUSE_H__
#define __TRACK_MOUSE_H__

#include "OnFinish.H"

class Fl_Window;
class Fl_Output;
class Fl_Button;

class TrackMouse
{
public:
    /**
     * startet eine CountDown-Activity.
     * Widgets werden wie benoetigt in Window hinzugefuegt, welches zu Beginn geleert wird.
     * Nach Ablauf der Activity wird das Objekt zerstoert und im Destruktor onFinish() aufgerufen, falls onFinish nicht leer ist.
     */
    TrackMouse(Fl_Window *window, const OnFinish &onFinish);
    virtual ~TrackMouse();
    /**
     * @return true iff activity has stopped.
     */
    void onTimeout();

private:
    void cancel();

private:
    Fl_Window *window;
    OnFinish onFinish;
    Fl_Output *output;
    Fl_Button *cancelButton;
};

#endif // __TRACK_MOUSE_H__
