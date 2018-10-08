#include <gtkmm.h>
#include <gtkmm/drawingarea.h>

#include <vector>


struct SPoint
    {
    SPoint() = default;
    SPoint(SPoint const & src) = default;
    template<typename T>
        SPoint(T const & x, T const & y) : x(x), y(y) {}
    template<typename T>
        SPoint(T const & t) : x(t.x), y(t.y) {}
    double x{0}, y{0};
    template<typename P>
        SPoint operator = (P const & p)
            {
            x = p.x;
            y = p.y;
            return *this;
            }
    template<typename P>
        SPoint operator -= (P const & p)
            {
            x -= p.x;
            y -= p.y;
            return *this;
            }
    template<typename P>
        SPoint operator += (P const & p)
            {
            x += p.x;
            y += p.y;
            return *this;
            }
    };

inline bool operator == (SPoint const & p1, SPoint const & p2)
    {
    return (p1.x==p2.x) && (p1.y==p2.y);
    }

inline bool operator != (SPoint const & p1, SPoint const & p2)
    {
    return !(p1==p2);
    }

inline SPoint operator - (SPoint const & p1, SPoint const & p2)
    {
    return {p1.x-p2.x, p1.y-p2.y};
    }

inline SPoint operator + (SPoint const & p1, SPoint const & p2)
    {
    return {p2.x+p1.x, p2.y+p1.y};
    }

inline SPoint operator / (SPoint const & p, double const & d)
    {
    return {p.x/d, p.y/d};
    }

inline SPoint operator * (SPoint const & p, double const & d)
    {
    return {p.x*d, p.y*d};
    }


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

struct SColor
    {
    double r{0},g{0},b{0};
    };

struct SButton
    {
    double x{0},y{0},w{0},h{0};
    std::string const sTitle;
    std::string const sFunct;

    SButton (double const & x, double const & y,
             double const & w, double const & h,
             std::string const t,
             std::string const f)
        : x(x), y(y), w(w), h(h), sTitle(t), sFunct(f) {}

    bool Collision(SPoint const & p) const
        {
        return ( p.x > x && p.x < x+w && p.y > y && p.y < y+h );
        }
    };

using VButtons = std::vector<SButton>;

using VPoints  = std::vector<SPoint>;
using VFlecken = std::vector<SFleck>;

// the first argument can be anything having the members x and y
// a Gtk event, a SFleck or whatever
template<typename P, typename T>
    double Distance( P const & a, T const & b )
        {
        return sqrt( pow((a.x-b.x),2) + pow((a.y-b.y),2) );
        }

template<typename P>
    void MoveTo(Cairo::RefPtr<Cairo::Context> const & cr, P const & tPoint)
        {
        cr->move_to(tPoint.x, tPoint.y);
        }

template<typename P>
    void LineTo(Cairo::RefPtr<Cairo::Context> const & cr, P const & tPoint)
        {
        cr->line_to(tPoint.x, tPoint.y);
        }

template<typename S, typename... P>
    void Line(Cairo::RefPtr<Cairo::Context> const & cr, S const & tPoint1, P const &... tPoints )
        {
         MoveTo(cr, tPoint1);
        (LineTo(cr, tPoints), ...);
        cr->stroke();
        }

template<typename L>
    void Line(Cairo::RefPtr<Cairo::Context> const & cr, L const & tLine )
        {
        MoveTo(cr, {tLine.x1,tLine.y1});
        LineTo(cr, {tLine.x2,tLine.y2});
        cr->stroke();
        }

template<typename C>
    void LineStrip(Cairo::RefPtr<Cairo::Context> const & cr, C const & tPoints )
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
    void Polygon(Cairo::RefPtr<Cairo::Context> const & cr, S const & tPoint1, P const &... tPoints )
        {
         MoveTo(cr, tPoint1);
        (LineTo(cr, tPoints), ...);
         LineTo(cr, tPoint1);
        cr->fill();
        }

template<typename C>
    void Polygon(Cairo::RefPtr<Cairo::Context> const & cr, C const & tPoints )
        {
        bool b{true};
        for ( auto const & a:tPoints )
            {
            if (b) MoveTo(cr, a); else LineTo(cr, a);
            }
        cr->fill();
        }

template<typename P>
    void Circle(Cairo::RefPtr<Cairo::Context> const & cr, P const & tPoint, double const & dRadius )
        {
        cr->arc(tPoint.x, tPoint.y, dRadius, 0, 2*M_PI);
        cr->fill();
        }

template<typename P>
    void Ring(Cairo::RefPtr<Cairo::Context> const & cr, P const & tPoint, double const & dRadius )
        {
        cr->arc(tPoint.x, tPoint.y, dRadius, 0, 2*M_PI);
        cr->stroke();
        }

template<typename P, typename S>
    void Rectangle(Cairo::RefPtr<Cairo::Context> const & cr, P const & tPoint, S const & tSize )
        {
        cr->rectangle(tPoint.x, tPoint.y, tSize.x, tSize.y);
        cr->fill();
        }

template<typename P, typename S>
    void Frame(Cairo::RefPtr<Cairo::Context> const & cr, P const & tPoint, S const & tSize )
        {
        cr->rectangle(tPoint.x, tPoint.y, tSize.x, tSize.y);
        cr->stroke();
        }


class CCanvas : public Gtk::DrawingArea
    {
    public:
        CCanvas()
            {
            add_events(Gdk::BUTTON_PRESS_MASK | Gdk::SCROLL_MASK | Gdk::SMOOTH_SCROLL_MASK);
            add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
            add_events(Gdk::BUTTON1_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::POINTER_MOTION_MASK);
            add_events(Gdk::KEY_PRESS_MASK | Gdk::KEY_RELEASE_MASK);

            m_fSlot[0]         = sigc::bind(sigc::mem_fun(*this, &CCanvas::AnimateBi), 0);
            m_fSlot[1]         = sigc::bind(sigc::mem_fun(*this, &CCanvas::AnimateRot), 0);
            m_afConnections[0] = Glib::signal_timeout().connect(m_fSlot[0], 40);
            m_afConnections[1] = Glib::signal_timeout().connect(m_fSlot[1], 40);

            for ( int i{0}; i<7; ++i)
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
        SColor   m_tMouseColor{ .5,.5,.5 };
        VPoints  m_vMouseTrail;
        VFlecken m_vFlecken { {30,30,20}, {300,300,50}, {500,200,40},
                              {40,50,25}, {240,320,30}, {580,270,45} };

        struct SCollision
            {
            SPoint tWhere  { .0,.0 };
            SPoint tOffset { .0,.0 };
            enum class EWhat
                {
                none,       // there was no collision
                Fleck,      // move a Fleck
                Line,       // move a Line
                }eWhat {EWhat::none};
            int nIndex {0};	// O: L1, L2, L3
            int nSubIx {0};	// L: P1, PM, P2
            } m_tCollision;

        // animation
        bool             m_bAnimate{true};
        bool             m_bAnimateLeftBi{true};
        bool             m_bAnimateLeftRot{true};

        // animation clock
        double           m_dAnimatorBi{0};  // $m_tAnimator animation parameter
        double           m_dAnimatorRot{0}; // $m_tAnimator animation parameter
        double           m_dAnimStep{0};    // intermediate animation parameter
        sigc::slot<bool> m_fSlot[2];
        sigc::connection m_afConnections[2];

        double           m_dAnimate   {0.0125}; // animation steps width
        double const     m_dAnimateMax{0.0250}; // maximal animation step width
        double const     m_dAnimateMin{0.0025}; // minimal animation step width

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

    VButtons    m_voButtons;            // button bar
    std::string m_oButtonPressed{""};   // last pressed button

    }; // CCanvas
