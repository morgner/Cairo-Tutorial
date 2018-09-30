#include "canvas.h"


bool CCanvas::on_draw(Cairo::RefPtr<Cairo::Context> const & cr)
    {
    cr->set_source_rgb(1.,.5,.0);
    cr->set_line_width(3);

    cr->move_to( 11, 11);
    cr->line_to(111,111);
    cr->stroke();

    return true;
    }
