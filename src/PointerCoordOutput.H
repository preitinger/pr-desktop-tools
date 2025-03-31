#ifndef __POINTER_COORD_OUTPUT_H__
#define __POINTER_COORD_OUTPUT_H__

#include <FL/Fl_Widget.H>
#include <FL/fl_draw.H>

#include <thread>
#include <iostream>
#include <atomic>

class PointerCoordOutput : public Fl_Widget
{
private:
    std::atomic<int> cx, cy;
    std::atomic<bool> stopped;
    std::thread t;

public:
    PointerCoordOutput(int x, int y) : Fl_Widget(x, y, 200, 20), cx(-1), cy(-1), stopped(false), t()
    {
        t = std::thread(&PointerCoordOutput::testThread, this);
    }

    ~PointerCoordOutput()
    {
        stopped = true;
        t.join();
    }

    void set(int x, int y)
    {
        // std::cout << "set: vor lock\n";
        // Fl::lock();
        // std::cout << "set: nach lock\n";
        Fl::lock();
        this->cx = x;
        this->cy = y;
        Fl::unlock();
        redraw();
        std::cout << "triggered redraw\n";
        Fl::awake();
    }

    void draw()
    {
        std::cout << "draw()\n";
        fl_rectf(x(), y(), w(), h(), 0);
        fl_color(1);
        char test[256];
        snprintf(test, 256, "Pointer: (%d, %d)", cx.load(), cy.load());
        std::cout << "test: '" << test << "'\n";
        fl_draw(test, x(), y() + 15);
    }

private:
    void testThread()
    {

        while (!stopped)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            int mx, my;
            Fl::get_mouse(mx, my);
            set(mx, my);
        }
    }
};

#endif