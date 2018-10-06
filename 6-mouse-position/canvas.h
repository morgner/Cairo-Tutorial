#include <gtkmm.h>
#include <gtkmm/drawingarea.h>

struct SPoint
    {
    SPoint() = default;
    SPoint(double const & x, double const & y) : x(x), y(y) {}
    template<typename T>
        SPoint(T const & t) : x(t.x), y(t.y) {}
    double x{0}, y{0};
    };

class CCanvas : public Gtk::DrawingArea
    {
    public:
        CCanvas()
            {
            add_events(Gdk::BUTTON1_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::POINTER_MOTION_MASK);
            }

        virtual ~CCanvas() { };

    protected:
        // Override default signal handler:
        bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
        bool on_motion_notify_event(GdkEventMotion *event) override;
        
        SPoint   m_tMousePos;


    }; // CCanvas
