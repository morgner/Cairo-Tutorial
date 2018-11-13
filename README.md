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

### 2-a-red-line
a Cairo program showing a line

### 3-a-total-line
showing a line trough the complete window

### 4-picture-png
a Cairo program showing a png picture

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
 * declare a circle at mouse position
 * fill the declared circle with the set colour

### 7-mouse-complete
a Cairo program tracking the mouse pointer and intractively drawing a line

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
