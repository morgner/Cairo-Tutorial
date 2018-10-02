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

    if( (event->type == GDK_BUTTON_PRESS) && (event->button == 3) )
        {
        m_tEventPress = *event;
        m_tShiftStart = m_tShift;
        }
    
    queue_draw();
    return true;
    }
    
bool CCanvas::on_motion_notify_event(GdkEventMotion *event)
    {
    m_tMousePos = SPoint{*event - m_tShift};

    if ( event->type & GDK_MOTION_NOTIFY )
        if ( event->state & GDK_BUTTON3_MASK )
            {
            m_tShift = m_tShiftStart - (m_tEventPress - *event);
            }

    queue_draw();
    return true;
    }
    
bool CCanvas::on_button_release_event(GdkEventButton* event)
    {
    if ( event->type & GDK_MOTION_NOTIFY )
        if ( event->state & GDK_BUTTON1_MASK )
            {
            m_tMouseColor = { .5,.5,.5 };
            m_vMouseTrail.emplace_back( SPoint{ *event - m_tShift } );
            }
        if ( event->state & GDK_BUTTON3_MASK )
            {
            }

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
    auto const all   { get_allocation() };
    auto const tSize { SPoint { (double)all.get_width(),
                                (double)all.get_height() } };

    static auto tHome{ SPoint { tSize }/2 };

    if ( m_bShiftInit )
        {
        tHome = m_tShift = tSize/2;
        m_bShiftInit = false;
        }
    auto const tSizeHalf{tSize/2};
    if ( tHome != tSizeHalf )
        {
        m_tShift -= tHome - tSizeHalf; tHome = tSizeHalf;
        }

    Cairo::Matrix matrix(1.0, 0.0, 0.0, 1.0, 0.0, 0.0);
    matrix.translate(m_tShift.x, m_tShift.y);

    cr->transform(matrix);


        
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
        if ( ic == i++)
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
