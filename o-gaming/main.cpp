#include "canvas.h"
#include <gtkmm/application.h>
#include <gtkmm/window.h>


int main(int argc, char** argv)
    {
    auto app = Gtk::Application::create(argc, argv, "org.gtkmm.cairo.tut");

    Gtk::Window window;
//  window.resize(800,600);
    window.fullscreen();
    window.set_title("Cairo tutorial C++");

    CCanvas area;
    window.add(area);
    area.show();

    return app->run(window);
    }

