#include "canvas.h"

#include "typesNmath.h"

#include <iostream>
#include <memory>
#include <pangomm/fontdescription.h>








bool CCanvas::Collision(SPoint const & tPoint)
    {
/*
    double  dDist   {  1e9  };
    SPoint  tWhere  { .0,.0 };
    SPoint  tOffset { .0,.0 };
    bool    bForcePointer { false };
    SLine   tLine   {{0,0},{1,1}};
    double  dAngle  { 1e9 };
*/
    int i;
//    if ( m_bButtonDown )
        {
        m_vHints.clear();
        SHint   tHint;

        // Point,       // nearest line point a,m,b

        tHint.dDist = 1e9;
        tHint.eHint = SHint::EHint::none;
        i = 0;
        for ( auto const & a:m_vDrawing)
            {
            if ( double d = Distance(a.a, tPoint); d < tHint.dDist )
                {
                tHint.dDist   = d;
                tHint.tWhere  = a.a;
                tHint.tOffset = tPoint - a.a;
                tHint.nIndex  = i;
                tHint.nSubIx  = 0;
                tHint.eHint   = SHint::EHint::Point;
                tHint.bForcePointer = true;
                }
            if ( double d = Distance((a.a+a.b)/2, tPoint); d < tHint.dDist )
                {
                tHint.dDist   = d;
                tHint.tWhere  = (a.a+a.b)/2;
                tHint.tOffset = tPoint - (a.a+a.b)/2;
                tHint.nIndex  = i;
                tHint.nSubIx  = 1;
                tHint.eHint   = SHint::EHint::Point;
                tHint.bForcePointer = true;
                }
            if ( double d = Distance(a.b, tPoint); d < tHint.dDist )
                {
                tHint.dDist   = d;
                tHint.tWhere  = a.b;
                tHint.tOffset = tPoint - a.b;
                tHint.nIndex  = i;
                tHint.nSubIx  = 2;
                tHint.eHint   = SHint::EHint::Point;
                tHint.bForcePointer = true;
                }
            ++i;
            }
        if ( tHint.eHint == SHint::EHint::Point ) m_vHints.emplace_back(tHint);

        // Perpendicle, // perp. line to the line on starpoint



        // Close

        if ( tHint.eHint == SHint::EHint::Point && !m_bCloseLine && m_bButtonDown )
            {
            if ( Distance(m_oStartPoint, tPoint) > 20 &&
                 Distance(tHint.tWhere,  tPoint) < 20 )
                {
                auto const & a = m_vDrawing[tHint.nIndex];
                double dW = VectorSlope(a.a, a.b);

                SPoint iS = Intersection(a, SLine{ m_oStartPoint,
                                                  {m_oStartPoint.x+sin(dW)*100, 
                                                   m_oStartPoint.y+cos(dW)*100}} );

                double dD = Distance(m_oStartPoint, iS);
                
                tHint.dDist   = dD;
               // tHint.tWhere  = tPoint;
                tHint.tOffset = { .0, .0 };
                tHint.dAngle  = dW;
                tHint.nIndex  = tHint.nIndex;
                tHint.eHint   = SHint::EHint::Close;
                tHint.bForcePointer = true;
                tHint.tLine   = {{ tHint.tWhere.x-sin(dW)*dD, tHint.tWhere.y-cos(dW)*dD},
                                 { tHint.tWhere.x+sin(dW)*dD, tHint.tWhere.y+cos(dW)*dD} };

                m_bCloseLine = true;
                m_vDrawing.emplace_back( tHint.tLine );
                }

            if ( tHint.eHint == SHint::EHint::Close ) m_vHints.emplace_back(tHint);
            }

        // Parallel,    // nearest prallel line (same length?)

        tHint.dDist = 1e9;
        tHint.eHint = SHint::EHint::none;
        i = 0;
        for ( auto const & a:m_vDrawing)
            {
            auto l = Distance(m_oStartPoint, tPoint);
            if ( double dW = abs(VectorAngleDiff(a, SLine{m_oStartPoint, tPoint}) ); dW < 7.0/l || abs(dW-M_PI) < 7.0/l )
                {
                SPoint iS = Intersection(a, SLine{ tPoint,
                                                  {tPoint.x+sin(dW+M_PI/2)*100, 
                                                   tPoint.y+cos(dW+M_PI/2)*100}} );

                if ( double dD = Distance(tPoint, iS); dD < tHint.dDist )
                    {
                    tHint.dDist   = dD;
                    tHint.tWhere  = tPoint;
                    tHint.tOffset = { .0,.0 };
                    tHint.dAngle  = (dW >.1) ? VectorSlope(a.b,a.a) : VectorSlope(a.a,a.b);
                    tHint.nIndex  = i;
                    tHint.eHint   = SHint::EHint::Parallel;
                    tHint.bForcePointer = true;
                    tHint.tLine   = {{0,0},{1,1}};
                    }
                }
            ++i;
            }
        if ( tHint.eHint == SHint::EHint::Parallel ) m_vHints.emplace_back(tHint);


        // OnLine,      // mouse is on a line
        // Tagente,     // nearest tangential point
        // Center,      // nearest center of circle
        // Mirror,      // same distance ?
        // Orthogonal   // line would become orthogonal
        }


    return m_vHints.size() > 0;
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

        m_bStartJoin = false;
        for ( auto const & a:m_vHints )
            if ( a.eHint == SHint::EHint::Point )
                {
                if ( Distance( a.tWhere, m_tMousePos) < 20 )
                    {
                    m_oStartPoint = a.tWhere;
                    m_bStartJoin  = true;
                    m_nStartJoin  = a.nIndex;
                    }
                }
        if ( !m_bStartJoin ) m_oStartPoint = m_tMousePos;
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
    m_bButtonDown = false;
    
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
            if ( m_oButtonPressed == "0" ) { m_vDrawing.clear(); }
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
            double l;
            SPoint tEndPoint{ m_tMousePos };
            for ( auto const & a:m_vHints )
                {
                switch (a.eHint)
                    {
                    case SHint::EHint::Point: // 
                        if ( Distance(m_oStartPoint, tEndPoint) > 20 &&
                             Distance(a.tWhere,      tEndPoint) < 20 )
                        tEndPoint = a.tWhere;
                        break;
                    case SHint::EHint::Parallel: // nearest prallel line (same length?)
                        l = Distance(m_oStartPoint, tEndPoint);
                        tEndPoint = {m_oStartPoint.x + sin(a.dAngle+M_PI/2)*l, 
                                     m_oStartPoint.y + cos(a.dAngle+M_PI/2)*l};
                        break;
                    case SHint::EHint::Perpendicle: // Perpendicle
                        break;
                    case SHint::EHint::Close:   // 

                        tEndPoint = a.tWhere;

                        break;
                    case SHint::EHint::OnLine:  // mouse is on a line
                        break;
                    case SHint::EHint::Tagente: // nearest tangential point
                        break;
                    case SHint::EHint::Center:  // nearest center of circle
                        break;
                    case SHint::EHint::Mirror:  // same distance ?
                        break;
                    case SHint::EHint::Orthogonal: // line would become orthogonal
                        break;
                    default:
                        break;
                    }
                if ( m_bCloseLine )
                    {
                    m_bCloseLine = false;
                    m_vDrawing.pop_back();
                    }
                }

            m_vDrawing.emplace_back( SLine{ m_oStartPoint, tEndPoint } );

            Collision(m_tMousePos);
            }
        if ( event->state & GDK_BUTTON3_MASK )
            {
            }

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

    // the hints
    SPoint tEndPoint{ m_tMousePos };
//    if ( m_bButtonDown )
        {
        double l;
        
        cr->save();
        Color(cr, RED);
        LineWidth(cr, {1});
        for ( auto const & a:m_vHints )
            {
            switch (a.eHint)
                {
                case SHint::EHint::none: // nearest line point a,m,b
                    break;
                case SHint::EHint::Point: // nearest line point a,m,b

                    cr->save();
                    Color(cr, RED,  .25); Circle(cr, a.tWhere, 20);
                    Color(cr, RED,  .75); Ring  (cr, a.tWhere, 20);
                    Color(cr, GRAY, .25); Circle(cr, a.tWhere,  5);
                    Color(cr, RED,  .5 ); Ring  (cr, a.tWhere,  5);
                    cr->restore();

                    if ( Distance(m_oStartPoint, tEndPoint) > 20 &&
                         Distance(a.tWhere,      tEndPoint) < 20 )
                      tEndPoint = a.tWhere;
                    break;
                case SHint::EHint::Parallel: // nearest prallel line (same length?)
                //    if ( m_bButtonDown )
                        {
                        cr->save();
                        LineWidth(cr, {5/m_dScale});
                        Line(cr, m_vDrawing[a.nIndex]);
                        cr->restore();

                        cr->save();
                        LineWidth(cr, {1/m_dScale});
                        Line(cr,SLine{ m_oStartPoint, 
                                      {m_oStartPoint.x + sin(a.dAngle+M_PI/2)*1000, 
                                       m_oStartPoint.y + cos(a.dAngle+M_PI/2)*1000}});
                        cr->restore();

                        l = Distance(m_oStartPoint, tEndPoint);
                        tEndPoint = {m_oStartPoint.x + sin(a.dAngle+M_PI/2)*l, 
                                     m_oStartPoint.y + cos(a.dAngle+M_PI/2)*l};
                        }
                    break;
                case SHint::EHint::Perpendicle: // perp. line to the line on starpoint
                    break;
                case SHint::EHint::Close: // 
/*
                    if ( m_bCloseLine && !m_bButtonDown )
                        {
                        m_bCloseLine = false;
                        m_vDrawing.pop_back();
                        }
*/
                    break;
                case SHint::EHint::OnLine: // mouse is on a line
                    break;
                case SHint::EHint::Tagente: // nearest tangential point
                    break;
                case SHint::EHint::Center: // nearest center of circle
                    break;
                case SHint::EHint::Mirror: // same distance ?
                    break;
                case SHint::EHint::Orthogonal: // line would become orthogonal
                    break;
                }
            }
        cr->restore();
        }
        
    if ( m_bButtonDown )
        {
        cr->save();
        LineWidth(cr, {2/m_dScale});
        Color(cr, BLACK);
        Line(cr,SLine{ m_oStartPoint, tEndPoint });
        cr->restore();
        }


    // the drawing
    
    LineWidth(cr, {2/m_dScale});
    Color(cr, BLACK);
    for ( auto const & a:m_vDrawing )
        {
        Line(cr, a);
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
