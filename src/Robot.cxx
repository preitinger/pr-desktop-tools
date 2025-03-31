#include "Robot.H"

#include <thread>
#include <iostream>
#include <cassert>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace awt
{

    static int handleXError(Display *d, XErrorEvent *e)
    {
        char msg[1024];
        assert(d == e->display);
        XGetErrorText(e->display, e->error_code, msg, 1024);
        std::cerr << "XError: " << msg << std::endl;
        return 0; // ignored
    }

    Robot::Robot(Display *d)
    {
        display = d;
        XSetErrorHandler(&handleXError);
    }

    Robot::~Robot()
    {
        // XCloseDisplay(display);
    }

    void Robot::delay(uint_fast32_t ms)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }

    void Robot::mouseMove(int_fast32_t x, int_fast32_t y)
    {
        XWarpPointer(display, None, DefaultRootWindow(display), 0, 0, 0, 0, x, y);
        XFlush(display);
    }

    void Robot::mousePress(uint_fast32_t buttons)
    {
        XEvent event;
        memset(&event, 0x00, sizeof(event));
        event.type = ButtonPress;
        event.xbutton.button = buttons;
        event.xbutton.same_screen = True;
        int res = XQueryPointer(display, RootWindow(display, DefaultScreen(display)), &event.xbutton.root, &event.xbutton.window, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
        //std::cout << "res of XQueryPointer " << res << "\n";

        event.xbutton.subwindow = event.xbutton.window;

        while (event.xbutton.subwindow)
        {
            event.xbutton.window = event.xbutton.subwindow;

            res = XQueryPointer(display, event.xbutton.window, &event.xbutton.root, &event.xbutton.subwindow, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
            //std::cout << "res of XQueryPointer " << res << "\n";
        }

        res = XSendEvent(display, PointerWindow, True, 0xfff, &event);
        //std::cout << "res of send press " << res << "\n";
    }

    void Robot::mouseRelease(uint_fast32_t buttons)
    {
        XEvent event;
        memset(&event, 0x00, sizeof(event));
        event.xbutton.button = buttons;
        event.xbutton.same_screen = True;

        int res = XQueryPointer(display, RootWindow(display, DefaultScreen(display)), &event.xbutton.root, &event.xbutton.window, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
        //std::cout << "res of XQueryPointer " << res << "\n";

        event.xbutton.subwindow = event.xbutton.window;

        while (event.xbutton.subwindow)
        {
            event.xbutton.window = event.xbutton.subwindow;

            res = XQueryPointer(display, event.xbutton.window, &event.xbutton.root, &event.xbutton.subwindow, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
            //std::cout << "res of XQueryPointer " << res << "\n";
        }

        event.xbutton.type = ButtonRelease;

        res = XSendEvent(display, PointerWindow, True, 0xfff, &event);
        //std::cout << "res of send release " << res << "\n";
    }

    void Robot::mousePoint(int_fast32_t *x_return, int_fast32_t *y_return)
    {
    }

    void Robot::getPixelColor(int_fast32_t x, int_fast32_t y, uint_fast32_t *rgb_return)
    {
        XColor c;
        Display *d = display;

        XImage *image;
        image = XGetImage(d, XRootWindow(d, XDefaultScreen(d)), x, y, 10, 10, AllPlanes, XYPixmap);
        dump("bits_per_pixel", image->bits_per_pixel);
        dump("bytes_per_line", image->bytes_per_line);
        dump("depth", image->depth);
        dump("format", image->format);
        dump("XYBitmap", XYBitmap);
        dump("XYPixmap", XYPixmap);
        dump("ZPixmap", ZPixmap);

        for (int y1 = 0; y1 < 10; ++y1)
        {
            for (int x1 = 0; x1 < 10; ++x1)
            {
                c.pixel = XGetPixel(image, x1, y1);
                XQueryColor(d, XDefaultColormap(d, XDefaultScreen(d)), &c);
                //std::cout << c.red / 256 << " " << c.green / 256 << " " << c.blue / 256 << "     ";

                if (rgb_return)
                {
                    *rgb_return = (((uint_fast32_t)c.red >> 8) << 16) | (((uint_fast32_t)c.green >> 8) << 8) | (c.blue >> 8);
                }
            }
            //std::cout << std::endl;
        }

        XFree(image);
    }

    XImage *Robot::createScreenCapture(int_fast32_t x, int_fast32_t y, int_fast32_t w, int_fast32_t h)
    {
        Display *d = display;

        return XGetImage(d, XRootWindow(d, XDefaultScreen(d)), x, y, 10, 10, AllPlanes, XYPixmap);
    }

    /**
     * result must be deleted using fl_delete_offscreen();
     */
    Fl_Offscreen Robot::createOffscreen(int_fast32_t x, int_fast32_t y, int_fast32_t w, int_fast32_t h)
    {
        Fl_Offscreen o = fl_create_offscreen(w, h);
        fl_begin_offscreen(o);
 
        XColor c;
        Display *d = display;

        XImage *image;
        image = XGetImage(d, XRootWindow(d, XDefaultScreen(d)), x, y, w, h, AllPlanes, XYPixmap);
        dump("bits_per_pixel", image->bits_per_pixel);
        dump("bytes_per_line", image->bytes_per_line);
        dump("depth", image->depth);
        dump("format", image->format);
        dump("XYBitmap", XYBitmap);
        dump("XYPixmap", XYPixmap);
        dump("ZPixmap", ZPixmap);

        for (int y1 = 0; y1 < h; ++y1)
        {
            for (int x1 = 0; x1 < w; ++x1)
            {
                c.pixel = XGetPixel(image, x1, y1);
                XQueryColor(d, XDefaultColormap(d, XDefaultScreen(d)), &c);
                //std::cout << c.red / 256 << " " << c.green / 256 << " " << c.blue / 256 << "     ";
                fl_color(c.red / 256, c.green / 256, c.blue / 256);
                fl_point(x1, y1);
            }
            //std::cout << std::endl;
        }

        XFree(image);
        
        fl_end_offscreen();
        return o;
    }

}