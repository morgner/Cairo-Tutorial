#ifndef L3LAGEN_H
#define L3LAGEN_H

#include <gtkmm/drawingarea.h>

#include <vector>

#include "canvas.h"


using SGrundPunkt    = SPoint;
using SGehausePunkt  = SPoint;
using SGelenkPunkt   = SPoint;
using SPolPunkt      = SPoint;

using VGrundPunkte   = std::vector<SGrundPunkt>;
using VGelenkPunkte  = std::vector<SGelenkPunkt>;
using VPolDreieck    = std::vector<SPolPunkt>;
using VEbenen        = std::vector<SEbene>;


int constexpr D_OK    {0};
int constexpr D2LONG  {1};
int constexpr D2SHORT {2};
struct SPointT
    {
    double x{.0}, y{.0};
    int    nSplit{D_OK};
    };
using VSpur          = std::vector<SPointT>;


struct C3Lagen
    {
    protected:

        int  m_nSplit{D_OK};

    public:
    
        bool m_bWithTraces   { false };
        bool m_bDurchschlagen{ false };
        bool m_bRotate       { false };
        bool m_bShowText     { true  };
        bool m_bShowHints    { true  };
        bool m_bShowBlink    { true  };
        bool m_bShowMouse    { true  };

        double m_tAnimStep   { 0.0 };
        bool   m_bAnimReverse{ false };

        void TraceReset()    { m_vSpurE1.clear(); m_vSpurE2.clear(); m_tAnimStep = 0; }
        void TraceInvrt()    { m_bWithTraces    = !m_bWithTraces; TraceReset(); }
        void Durchschlagen() { m_bDurchschlagen = !m_bDurchschlagen; TraceReset(); }
        void Rotate()        { m_bRotate        = !m_bRotate; }
        void WithText()      { m_bShowText      = !m_bShowText; }
        void WithHints()     { m_bShowHints     = !m_bShowHints; }
        void WithBlink()     { m_bShowBlink     = !m_bShowBlink; }
        void WithMouse()     { m_bShowMouse     = !m_bShowMouse; }
    
    
        VGrundPunkte    m_vGrundPunkte;
        VGelenkPunkte   m_vGelenkPunkte[2];
        VPolDreieck     m_vPolDreieck;
        VEbenen         m_vEbenen;
        VSpur           m_vSpurE1;
        VSpur           m_vSpurE2;

    void Show(CairoCtx cr, double const & dScale);

    CCanvas::SCollision Collision(SPoint const & tPoint);
    bool MoveObject(CCanvas::SCollision const & tCollision, SPoint const & pos);


    void Update(SEbene       const & e, size_t const & i);
    void Update(SGrundPunkt  const & e, size_t const & i);
//    void Update(SGelenkPunkt const & e, size_t const & i);

    bool CalcGrundPunktDerivates(CairoCtx cr, SPoint      const & gp, 
                                              VPolDreieck const & pd );

    void DrawEbene(CairoCtx cr, SEbene const & e, size_t const & i);
    
    void DrawPoldreieck(CairoCtx cr, SPoint const & pa,
                                     SPoint const & pb,
                                     SPoint const & pc,
                                     SColor const & fc,
                                     SColor const & bc,
                                     double const & s=1.0);

    void DrawGrundPunkt(CairoCtx cr, SPoint const & m, size_t const & i);
    void DrawGelenkViereck(CairoCtx cr, size_t const & i);
    void DrawGehausePunkt (CairoCtx cr, SGehausePunkt const & G0,
                                       std::string   const & t);
    void DrawGetriebe(CairoCtx cr, double const & ani, double const & dAni);

    }; // struct C3Lagen

// L3LAGEN_H
#endif
