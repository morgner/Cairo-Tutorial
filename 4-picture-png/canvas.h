#include <gtkmm.h>
#include <gtkmm/drawingarea.h>

class CCanvas : public Gtk::DrawingArea
    {
    public:
    CCanvas()
        {
        }

    virtual ~CCanvas() { };

    protected:
        // Override default signal handler:
        bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

    }; // CCanvas
