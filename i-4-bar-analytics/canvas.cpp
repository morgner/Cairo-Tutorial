#include "canvas.h"

#include "4bar.h"
#include "typesNmath.h"

#include <memory>
#include <pangomm/fontdescription.h>


C4Bar g_o3LS;


void CCanvas::AdjustAnimationToMechanism()
    {
    g_o3LS.A1 = g_o3LS.A1m; g_o3LS.B1 = g_o3LS.B1m;
    auto const w{ VectorSlope(g_o3LS.A0, g_o3LS.A1) };
    m_dAnimator = w/(2*M_PI)+.75;
    if ( m_dAnimator >= 1 ) m_dAnimator -= 1;
    }


SPoint draw_text(CairoCtx cr,
                 SPoint const & pos,
                 std::string const & crsText, 
                 double const & dScale,
                 bool const & label)
    {
    cr->save();

    Pango::FontDescription font;

    font.set_family("Sans");
    font.set_absolute_size(12.0 * PANGO_SCALE*dScale);

    CCanvas w;
    auto layout = w.create_pango_layout(crsText);

    layout->set_font_description(font);
    int iWidth {0};
    int iHeight{0};
    SPoint tSize{.0,.0};

    layout->get_pixel_size(iWidth, iHeight);
    tSize = SPoint{iWidth, iHeight};
    
    if (label)
        {
        cr->save();
        Color(cr, WHITE, .75);
        LineWidth(cr, {.0+iHeight});
        Line(cr, {{pos.x-tSize.x/2+tSize.y/4, pos.y},
                  {pos.x+tSize.x/2-tSize.y/4, pos.y}});    
        cr->restore();
        }
    
    cr->move_to(pos.x-tSize.x/2, pos.y-tSize.y/2);
    layout->show_in_cairo_context(cr);
    cr->restore();
    
    return std::move(tSize);
    }

bool CCanvas::Collision(SPoint const & tPoint)
    {
    m_tCollision = g_o3LS.Collision(tPoint);
    
/*
    m_tCollision.eWhat  = SCollision::EWhat::none;

    int i{0};
    for ( auto const & a:m_vFlecken )
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
*/
    return m_tCollision.eWhat != SCollision::EWhat::none;
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

    if (event->type == GDK_BUTTON_PRESS )
        {
        m_tEventPress = *event;
        m_tShiftStart = m_tShift;
        }
    else
        {
        auto const bCol { Collision(m_tMousePos) };
        }

    m_bButtonDown = true;

    queue_draw();
    return true;
    }
    
bool CCanvas::on_motion_notify_event(GdkEventMotion *event)
    {
    m_tMousePos = (*event - m_tShift)/m_dScale;

    if ( m_oButtonPressed > "") return true;
    
    if ( event->type & GDK_MOTION_NOTIFY )
        {
        if ( event->state & GDK_BUTTON3_MASK )
            {
            if ( m_tCollision.eWhat == SCollision::EWhat::none )
                m_tShift = m_tShiftStart - (m_tEventPress - *event);
            }
        else if ( event->state & GDK_BUTTON1_MASK )
            {
            if ( m_tCollision.eWhat != SCollision::EWhat::none )
                g_o3LS.MoveObject(m_tCollision, m_tMousePos);
            }
        else
            {
            auto const bCol { Collision(m_tMousePos) };
            }
        }

    queue_draw();
    return true;
    }

std::string s{"X"};
bool CCanvas::on_button_release_event(GdkEventButton* event)
    {
    if ( m_oButtonPressed.size() > 0 )
        {
        if ( event->state & GDK_BUTTON1_MASK )
            {
            m_oButtonPressed = "";
            for ( auto const & a:m_voButtons )
                if ( a.Collision(*event) )
                    {
                    m_oButtonPressed = a.sFunct;
                    break;
                    }         
            if ( m_oButtonPressed == "0" ) {      AnimateHlt(); g_o3LS.m_bCalculate = m_bAnimate;} 
            if ( m_oButtonPressed == "1" )        AnimateAdd(); 
            if ( m_oButtonPressed == "2" )        AnimateSub(); 
            if ( m_oButtonPressed == "3" ) {      AnimateRev(); g_o3LS.m_bAnimReverse = m_bAnimateLeft; }
            if ( m_oButtonPressed == "4" ) g_o3LS.TraceReset();  
            if ( m_oButtonPressed == "5" ) g_o3LS.TraceInvrt();
            if ( m_oButtonPressed == "6" ) g_o3LS.Durchschlagen();
            if ( m_oButtonPressed == "7" ) g_o3LS.Rotate();      
            if ( m_oButtonPressed == "8" ) g_o3LS.WithText();    
            if ( m_oButtonPressed == "9" ) g_o3LS.WithHints();   
            if ( m_oButtonPressed =="11" ) g_o3LS.WithBlink();   
            if ( m_oButtonPressed =="12" ) g_o3LS.WithMouse();
            }
        m_oButtonPressed="";
        queue_draw();
        return true;
        }

    if ( event->type & GDK_MOTION_NOTIFY )
        if ( event->state & GDK_BUTTON1_MASK )
            {
            }
        if ( event->state & GDK_BUTTON3_MASK )
            {
            }

    m_bButtonDown = false;
    AdjustAnimationToMechanism();

    queue_draw();
    return true;
    }
    
bool CCanvas::on_scroll_event(GdkEventScroll *event)
    {
    SPoint const p0{ (*event - m_tShift)/m_dScale };
    m_dScale *= (event->delta_y>0)?.9:1.1; if (m_dScale<.01) m_dScale=.01;
    SPoint const p1{ (*event - m_tShift)/m_dScale };
    m_tShift -= (p0-p1)*m_dScale;

    queue_draw();
    return true;
    }



bool CCanvas::on_draw(CairoCtx cr)
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

    // adj the output context
    Cairo::Matrix matrix(1.0, 0.0, 0.0, 1.0, 0.0, 0.0);
    matrix.scale(m_dScale,m_dScale);
    matrix.translate(m_tShift.x/m_dScale, m_tShift.y/m_dScale);
    cr->transform(matrix);

    cr->set_line_cap(Cairo::LINE_CAP_ROUND);

    // main content

    if (m_tCollision.eWhat != SCollision::EWhat::none)
        {
        Color(cr, RED, .5);
        Circle(cr, m_tMousePos - m_tCollision.tOffset, 17);
        }

    g_o3LS.Show(cr, m_dScale);
    if ( m_bButtonDown )
        {
        g_o3LS.DrawGetriebe(cr, m_dAnimator, m_dAnimate);
        }
    else
        {
        g_o3LS.CalcGetriebe(cr, m_dAnimator, m_dAnimate);
        }

    // text output
    Color(cr, GRAY);
//    draw_text(cr, {sin(2*M_PI*m_dAnimatorRot)*75, -60.0}, "Informational text submission", 4);


    // Buttons
    
    int i=0;
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

        Rectangle( cr, (a-m_tShift)/m_dScale, SPoint{a.w/m_dScale, a.h/m_dScale} );
        cr->set_source_rgb(.2,.2,.2);
        Frame( cr, (a-m_tShift)/m_dScale, SPoint{a.w/m_dScale, a.h/m_dScale} );

        cr->set_source_rgb(0,0,0);
        draw_text(cr,  {(a.x+a.w/2-m_tShift.x)/m_dScale, 
                        (a.y+a.h/2-m_tShift.y)/m_dScale}, a.sTitle, 1/m_dScale);
        }

    return true;
    }
