#include "canvas.h"

#include "typesNmath.h"

#include <iostream>
#include <memory>
#include <pangomm/fontdescription.h>







bool CCanvas::Collision(SPoint const & tPoint)
    {
    CCanvas::SCollision tCollision;
    m_tCollision.eWhat = CCanvas::SCollision::EWhat::none;
    m_tCollision.d = 1e9;
    
    int i = 0;
    for ( auto const & a:m_vDrawing)
        {
        if ( double d = Distance(a.a, tPoint); d < m_tCollision.d )
            {
            m_tCollision.d       = d;
            m_tCollision.nIndex  = i;
            m_tCollision.nSubIx  = 0;
            m_tCollision.tWhere  = a.a;
            m_tCollision.tOffset = tPoint - a.a;
            }
        if ( double d = Distance(a.b, tPoint); d < m_tCollision.d )
            {
            m_tCollision.d       = d;
            m_tCollision.nIndex  = i;
            m_tCollision.nSubIx  = 2;
            m_tCollision.tWhere  = a.b;
            m_tCollision.tOffset = tPoint - a.b;
            }
        ++i;
        }

    if ( m_tCollision.d < 1e9 )
        {
        m_tCollision.eWhat = CCanvas::SCollision::EWhat::Element;
        }

    if ( m_bButtonDown )
        {
        double  dDmin{1e9};
        int i = 0;
        for ( auto const & a:m_vDrawing)
            {
            
            if ( double dW = abs(VectorAngleDiff(a, {m_oStartPoint, m_tMousePos}) ); dW < .1 || abs(dW-M_PI) < .1 )
                {
                SPoint iS = Intersection(a, SLine{{m_tMousePos.x,
                                                   m_tMousePos.y}, 
                                                  {m_tMousePos.x+sin(dW+M_PI/2)*100, 
                                                   m_tMousePos.y+cos(dW+M_PI/2)*100}} );

                if ( double dD = Distance(m_tMousePos, iS); dD < dDmin )
                    {
                    dDmin = dD;
                    m_tCollision.d      = dW;
                    m_tCollision.dAngle = (dW >.1) ? VectorSlope(a.b,a.a) : VectorSlope(a.a,a.b);
                    m_tCollision.nIndex = i;
                    m_tCollision.eWhat  = SCollision::EWhat::Line;
                    }
                }
            ++i;
            }
        }


    return m_tCollision.eWhat != SCollision::EWhat::none;
    }


bool CCanvas::on_button_press_event(GdkEventButton *event)
    {
    m_tMousePos = (*event - m_tShift)/m_dScale;
    
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

        if ( Distance( m_tCollision.tWhere, m_tMousePos) < 20 )
            m_oStartPoint = m_tCollision.tWhere;
        else
            m_oStartPoint = m_tMousePos;
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
            m_tShift = m_tShiftStart - (m_tEventPress - *event);
            }
        else if ( event->state & GDK_BUTTON1_MASK )
            {
            Collision(m_tMousePos);
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
    m_tMousePos = (*event - m_tShift)/m_dScale;
    
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
            if ( m_oButtonPressed == "0" ) {}
            if ( m_oButtonPressed == "1" ) {}
            if ( m_oButtonPressed == "2" ) {}
            if ( m_oButtonPressed == "3" ) {}
            if ( m_oButtonPressed == "4" ) {}
            }
        m_oButtonPressed="";
        queue_draw();
        return true;
        }

    if ( event->type & GDK_MOTION_NOTIFY )
        if ( event->state & GDK_BUTTON1_MASK )
            {
            
            if ( m_tCollision.eWhat == SCollision::EWhat::Line )
                {
                auto l{ Distance(m_oStartPoint,m_tMousePos) };
                m_vDrawing.emplace_back( SLine{ m_oStartPoint, 
                                               {m_oStartPoint.x + sin(m_tCollision.dAngle+M_PI/2)*l, 
                                                m_oStartPoint.y + cos(m_tCollision.dAngle+M_PI/2)*l}} );
                }
            else
                {
                if ( Distance( m_tCollision.tWhere, m_tMousePos) < 20 )
                    m_vDrawing.emplace_back( SLine{ m_oStartPoint, m_tCollision.tWhere } );
                else
                    m_vDrawing.emplace_back( SLine{ m_oStartPoint, m_tMousePos } );
                }
            Collision(m_tMousePos);
            }
        if ( event->state & GDK_BUTTON3_MASK )
            {
            }

    m_bButtonDown = false;

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

    static auto tHome{ SPoint{ m_tCtxSize }/2 };

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

    LineWidth( cr, {2/m_dScale} );

    switch (m_tCollision.eWhat)
        {
        case SCollision::EWhat::none:
            break;
        case SCollision::EWhat::Element:
            Color(cr, RED,  .25); Circle(cr, m_tCollision.tWhere, 20);
            Color(cr, RED,  .75); Ring  (cr, m_tCollision.tWhere, 20);
            Color(cr, GRAY, .25); Circle(cr, m_tCollision.tWhere,  5);
            Color(cr, RED,  .5 ); Ring  (cr, m_tCollision.tWhere,  5);
            break;
        case SCollision::EWhat::Line:
            cr->save();
            LineWidth( cr, {5/m_dScale} );
            Color(cr, RED,  .75); Line(cr, m_vDrawing[m_tCollision.nIndex]);
            LineWidth( cr, {4/m_dScale} );
            Color(cr, RED,  .25); Line(cr, m_vDrawing[m_tCollision.nIndex]);
            LineWidth( cr, {1/m_dScale} );
            Line(cr,SLine{m_oStartPoint, 
                         {m_oStartPoint.x + sin(m_tCollision.dAngle+M_PI/2)*1000, 
                          m_oStartPoint.y + cos(m_tCollision.dAngle+M_PI/2)*1000}});
            cr->restore();
            break;
        }


    Color(cr, BLACK);
    for ( auto const & a:m_vDrawing )
        {
        Line(cr, a);
        }
    
    if ( m_bButtonDown )
        {
        if ( m_tCollision.eWhat == SCollision::EWhat::Line )
            {
            auto l{ Distance(m_oStartPoint,m_tMousePos) };
            Line(cr,SLine{m_oStartPoint, 
                         {m_oStartPoint.x + sin(m_tCollision.dAngle+M_PI/2)*l, 
                          m_oStartPoint.y + cos(m_tCollision.dAngle+M_PI/2)*l}});
            }
        else
            {
            if ( Distance( m_tCollision.tWhere, m_tMousePos) < 20 )
                Line( cr, m_oStartPoint, m_tCollision.tWhere );
            else
                Line( cr, m_oStartPoint, m_tMousePos );
            }
        }


    // clock (upper right corner)

    auto t = std::time(nullptr);
    char sDate[100];
    std::strftime(sDate, sizeof(sDate), "%H:%M:%S", std::localtime(&t));
    Text(cr, (SPoint{-15.0,15.0}+SPoint{m_tCtxSize.x, .0}-m_tShift)/m_dScale, sDate, 1/m_dScale, false, TEXT_RIGHT);


    // Buttons
        
    int i=0;
    SPoint const tMousePos{m_tMousePos*m_dScale+m_tShift};
    for ( auto const & a:m_voButtons )
        {
        ++i;
        cr->set_line_width(1/m_dScale);

        if ( a.Collision(tMousePos) )
            {
            Color( cr, GREEN, .8 );
            }
        else
            {
            Color( cr, {.7,.7,.7}, .8 );
            }

        Rectangle( cr, (a-m_tShift)/m_dScale, SPoint{a.w/m_dScale, a.h/m_dScale} );
        cr->set_source_rgb(.2,.2,.2);
        Frame    ( cr, (a-m_tShift)/m_dScale, SPoint{a.w/m_dScale, a.h/m_dScale} );

        Color( cr, BLACK );
        Text(cr, {(a.x+a.w/2-m_tShift.x)/m_dScale, 
                       (a.y+a.h/2-m_tShift.y)/m_dScale}, a.sTitle, 1/m_dScale);
        }

    return true;
    }
