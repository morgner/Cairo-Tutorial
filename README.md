# Cairo-Tutorial
Simple C++ code samples to get a feeling for Cairo and how to animate your graohics

## How to compile

 * enter a sample directory, e.g. 8-collision
 * call: ../compile.sh

## what's in there?

### 0-empty-program
an empty C++ program

### 1-empty-cairo-program
an empty Cairo program

let us open a GDK window filled with Cairo filled with nothing

### 2-a-red-line
a Cairo program showing a line

we need to override default signal handler
```c++
bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
```
this is, what we do override it for
```c++
bool on_draw(Cairo::RefPtr<Cairo::Context> const & cr)
    {
    cr->set_source_rgb(1.,.5,.0);
    cr->set_line_width(3);

    cr->move_to( 11, 11);
    cr->line_to(111,111);
    cr->stroke();

    return true;
    }
```
 * inform the context about the next colour
 * inform it about the next line width
 * move the pencil to 11,11
 * propose a line to the context, to 111,111
 * request the context to draw this line using all accumulated information

### 3-a-total-line
showing a line trough the complete window.

To enable this, we need to know
the visible coordinates range of the cairo render context:
```c++
Gtk::Allocation allocation{ get_allocation() };
auto const width { (double)allocation.get_width() };
auto const height{ (double)allocation.get_height() };
```
now the total line:
```c++
cr->move_to(    0,      0);
cr->line_to(width, height);
cr->stroke();
```
and a funny gray circle in the middle:
```c++
cr->set_source_rgb(.7,.7,.7);
cr->arc(width/2, height/2, 100, 0, 2*M_PI);
cr->fill();
```

### 4-picture-png
a Cairo program showing a png picture

```c++
static Glib::RefPtr<Gdk::Pixbuf> const image  = Gdk::Pixbuf::create_from_file("CairoTut.png");
static Glib::RefPtr<Gdk::Pixbuf>       imageS = image->scale_simple( 180, 180, Gdk::INTERP_BILINEAR);
Gdk::Cairo::set_source_pixbuf(cr, imageS, width/2-90, height/2-90 );
cr->rectangle( width/2-90, height/2-90, 180, 180 );
cr->fill();
```
 * load picture
static Glib::RefPtr<Gdk::Pixbuf> const image  = Gdk::Pixbuf::create_from_file("CairoTut.png");
 * scale picture to destination size
static Glib::RefPtr<Gdk::Pixbuf>       imageS = image->scale_simple( 180, 180, Gdk::INTERP_BILINEAR);
 * place scaled pictures to specified position in render context
Gdk::Cairo::set_source_pixbuf(cr, imageS, width/2-90, height/2-90 );
 * open a hole for the pixels
cr->rectangle( width/2-90, height/2-90, 180, 180 );
 * show the hole

### 5-picture-svg
a Cairo program showing a svg drawing

### 6-mouse-position
a Cairo program tracking the mouse pointer

intoducing the first GDK callback into the Cairo application

```C++
bool on_motion_notify_event(GdkEventMotion *event)
    {
    tMousePos = SPoint{*event};
    queue_draw();
    return true;
    }
```

  * saving the current mouse position to draw a point there later
  * from Cairo request a call to `bool on_draw(...)`
  * return true because we have consumed the event

show the result utilizing `on_draw(...)`

```c++
cr->set_source_rgb(.0,.0,.9);
cr->arc(tMousePos.x, tMousePos.y, 3, 0, 2*M_PI);
cr->fill();
```
 * set the colour to blue
 * declare a circle at saved mouse position
 * fill the declared circle with the set colour

### 7-mouse-complete
a Cairo program tracking the mouse pointer and intractively drawing a line

```c++
bool CCanvas::on_button_press_event(GdkEventButton *event)
    {
    m_tMouseColor = { .0,.0,.9 };
    queue_draw();
    return true;
    }
```
 * if a mouse button is pressed, the mouse color becomes blue
 * to show thw change, a redraw is queued
```c++
bool CCanvas::on_button_release_event(GdkEventButton* event)
    {
    m_tMouseColor = { .5,.5,.5 };
    m_vMouseTrail.emplace_back( SPoint{ *event } );
    queue_draw();
    return true;
    }
```
 * if a mouse button is released, the position becomes queued
```c++
bool CCanvas::on_draw(Cairo::RefPtr<Cairo::Context> const & cr)
    {
    cr->set_source_rgb( .0,.0,.0 );
    cr->set_line_width(3);
    if ( m_vMouseTrail.size() )
        {
        cr->move_to(m_vMouseTrail[0].x,m_vMouseTrail[0].y);
        for (auto const & a:m_vMouseTrail)
            {
            cr->line_to( a.x, a.y);
            }
        cr->stroke();
        }
```
  * draw lines connecting all trigger points

### 8-collision
a Cairo program demonstraing the collision between the mouse pointer and graphucal objects

### 9-shift
a Cairo program showing how to move the canvas whilst keeping the mouse collider functional

### a-shift+move
a Cairo program adding moving of objects by mouse

### b-shift+zoom+move
a Cairo program adding zooming of tha canvas  whilst keeping the canvas shifter and the mouse collider functional

### c-animation
a Cairo program adding two different animation clocks to the mix, demonstrating multi animation

### d-text
a Cairo program adding text to all other functions

### e-ui-button-bar
adding a button bar
### f-drawing-helpers
adding an elastic line

### g-app-draw
a simple drawing application

### h-3-lagen-synthese
a mechanism sythese application

### i-4-bar-analytics
a mechanism analyse application

### k-like-cad
an experimental CAD application

### o-gaming
an experimental game with potential
