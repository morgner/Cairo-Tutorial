#include "canvas.h"

#include "typesNmath.h"

#include <iostream>
#include <memory>
#include <pangomm/fontdescription.h>








bool CCanvas::Collision(SPoint const & p)
    {
    m_tCollision.eWhat = SCollision::EWhat::none;
    size_t i{0};
    for ( auto const & a:m_vBricks )
        {
//      if ( p.x > a.p.x && p.x < a.p.x+a.s.x && p.y > a.p.y && p.y < a.p.y+a.s.y )
        if ( p.x+11 > a.p.x && p.x-11 < a.p.x+a.s.x && p.y+11 > a.p.y && p.y-11 < a.p.y+a.s.y )
            {
            m_tCollision.eWhat = SCollision::EWhat::Element;
            m_tCollision.nIndex = i;
            return true;
            }
        ++i;
        }

    return false;
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
        }
    else
        {
//      auto const bCol { Collision(m_tMousePos) };
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
//          m_tShift = m_tShiftStart - (m_tEventPress - *event);
            }
        else if ( event->state & GDK_BUTTON1_MASK )
            {
//          Collision(m_tMousePos);
            }
        else
            {
//          auto const bCol { Collision(m_tMousePos) };
            }
        }

    queue_draw();
    return true;
    }

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
            if ( m_oButtonPressed == "0" ) { m_vBricks.clear(); }
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
//          Collision(m_tMousePos);
            }
        if ( event->state & GDK_BUTTON3_MASK )
            {
            m_bBallFree = true;
            }

    queue_draw();
    return true;
    }
    
bool CCanvas::on_scroll_event(GdkEventScroll *event)
    {
    return true;

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

    if ( !m_bBricksInitialized )
        {
        double s = 2;
        SPoint pgpos{SPoint{-m_tCtxSize.x, -m_tCtxSize.y}/2+SPoint{50,100}};
        double xStep{12};
        double yStep{16};
        double w = (m_tCtxSize.x-100)/(xStep);
        double h = (m_tCtxSize.y-100)/(yStep);
        SPoint pgsiz{w-2*s, h-2*s};
        m_tPlayground = { pgpos, SPoint{m_tCtxSize.x, m_tCtxSize.y}-SPoint{100,100} };
        for (int x{0}; x < xStep; ++x)
            for (int y{0}; y < yStep/2; ++y)
                m_vBricks.emplace_back( SRect{ pgpos + SPoint{w*x+s, h*y+s}, pgsiz } );

        m_tPaddle = SRect{ SPoint{.0, m_tCtxSize.y/2-pgsiz.y}, pgsiz };
        m_tBall   = SPoint{.0, m_tCtxSize.y/2-pgsiz.y-11};

        m_bBricksInitialized = true;
        }

    // playground

    cr->set_line_width(1/m_dScale);
    Color( cr, BLACK );
    Rectangle( cr, SPoint{-m_tCtxSize.x, -m_tCtxSize.y}/2, SPoint{m_tCtxSize.x, m_tCtxSize.y} );
    Color( cr, GRAY );
    Rectangle( cr, m_tPlayground );

    // paddle
   
    if ( m_tMousePos.x < m_tPlayground.p.x+m_tPaddle.s.x/2 )
        m_tPaddle.p.x = m_tPlayground.p.x; 
    else if ( m_tMousePos.x > m_tPlayground.p.x+m_tPlayground.s.x-m_tPaddle.s.x/2 )
        m_tPaddle.p.x = m_tPlayground.p.x+m_tPlayground.s.x-m_tPaddle.s.x; 
    else
        m_tPaddle.p.x = m_tMousePos.x - m_tPaddle.s.x/2; 
    Color( cr, YELLOW );
    Rectangle( cr, m_tPaddle );
    Color( cr, GRAY );
    Frame( cr, m_tPaddle );

    // ball

    if ( !m_bBallFree )
        {
        if ( m_tMousePos.x < m_tPlayground.p.x+11 )
            m_tBall.x = m_tPlayground.p.x+11;
        else if ( m_tMousePos.x > m_tPlayground.p.x+m_tPlayground.s.x-11 )
            m_tBall.x = m_tPlayground.p.x+m_tPlayground.s.x-11;
        else
            m_tBall.x = m_tMousePos.x;
        }
    else
        {
        }
    Color( cr, RED );
    Circle( cr, m_tBall, 11 );
    Color( cr, BLACK );
    Ring( cr, m_tBall, 11 );

    static double m_nTick{m_dAnimator};
    if ( m_bBallFree && (m_nTick != m_dAnimator) )
        {
        m_nTick = m_dAnimator;
        static double m{0};
        static double s{.018};
        static double u{1.2};
        if (Collision(m_tBall)) s = -s;
        m += s;
        if (m > 1.0 - abs(s) || m < abs(s))
            {
            s = -s;
            if ( s > 0 )
                {
                if ( m_tBall.x < (m_tPaddle.p.x - 11) || m_tBall.x > (m_tPaddle.p.x + m_tPaddle.s.x + 11) )
                    {
                    m_bBallFree = false;
                    u = 1.2;
                    }
                else
                    {
                    u = (m_tBall.x - (m_tPaddle.p.x+m_tPaddle.s.x/2)) / ( m_tPaddle.s.x/5 );
                    }
                }
            }
        m_tBall.y =  m_tCtxSize.y/2-m_tPaddle.s.y-11 - m*(m_tPlayground.s.y-m_tPaddle.s.y-11*2);

        m_tBall.x += u;
        if ( m_tBall.x < m_tPlayground.p.x+11 || m_tBall.x > m_tPlayground.p.x+m_tPlayground.s.x-11 )
            {
            u = -u;
            }
        }

    // button pressed?

    // the bricks

    if ( m_tCollision.eWhat == SCollision::EWhat::Element )
        {
        m_vBricks.erase(m_vBricks.begin()+m_tCollision.nIndex);
        }
    
    int i=0;
    // LineWidth(cr, {m_tCtxSize.y/15/m_dScale});
    LineWidth(cr, {1/m_dScale});
    Color(cr, GREEN);
    for ( auto const & a:m_vBricks )
        {
    Color(cr, GREEN);
        Rectangle(cr, a);
    Color(cr, BLACK);
        Frame(cr, a);
        ++i;
        }

    // clock (upper right corner)

    auto t = std::time(nullptr);
    char sDate[100];
    std::strftime(sDate, sizeof(sDate), "%H:%M:%S", std::localtime(&t));
    Text(cr, (SPoint{-15.0,15.0}+SPoint{m_tCtxSize.x, .0}-m_tShift)/m_dScale, sDate, 1/m_dScale, false, TEXT_RIGHT);


    // Buttons
        
    i=0;
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
