#ifndef CCANVAS_H
#define CCANVAS_H

#include <gtkmm.h>
#include <gtkmm/drawingarea.h>

#include <vector>
#include "typesNmath.h"



struct SFleck
    {
    double x{0}, y{0}, r{0};
    template<typename P>
        SFleck operator -= (P const & p)
            {
            x -= p.x;
            y -= p.y;
            return *this;
            }
    template<typename P>
        SFleck operator += (P const & p)
            {
            x += p.x;
            y += p.y;
            return *this;
            }
    template<typename P>
        SFleck operator = (P const & p)
            {
            x = p.x;
            y = p.y;
            return *this;
            }
   };

using VFlecken  = std::vector<SFleck>;

using CairoCtx = Cairo::RefPtr<Cairo::Context> const &;


// drawing primitives
inline void LineWidth( CairoCtx cr, SLineWidth const & w, double const & s=1 )
        {
        cr->set_line_width(w.w / ((s==0)?1:s));
        }

inline void Color( CairoCtx cr, SColor const & c )
        {
        cr->set_source_rgb( c.r, c.g, c.b );
        }

inline void Color( CairoCtx cr, SColor const & c, double const & a )
        {
        cr->set_source_rgba( c.r, c.g, c.b, a );
        }

inline void Color( CairoCtx cr, double const & r, double const & g, double const & b )
        {
        cr->set_source_rgb( r, g, b );
        }

inline void Color( CairoCtx cr, double const & r, double const & g, double const & b, double const & a )
        {
        cr->set_source_rgba( r, g, b, a );
        }

template<typename P>
    void MoveTo(CairoCtx cr, P const & tPoint)
        {
        cr->move_to(tPoint.x, tPoint.y);
        }

template<typename P>
    void LineTo(CairoCtx cr, P const & tPoint)
        {
        cr->line_to(tPoint.x, tPoint.y);
        }

template<typename S, typename... P>
    void Line(CairoCtx cr, S const & tPoint1, P const &... tPoints )
        {
         MoveTo(cr, tPoint1);
        (LineTo(cr, tPoints), ...);
        cr->stroke();
        }

inline void Line(CairoCtx cr, SLine const & tLine )
        {
        MoveTo(cr, tLine.a);
        LineTo(cr, tLine.b);
        cr->stroke();
        }

template<typename C>
    void LineStrip(CairoCtx cr, C const & tPoints )
        {
        bool b{true};
        for ( auto const & a:tPoints )
            {
            if (b)
                {
                MoveTo(cr, a);
                b = false;
                }
            else
                {
                LineTo(cr, a);
                }
            }
        cr->stroke();
        }

template<typename S, typename... P>
    void Polygon(CairoCtx cr, S const & tPoint1, P const &... tPoints )
        {
         MoveTo(cr, tPoint1);
        (LineTo(cr, tPoints), ...);
         LineTo(cr, tPoint1);
        cr->fill();
        }

template<typename C>
    void Polygon(CairoCtx cr, C const & tPoints )
        {
        bool b{true};
        for ( auto const & a:tPoints )
            {
            if (b) MoveTo(cr, a); else LineTo(cr, a);
            }
        cr->fill();
        }

template<typename P>
    void Circle(CairoCtx cr, P const & tPoint, double const & dRadius )
        {
        cr->arc(tPoint.x, tPoint.y, dRadius, 0, 2*M_PI);
        cr->fill();
        }

template<typename P>
    void Ring(CairoCtx cr, P const & tPoint, double const & dRadius )
        {
        cr->arc(tPoint.x, tPoint.y, dRadius, 0, 2*M_PI);
        cr->stroke();
        }

template<typename P, typename S>
    void Rectangle(CairoCtx cr, P const & tPoint, S const & tSize )
        {
        cr->rectangle(tPoint.x, tPoint.y, tSize.x, tSize.y);
        cr->fill();
        }

template<typename P, typename S>
    void Frame(CairoCtx cr, P const & tPoint, S const & tSize )
        {
        cr->rectangle(tPoint.x, tPoint.y, tSize.x, tSize.y);
        cr->stroke();
        }

SPoint draw_text(CairoCtx cr,
                 SPoint const & pos,
                 std::string const & crsText, 
                 double const & dScale = 1.0,
                 bool const & label = false);



class CCanvas : public Gtk::DrawingArea
    {
    public:

//        bool m_bDirectionLeft{ true };

        void AnimateHlt()    { m_bAnimate       = !m_bAnimate; }
        void AnimateAdd()    { m_dAnimate *= 1.1; m_dAnimate = (m_dAnimate>m_dAnimateMax)?m_dAnimateMax:m_dAnimate; }
        void AnimateSub()    { m_dAnimate *= 0.9; m_dAnimate = (m_dAnimate<m_dAnimateMin)?m_dAnimateMin:m_dAnimate; }
        void AnimateRev()    { m_bAnimateLeftUst = !m_bAnimateLeftUst; }

        
        CCanvas()
            {
            add_events(Gdk::BUTTON_PRESS_MASK | Gdk::SCROLL_MASK | Gdk::SMOOTH_SCROLL_MASK);
            add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
            add_events(Gdk::BUTTON1_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::POINTER_MOTION_MASK);
            add_events(Gdk::KEY_PRESS_MASK | Gdk::KEY_RELEASE_MASK);

            m_fSlot[0]         = sigc::bind(sigc::mem_fun(*this, &CCanvas::AnimateBi),  0);
            m_fSlot[1]         = sigc::bind(sigc::mem_fun(*this, &CCanvas::AnimateRot), 0);
            m_fSlot[2]         = sigc::bind(sigc::mem_fun(*this, &CCanvas::AnimateUst), 0);
            m_afConnections[0] = Glib::signal_timeout().connect(m_fSlot[0], 40);
            m_afConnections[1] = Glib::signal_timeout().connect(m_fSlot[1], 30);
            m_afConnections[2] = Glib::signal_timeout().connect(m_fSlot[2], 35);

            for ( int i{0}; i<13; ++i)
                {
                auto constexpr bs {38.0}; // button size
                auto constexpr uix{20.0},uiy{20.0},uiw{bs},uih{bs};
                auto constexpr bsp{ 8.0};  // button spacing
                auto constexpr bbo{16.0};  // button bar offset
                m_voButtons.emplace_back( bbo+i*(uiw+bsp), bbo+uiy, uiw, uih,
                                          std::to_string(i),
                                          std::to_string(i) );
                }
            }

        virtual ~CCanvas()
            {
            m_afConnections[0].disconnect();
            m_afConnections[1].disconnect();
            m_afConnections[2].disconnect();
            };

    protected:
        // Override default signal handler:
        bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
        bool on_scroll_event(GdkEventScroll *event) override;
        bool on_button_press_event(GdkEventButton * event) override;
        bool on_motion_notify_event(GdkEventMotion *event) override;
        bool on_button_release_event(GdkEventButton* release_event) override;

        bool Collision(SPoint const & tPoint);
        
        SPoint   m_tCtxSize     { .0,.0 };
        double   m_dScale       { 1.0 };
        
        bool     m_bShiftInit   { true };
        SPoint   m_tShift       { .0,.0 };
        SPoint   m_tEventPress  { .0,.0 };
        SPoint   m_tEventRelease{ .0,.0 };
        SPoint   m_tShiftStart  { .0,.0 };

       
        SPoint   m_tMousePos;

        VFlecken m_vFlecken { {30,30,20}, {300,300,50}, {500,200,40},
                              {40,50,25}, {240,320,30}, {580,270,45} };
    public:
    
        struct SCollision
            {
            SPoint tWhere  { .0,.0 };
            SPoint tOffset { .0,.0 };
            enum class EWhat
                {
                none,       // there was no collision
                Ebene,      // ebene in middle  index to be ignored
                Pol,        // polpunkt         index in 0,1,2
                G123,       // grundpunkt       index in 0,1
                G0,         // gehausepunkt     index in 0,1
                G           // gelenkpunkt      index in 0,1, sub in 1,2,3
                }eWhat {EWhat::none};
            int nIndex {0};	// O: L1, L2, L3
            int nSubIx {0};	// L: P1, PM, P2
            } m_tCollision;

    protected:
    
        // animation
        bool             m_bAnimate{true};
        bool             m_bAnimateLeftBi{true};
        bool             m_bAnimateLeftRot{true};
        bool             m_bAnimateLeftUst{true};

        // animation clock
        double           m_dAnimatorBi{0};  // $m_tAnimator animation parameter
        double           m_dAnimatorRot{0}; // $m_tAnimator animation parameter
        double           m_dAnimatorUst{0}; // $m_tAnimator animation parameter
        double           m_dAnimStep{0};    // intermediate animation parameter
        sigc::slot<bool> m_fSlot[3];
        sigc::connection m_afConnections[3];

        double           m_dAnimate   {0.0025}; // animation steps width
        double const     m_dAnimateMax{0.0250}; // maximal animation step width
        double const     m_dAnimateMin{0.0025}; // minimal animation step width

        bool AnimateBi(int c) // bidirectional
            {
            if (!m_bAnimate) return true;
            if (m_bAnimateLeftBi)
                m_dAnimatorBi -= m_dAnimate;
                else
                m_dAnimatorBi += m_dAnimate;
            if (m_dAnimatorBi <= 0) m_bAnimateLeftBi = false;
            if (m_dAnimatorBi >= 1) m_bAnimateLeftBi = true;
            queue_draw();
            return true;
            }

        bool AnimateRot(int c) // rotation
            {
            if (!m_bAnimate) return true;
            if (m_bAnimateLeftRot)
                m_dAnimatorRot = (m_dAnimatorRot <=  m_dAnimate) ? 1 : m_dAnimatorRot-m_dAnimate;
                else
                m_dAnimatorRot = (m_dAnimatorRot >=1-m_dAnimate) ? 0 : m_dAnimatorRot+m_dAnimate;
            queue_draw();
            return true;
            }

        bool AnimateUst(int c) // rotation
            {
//            if (!m_bAnimate) return true;
            if (m_bAnimateLeftUst)
                m_dAnimatorUst = (m_dAnimatorUst <=  m_dAnimate) ? 1 : m_dAnimatorUst-m_dAnimate;
                else
                m_dAnimatorUst = (m_dAnimatorUst >=1-m_dAnimate) ? 0 : m_dAnimatorUst+m_dAnimate;
            queue_draw();
            return true;
            }

    VButtons    m_voButtons;            // button bar
    std::string m_oButtonPressed{""};   // last pressed button

    char        m_cNextDrop{' '};

    }; // CCanvas

// CCANVAS_H
#endif
