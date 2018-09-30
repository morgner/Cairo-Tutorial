#include <gtkmm.h>
#include <gtkmm/drawingarea.h>

#include <vector>


struct SPoint
    {
    SPoint() = default;
    SPoint(double const & x, double const & y) : x(x), y(y) {}
    template<typename T>
        SPoint(T const & t) : x(t.x), y(t.y) {}
    double x{0}, y{0};
    };

struct SColor
    {
    double r{0},g{0},b{0};
    };

using VPoints = std::vector<SPoint>;

class CCanvas : public Gtk::DrawingArea
    {
    public:
        CCanvas()
            {
            add_events(Gdk::BUTTON_PRESS_MASK | Gdk::SCROLL_MASK | Gdk::SMOOTH_SCROLL_MASK);
            add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
            add_events(Gdk::BUTTON1_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::POINTER_MOTION_MASK);
            add_events(Gdk::KEY_PRESS_MASK | Gdk::KEY_RELEASE_MASK);
            }

        virtual ~CCanvas() { };

    protected:
        // Override default signal handler:
        bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
        bool on_scroll_event(GdkEventScroll *event) override;
        bool on_button_press_event(GdkEventButton * event) override;
        bool on_motion_notify_event(GdkEventMotion *event) override;
        bool on_button_release_event(GdkEventButton* release_event) override;
        
        SPoint  m_tMousePos;
        SColor  m_tMouseColor{ .5,.5,.5 };
        VPoints m_vMouseTrail;

    }; // CCanvas
