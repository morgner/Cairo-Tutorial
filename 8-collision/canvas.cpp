#include "canvas.h"






int CCanvas::Collision(SPoint const & tPoint)
    {
    int i{0};
    for ( auto & a:m_vFlecken )
        {
        if ( Distance(a, tPoint) < a.r )
            {
            return std::move(i);
            }
        ++i;
        }
    return -1;
    }


bool CCanvas::on_button_press_event(GdkEventButton *event)
    {
    m_tMouseColor = { .0,.0,.9 };
    
    queue_draw();
    return true;
    }
    
bool CCanvas::on_motion_notify_event(GdkEventMotion *event)
    {
    m_tMousePos = SPoint{*event};

    queue_draw();
    return true;
    }
    
bool CCanvas::on_button_release_event(GdkEventButton* event)
    {
    m_tMouseColor = { .5,.5,.5 };
    m_vMouseTrail.emplace_back( SPoint{ *event } );

    queue_draw();
    return true;
    }
    
bool CCanvas::on_scroll_event(GdkEventScroll *event)
    {
    if ( event->delta_y>0 )
        m_tMouseColor = { .9,.0,.0 };
    else
        m_tMouseColor = { .0,.9,.0 };

    queue_draw();
    return true;
    }

bool CCanvas::on_draw(Cairo::RefPtr<Cairo::Context> const & cr)
    {
    cr->set_source_rgb( .0,.0,.0 );
    cr->set_line_width(3);
    if ( m_vMouseTrail.size() )
        {
        cr->move_to(m_vMouseTrail[0].x,m_vMouseTrail[0].y);
        for (auto const & a:m_vMouseTrail)
            {
            cr->line_to( a.x, a.y);
            }
        cr->stroke();
        }
    
    auto const ic{Collision(m_tMousePos)};
    int i{0};
    for ( auto const & a:m_vFlecken )
        {
        if ( ic == i++ )
            cr->set_source_rgb( .9, .0, .0 );
        else
            cr->set_source_rgb( .0, .9, .0 );
        cr->arc(a.x, a.y, a.r, 0, 2*M_PI);
        cr->fill();
        }

    
    cr->set_source_rgb( m_tMouseColor.r, m_tMouseColor.b, m_tMouseColor.b );
    cr->arc(m_tMousePos.x, m_tMousePos.y, 11, 0, 2*M_PI);
    cr->fill();

    return true;
    }
