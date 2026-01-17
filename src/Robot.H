#ifndef __ROBOT_H__
#define __ROBOT_H__

#include <iostream>
#include <cstdint>

#include <FL/fl_draw.H>
#include <X11/Xlib.h>

namespace awt
{

    template<class T>
    static void dump(const char* label, T val) {
        //std::cout << label << "  " << val << std::endl;
    }

    class Robot
    {
    private:
        Display *display;

    public:
        Robot(Display* d);
        virtual ~Robot();
        void delay(uint_fast32_t ms);
        void mouseMove(int_fast32_t x, int_fast32_t y);
        void mousePress(uint_fast32_t buttons);
        void mouseRelease(uint_fast32_t buttons);
        void mousePoint(int_fast32_t* x_return, int_fast32_t* y_return);
        void getPixelColor(int_fast32_t x, int_fast32_t y, uint_fast32_t* rgb_return);
        XImage* createScreenCapture(int_fast32_t x, int_fast32_t y, int_fast32_t w, int_fast32_t h);
        Fl_Offscreen createOffscreen(int_fast32_t x, int_fast32_t y, int_fast32_t w, int_fast32_t h);
    };

}


enum class ClickState
{
    MOVE,
    PRESS,
    RELEASE,
};

inline std::ostream &operator<<(std::ostream &o, const ClickState &s)
{
    switch (s)
    {
    case ClickState::MOVE:
        o << "MOVE";
        break;
    case ClickState::PRESS:
        o << "PRESS";
        break;
    case ClickState::RELEASE:
        o << "RELEASE";
        break;
    }
    return o;
}

#endif
