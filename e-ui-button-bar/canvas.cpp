#include "canvas.h"
#include <pangomm/fontdescription.h>





SPoint draw_text(Cairo::RefPtr<Cairo::Context> const & cr,
                 double posx, double posy,
                 std::string const & crsText, double const & dScale = 1.0)
    {
    cr->save();

    Pango::FontDescription font;

    font.set_family("Monospace");
    font.set_absolute_size(12.0 * PANGO_SCALE*dScale);

    CCanvas w;
    auto layout = w.create_pango_layout(crsText);

    layout->set_font_description(font);
    int iWidth {0};
    int iHeight{0};
    SPoint tSize{.0,.0};

    layout->get_pixel_size(iWidth, iHeight);
    tSize = SPoint{iWidth, iHeight};
    cr->move_to(posx-tSize.x/2, posy-tSize.y/2);
    layout->show_in_cairo_context(cr);
    cr->restore();

    return std::move(tSize);
    }

bool CCanvas::Collision(SPoint const & tPoint)
    {
    m_tCollision.eWhat  = SCollision::EWhat::none;

    int i{0};
    for ( auto & a:m_vFlecken )
        {
        if ( Distance(a, tPoint) < a.r )
            {
            m_tCollision.tWhere  = tPoint;
            m_tCollision.tOffset = tPoint - a;
            m_tCollision.eWhat   = SCollision::EWhat::Fleck;
            m_tCollision.nIndex  = i;
            return std::move(true);
            }
        ++i;
        }
    return false;
    }


bool CCanvas::on_button_press_event(GdkEventButton *event)
    {
    for ( auto const & a:m_voButtons )
        {
        if ( a.Collision(*event) )
            {
            m_oButtonPressed = a.sFunct;
            return true;
            }
        }

    m_tMouseColor = { .0,.0,.9 };
    if (event->type == GDK_BUTTON_PRESS )
        {
        m_tEventPress = *event;
        m_tShiftStart = m_tShift;
        }
    else
        {
        auto const bCol { Collision(m_tMousePos) };
        }

    if ( event->button == 3 )
        {
        switch ( m_tCollision.eWhat  )
            {
            case SCollision::EWhat::Fleck:
                break;
            case SCollision::EWhat::Line:
                break;
            case SCollision::EWhat::none:
                break;
            }
        }

    queue_draw();
    return true;
    }
    
bool CCanvas::on_motion_notify_event(GdkEventMotion *event)
    {
    m_tMousePos = (*event - m_tShift)/m_dScale;

    if ( m_oButtonPressed > "") return true;
    
    if ( event->type & GDK_MOTION_NOTIFY )
        if ( event->state & GDK_BUTTON3_MASK )
            {
            switch ( m_tCollision.eWhat  )
                {
                case SCollision::EWhat::Fleck:
                    m_vFlecken[m_tCollision.nIndex] = m_tMousePos
                                                    - m_tCollision.tOffset;
                    break;
                case SCollision::EWhat::Line:
                    break;
                case SCollision::EWhat::none:
                    m_tShift = m_tShiftStart - (m_tEventPress - *event);
                    break;
                }           
            }
        else
            {
            auto const bCol { Collision(m_tMousePos) };
            }

    queue_draw();
    return true;
    }

std::string s{"X"};
bool CCanvas::on_button_release_event(GdkEventButton* event)
    {
    if ( m_oButtonPressed.size() > 0 )
        {
        m_oButtonPressed = "";
        for ( auto const & a:m_voButtons )
            if ( a.Collision(*event) )
                {
                m_oButtonPressed = a.sFunct;
                break;
                }
        if ( m_oButtonPressed == "0" ) { s="A"; }
        if ( m_oButtonPressed == "1" ) { s="B"; }
        if ( m_oButtonPressed == "2" ) { s="C"; }
        if ( m_oButtonPressed == "3" ) { s="D"; }
        if ( m_oButtonPressed == "4" ) { s="E"; }
        if ( m_oButtonPressed == "5" ) { s="F"; }
        if ( m_oButtonPressed == "6" ) { s="G"; }

        m_oButtonPressed="";
        queue_draw();
        return true;
        }

    if ( event->type & GDK_MOTION_NOTIFY )
        if ( event->state & GDK_BUTTON1_MASK )
            {
            m_tMouseColor = { .5,.5,.5 };
            m_vMouseTrail.emplace_back( (*event - m_tShift)/m_dScale );
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

    SPoint const p0{ (*event - m_tShift)/m_dScale };
    m_dScale *= (event->delta_y>0)?.9:1.1; if (m_dScale<.01) m_dScale=.01;
    SPoint const p1{ (*event - m_tShift)/m_dScale };
    m_tShift -= (p0-p1)*m_dScale;

    queue_draw();
    return true;
    }

bool CCanvas::on_draw(Cairo::RefPtr<Cairo::Context> const & cr)
    {
    auto const all        { get_allocation() };
    auto const m_tCtxSize { SPoint { (double)all.get_width(),
                                     (double)all.get_height() } };

    static auto tHome{ SPoint { m_tCtxSize }/2 };

    if ( m_bShiftInit )
        {
        tHome = m_tShift = m_tCtxSize/2;
        m_bShiftInit = false;
        }
    auto const tSizeHalf{m_tCtxSize/2};
    if ( tHome != tSizeHalf )
        {
        m_tShift -= tHome - tSizeHalf; tHome = tSizeHalf;
        }

    Cairo::Matrix matrix(1.0, 0.0, 0.0, 1.0, 0.0, 0.0);
    matrix.scale(m_dScale,m_dScale);
    matrix.translate(m_tShift.x/m_dScale, m_tShift.y/m_dScale);

    cr->transform(matrix);



    if ( auto const i{m_vMouseTrail.size()} )
        {
        cr->set_source_rgb( .0,.0,.0 );
        cr->set_line_width(3);
        cr->move_to(m_vMouseTrail[0].x,m_vMouseTrail[0].y);
        for (auto const & a:m_vMouseTrail)
            {
            cr->line_to( a.x, a.y);
            }
        cr->stroke();

        cr->set_source_rgb( .6,.6,.6 );
        cr->set_line_width(1);
        cr->move_to(m_vMouseTrail[i-1].x,m_vMouseTrail[i-1].y);
        cr->line_to( m_tMousePos.x, m_tMousePos.y );
        cr->stroke();
        }

    int i{0};
    for ( auto const & a:m_vFlecken )
        {
        if ( ( m_tCollision.nIndex == i++ ) &&
             ( m_tCollision.eWhat == SCollision::EWhat::Fleck ) )
            cr->set_source_rgb( .9, .0, .0 );
        else
            cr->set_source_rgb( .0, .9, .0 );
        if ( i == 1 )
            cr->arc(a.x + 250*m_dAnimatorBi, a.y, a.r, 0, 2*M_PI);
        else if ( i == 2 )
            cr->arc(a.x, a.y - sin(2*M_PI*m_dAnimatorRot)*125, a.r, 0, 2*M_PI);
        else
            cr->arc(a.x, a.y, a.r, 0, 2*M_PI);
        cr->fill();
        
        cr->set_source_rgb( .0, .2, .0 );
        cr->set_line_width(1);
        cr->arc(a.x, a.y, a.r, 0, 2*M_PI);
        cr->stroke();
        }

    
    cr->set_source_rgb( m_tMouseColor.r, m_tMouseColor.b, m_tMouseColor.b );
    cr->arc(m_tMousePos.x, m_tMousePos.y, 11, 0, 2*M_PI);
    cr->fill();

    cr->set_source_rgb( .5, .5, .5 );
    draw_text(cr, sin(2*M_PI*m_dAnimatorRot)*75, -60, "Informational text submission", 4);


    // Buttons
    draw_text(cr,  -150, -120, s, 8/m_dScale);
    i=0;
    SPoint const tMousePos{m_tMousePos*m_dScale+m_tShift};
    for ( auto const & a:m_voButtons )
        {
        ++i;
        cr->set_line_width(1/m_dScale);

        if ( a.Collision(tMousePos) )
            {
            cr->set_source_rgba(0,1,0,.8);
            }
        else
            {
            cr->set_source_rgba(.7,.7,.7,.8);
            }

        cr->rectangle( (a.x-m_tShift.x)/m_dScale, (a.y-m_tShift.y)/m_dScale, (a.w)/m_dScale, (a.h)/m_dScale );
        cr->fill();
        cr->set_source_rgb(.2,.2,.2);
        cr->rectangle( (a.x-m_tShift.x)/m_dScale, (a.y-m_tShift.y)/m_dScale, (a.w)/m_dScale, (a.h)/m_dScale );
        cr->stroke();

        cr->set_source_rgb(0,0,0);
        draw_text(cr,  (a.x+a.w/2-m_tShift.x)/m_dScale, (a.y+a.h/2-m_tShift.y)/m_dScale, a.sTitle, 1/m_dScale);
        }

    return true;
    }
