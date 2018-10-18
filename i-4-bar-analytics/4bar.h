#ifndef A4BAR_H
#define A4BAR_H

#include <gtkmm/drawingarea.h>

#include <vector>

#include "canvas.h"





int constexpr D_OK    {0};
int constexpr D2LONG  {1};
int constexpr D2SHORT {2};
struct SPointT
    {
    double x{.0}, y{.0};
    int    nSplit{D_OK};
    };
using VSpur = std::vector<SPointT>;


struct C4Bar
    {
    int  m_nSplit{D_OK};

    bool m_bWithTraces   { false };
    bool m_bDurchschlagen{ true  };
    bool m_bShowText     { true  };
    bool m_bShowHints    { true  };
    bool m_bShowBlink    { true  };
    bool m_bShowMouse    { true  };
    
    bool m_bCalculate    { true  };


    double m_tAnimStep   { 0.0 };

    void TraceReset()    { m_vSpurE1.clear(); m_vSpurE2.clear(); m_tAnimStep = 0; }
    void TraceInvrt()    { m_bWithTraces    = !m_bWithTraces; TraceReset(); }
    void Durchschlagen() { m_bDurchschlagen = !m_bDurchschlagen; TraceReset(); }
    void Rotate()        { }
    void WithText()      { m_bShowText      = !m_bShowText; }
    void WithHints()     { m_bShowHints     = !m_bShowHints; }
    void WithBlink()     { m_bShowBlink     = !m_bShowBlink; }
    void WithMouse()     { m_bShowMouse     = !m_bShowMouse; }


    bool   m_bAnimReverse{false};
    SPoint A0{-100, 0},A1{-100,-100}, A1m{A1},
           B0{ 100, 0},B1{ 100,-150}, B1m{B1};
    VSpur  m_vSpurE1;
    VSpur  m_vSpurE2;

    void Show(CairoCtx cr, double const & dScale);

    CCanvas::SCollision Collision(SPoint const & tPoint);
    bool MoveObject(CCanvas::SCollision const & tCollision, SPoint const & pos);
    void CalcGetriebe(CairoCtx cr, double const & ani, double const & dAni);


    void DrawGehausePunkt (CairoCtx cr, SPoint const & G0,
                                        std::string   const & t);
    void DrawGetriebe(CairoCtx cr, double const & ani, double const & dAni);

    }; // struct C4Bar

// A4BAR_H
#endif
