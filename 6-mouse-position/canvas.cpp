#include "canvas.h"






bool CCanvas::on_motion_notify_event(GdkEventMotion *event)
    {
    m_tMousePos = SPoint{*event};
    queue_draw();
    return true;
    }

bool CCanvas::on_draw(Cairo::RefPtr<Cairo::Context> const & cr)
    {
    Gtk::Allocation allocation{ get_allocation() };
    auto const width { (double)allocation.get_width() };
    auto const height{ (double)allocation.get_height() };

    cr->set_source_rgb(1.,.5,.0);
    cr->set_line_width(3);

    // line crossing the whole window
    cr->move_to(    0,      0);
    cr->line_to(width, height);
    cr->stroke();

    // circle gray
    cr->set_source_rgb(.7,.7,.0);
    cr->arc(width/2, height/2, 100, 0, 2*M_PI);
    cr->fill();

    // picture png quadratic
    // - load picture
    static Glib::RefPtr<Gdk::Pixbuf> const image  = Gdk::Pixbuf::create_from_file("../CairoTut.svg");
    // - scale picture to destination size
    static Glib::RefPtr<Gdk::Pixbuf>       imageS = image->scale_simple( 180, 180, Gdk::INTERP_BILINEAR);
    // - place scaled pictures to specified position in render context
    Gdk::Cairo::set_source_pixbuf(cr, imageS, width/2-90, height/2-90 );
    // - open a hole for the pixels
    cr->rectangle( width/2-90, height/2-90, 180, 180 );
    // - show the hole
    cr->fill();

    
    
    cr->set_source_rgb(.0,.0,.9);
    cr->arc(m_tMousePos.x, m_tMousePos.y, 3, 0, 2*M_PI);
    cr->fill();

    return true;
    }
