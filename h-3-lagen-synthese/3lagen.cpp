#include "3lagen.h"




bool C3Lagen::MoveObject(CCanvas::SCollision const & tCollision, SPoint const & pos)
    {
    int i;
    switch (tCollision.eWhat)
        {
        case CCanvas::SCollision::EWhat::Ebene:
            {
            SEbene e{m_vEbenen[tCollision.nIndex]};
            auto fl{ Distance(e.a,e.b) };
            switch (tCollision.nSubIx)
                {
                case 1:
                    e.a = pos - tCollision.tOffset;
                    Update( FixedLenLine(e, fl, true ) , tCollision.nIndex);
                    break;
                case 2:
                    Update(SEbene{pos - tCollision.tOffset + (e.a-e.b)/2,
                                  pos - tCollision.tOffset - (e.a-e.b)/2}, tCollision.nIndex);
                    break;
                case 3:
                    e.b = pos - tCollision.tOffset;
                    Update( FixedLenLine(e, fl, false ) , tCollision.nIndex);
                    break;
                } // switch (tCollision.nSubIx)
                
            i = 0; for (auto const & a:m_vGrundPunkte) Update(a, i++);
            } // case CCanvas::SCollision::EWhat::Ebene:
            break;
            
        case CCanvas::SCollision::EWhat::G123:
            Update(SGrundPunkt{pos - tCollision.tOffset}, tCollision.nIndex);
            break;

        case CCanvas::SCollision::EWhat::G:
//            Update(SGelenkPunkt{pos - tCollision.tOffset}, tCollision.nIndex);
            break;
        }

    return true;
    }

/*
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

    VGrundPunkte    m_vGrundPunkte;
    VGelenkPunkte   m_vGelenkPunkte[2];
    VPolDreieck     m_vPolDreieck;
    VEbenen         m_vEbenen;

*/

CCanvas::SCollision C3Lagen::Collision(SPoint const & tPoint)
    {
    int i;
    double const dRCol{9};
    CCanvas::SCollision tCollision;
    tCollision.eWhat = CCanvas::SCollision::EWhat::none;

    i = 0;
    for ( auto const & a:m_vEbenen )
        {
        if ( Distance(a.a, tPoint) < dRCol )
            {
            tCollision.tWhere  = tPoint;
            tCollision.tOffset = tPoint - a.a;
            tCollision.eWhat   = CCanvas::SCollision::EWhat::Ebene;
            tCollision.nIndex  = i;
            tCollision.nSubIx  = 1;
            return std::move(tCollision);
            }
        if ( Distance( (a.a+a.b)/2, tPoint) < dRCol )
            {
            tCollision.tWhere  = tPoint;
            tCollision.tOffset = tPoint - (a.a+a.b)/2;
            tCollision.eWhat   = CCanvas::SCollision::EWhat::Ebene;
            tCollision.nIndex  = i;
            tCollision.nSubIx  = 2;
            return std::move(tCollision);
            }
        if ( Distance(a.b, tPoint) < dRCol )
            {
            tCollision.tWhere  = tPoint;
            tCollision.tOffset = tPoint - a.b;
            tCollision.eWhat   = CCanvas::SCollision::EWhat::Ebene;
            tCollision.nIndex  = i;
            tCollision.nSubIx  = 3;
            return std::move(tCollision);
            }
        ++i;
        }

    i = 0;
    for ( auto const & a:m_vGrundPunkte )
        {
        if ( Distance(a, tPoint) < dRCol )
            {
            tCollision.tWhere  = tPoint;
            tCollision.tOffset = tPoint - a;
            tCollision.eWhat   = CCanvas::SCollision::EWhat::G123;
            tCollision.nIndex  = i;
            return std::move(tCollision);
            }
        ++i;
        }

    return std::move(tCollision);
    }



void C3Lagen::Show(CairoCtx cr, double const & dScale)
    {
    int i{0}; for (auto const & a:m_vEbenen) DrawEbene(cr, a, i++);

    if (m_vEbenen.size() < 3) return;

    SEbene const & e1 = m_vEbenen[0],
                   e2 = m_vEbenen[1],
                   e3 = m_vEbenen[2];

    // poldreieck
    if (m_vPolDreieck.size() < 3) return;

    SPoint const & p12{m_vPolDreieck[0]},
                   p13{m_vPolDreieck[1]},
                   p23{m_vPolDreieck[2]};

    // spiegelpoldreiecke
    SPoint const p12m = PointMirror(p12, {p23,p13}),
                 p13m = PointMirror(p13, {p23,p12}),
                 p23m = PointMirror(p23, {p12,p13});

    // draw all poldreicks
    DrawPoldreieck(cr, p12,  p13,  p23,  GREEN, BLACK, dScale);
    DrawPoldreieck(cr, p12m, p13,  p23,  BLUE,  GRAY,  dScale);
    DrawPoldreieck(cr, p12,  p13m, p23,  BLUE,  GRAY,  dScale);
    DrawPoldreieck(cr, p12,  p13,  p23m, BLUE,  GRAY,  dScale);

    // draw points and text into poldreiecks
    Color(cr, WHITE); Circle(cr,p12, 5); Circle(cr,p13, 5); Circle(cr,p23, 5);
                      Circle(cr,p12m,5); Circle(cr,p13m,5); Circle(cr,p23m,5);
    Color(cr, BLACK); Ring  (cr,p12, 5); Ring  (cr,p13, 5); Ring  (cr,p23, 5);
                      Ring  (cr,p12m,5); Ring  (cr,p13m,5); Ring  (cr,p23m,5);
    Color(cr, BLACK); // text color
    SPoint const pto{.0,-11.3};
    draw_text(cr, p12 +pto, "P12" , 1, true);
    draw_text(cr, p13 +pto, "P13" , 1, true);
    draw_text(cr, p23 +pto, "P23" , 1, true);
    draw_text(cr, p12m+pto, "P12'", 1, true);
    draw_text(cr, p13m+pto, "P13'", 1, true);
    draw_text(cr, p23m+pto, "P23'", 1, true);

    // Grundpunkte
    i = 0;
    for (auto const & a:m_vGrundPunkte)
        {
        DrawGrundPunkt(cr, {a}, i++);
        }

    DrawGelenkViereck(cr, 0);
    DrawGelenkViereck(cr, 1);
    }

void C3Lagen::Update(SEbene const & e, size_t const & i)
    {
    if (i>2) return;
    if (m_vEbenen.size() < 3) m_vEbenen.emplace_back(e);
    if (m_vEbenen.size() < 3) return;
    
    m_vEbenen[i] = e;

    if (m_vEbenen.size() < 3) return;

    SEbene const & e1 = m_vEbenen[0],
                   e2 = m_vEbenen[1],
                   e3 = m_vEbenen[2];

    SPoint const p12 = Polpunkt(e1, e2),
                 p13 = Polpunkt(e1, e3),
                 p23 = Polpunkt(e2, e3);

    if (m_vPolDreieck.size()==0)
        {
        m_vPolDreieck.emplace_back(p12);
        m_vPolDreieck.emplace_back(p13);
        m_vPolDreieck.emplace_back(p23);
        }
    else
        {
        m_vPolDreieck[0] = p12;
        m_vPolDreieck[1] = p13;
        m_vPolDreieck[2] = p23;
        }
    }

void C3Lagen::Update(SGrundPunkt const & G123, size_t const & i)
    {
    if (i>1) return;
    if (m_vGrundPunkte.size() < 2  ) m_vGrundPunkte.emplace_back(G123);
    if (m_vGrundPunkte.size() < i+1) return;

    m_vGrundPunkte[i] = G123;
    
    if (m_vPolDreieck.size() < 3) return;
    SPolPunkt const & p12 = m_vPolDreieck[0],
                      p13 = m_vPolDreieck[1],
                      p23 = m_vPolDreieck[2];
    
    while (m_vGelenkPunkte[i].size() < 4) m_vGelenkPunkte[i].emplace_back(SGelenkPunkt{.0,.0});
    // G123 => G0-G3
    SGelenkPunkt const & G1 = m_vGelenkPunkte[i][1] = PointMirror(G123, {p12, p13});
    SGelenkPunkt const & G2 = m_vGelenkPunkte[i][2] = PointMirror(G123, {p12, p23});
    SGelenkPunkt const & G3 = m_vGelenkPunkte[i][3] = PointMirror(G123, {p13, p23});
    SUmkreis u = Umkreis(G1, G2, G3);
    SGelenkPunkt G0 = m_vGelenkPunkte[i][0] = u.p;
    }
/*
void Update(SGelenkPunkt const & A1, size_t const & i)
    {
    if (i>0) return;
    if (m_vGelenkPunkte.size() < 2  ) m_vGelenkPunkte.emplace_back(A1);
    if (m_vGelenkPunkte.size() < i+1) return;

    m_vGelenkPunkte[0] = A1;

    }
*/
void C3Lagen::DrawEbene(CairoCtx cr, SEbene const & e, size_t const & i)
    {
    cr->save();
    Color(cr, RED);
    LineWidth(cr, {7.5});
    Line(cr, e);
    Color(cr, WHITE);
    Circle(cr, e.a, 3);
    Circle(cr, e.b, 3);
    cr->restore();

    SPoint const pto{.0,-11.3};
    draw_text(cr, { (e.b + e.a)/2 +pto }, "E" + std::to_string(i+1) , 1, true);
    draw_text(cr, e.a +pto, "P1", 1, true);
    draw_text(cr, e.b +pto, "P2", 1, true);
    }

void C3Lagen::DrawPoldreieck(CairoCtx cr, SPoint const & pa, 
                                          SPoint const & pb, 
                                          SPoint const & pc, 
                                          SColor const & fc, 
                                          SColor const & bc, 
                                          double const & s)
    {
    if ( !m_bShowHints ) return;
    
    LineWidth(cr, {1/s});
    Color(cr, fc, .25 );
    Polygon(cr, pa,pb,pc,pa);
    Color(cr, bc);
    Line(cr, pa,pb,pc,pa);
    if ( (abs(pa.x) < 3000) && (abs(pb.x) < 3000) && (abs(pc.x) < 3000) &&
         (abs(pa.y) < 3000) && (abs(pb.y) < 3000) && (abs(pc.y) < 3000) )
        {
        SUmkreis u = Umkreis(pa,pb,pc);
        Ring(cr, u.p,u.r);
        }
    }

void C3Lagen::DrawGrundPunkt(CairoCtx cr, SPoint const & m, size_t const & i)
    {
    cr->save();
    
    // G123
    Color (cr, .0,.5,.5, 0.75);
    Circle(cr, m, 8);
    Color (cr, BLACK);
    Ring  (cr, m, 8);

    SPoint const pto{.0,-11.3};
    std::string L{"AB"[i]};
    draw_text(cr, m +pto, L + "123" , 1, true);
 
    cr->restore();
    }

void C3Lagen::DrawGetriebe(CairoCtx cr, double const & ani, double const & dAni)
    {
    if (m_vGelenkPunkte[0].size() < 4) return;
    if (m_vGelenkPunkte[1].size() < 4) return;
    // G0-G3
    SGelenkPunkt const & A0 = m_vGelenkPunkte[0][0];
    SGelenkPunkt const & A1 = m_vGelenkPunkte[0][1];
    SGelenkPunkt const & A2 = m_vGelenkPunkte[0][2];
    SGelenkPunkt const & A3 = m_vGelenkPunkte[0][3];
    SGelenkPunkt const & B0 = m_vGelenkPunkte[1][0];
    SGelenkPunkt const & B1 = m_vGelenkPunkte[1][1];
    SGelenkPunkt const & B2 = m_vGelenkPunkte[1][2];
    SGelenkPunkt const & B3 = m_vGelenkPunkte[1][3];

    auto const GL = Distance( A0, B0 );
    auto const AL = Distance( A0, A1 );
    auto const BL = Distance( B0, B1 );
    auto const CL = Distance( A1, B1 );
    auto const DL = Distance( A1, B0 );

    auto const S = sin(ani*2*M_PI);
    auto const C = cos(ani*2*M_PI);

    auto const A1m = SPoint{A0.x+AL*S, A0.y+AL*C};
    auto const  d = Distance( A1m , B0 );

    m_nSplit = 0;
    if (     BL+CL  < d ) { m_nSplit = D2LONG;  };
    if ( abs(BL-CL) > d ) { m_nSplit = D2SHORT; };

    double const lw{7};
    LineWidth(cr, {lw});
    Color(cr, BLACK);

    auto const epsi = VectorSlope(A1m, B0) -M_PI/2;

    auto const beta = (m_nSplit != 0) ? 0 : Alpha(BL,CL,d); // 143,  86, Distance( A1m , B0 )  g:58, a:43
    auto const gama = (m_nSplit != 0) ? 0 : Alpha(CL,BL,d); //  86, 143, Distance( A1m , B0 )

//	std::cout << "e: " << epsi/M_PI*180 << ", b: " << beta/M_PI*180 << ", g: " << gama/M_PI*180 << '\n';


    static bool zero {false};
    if ( beta == 0 && gama == 0 )
        {
        if ( !zero ) m_bAnimReverse = !m_bAnimReverse;
        zero = true;
        }
    else
        {
        zero = false;
        }

    SPoint B1m;
    SPoint pab;
    
    auto const epg{epsi+gama};
    auto const emg{epsi-gama};
    auto const epb{epsi+beta};
    auto const emb{epsi-beta};
    if ( m_bDurchschlagen )
        {
        B1m = SPoint{B0.x+BL *sin(emg),      B0.y+BL *cos(emg)};
        pab = SPoint{A1m.x+CL*sin(epb+M_PI), A1m.y+CL*cos(epb+M_PI)};
        }
    else
        {
        B1m = SPoint{B0.x+BL *sin(epg),      B0.y+BL *cos(epg)};
        pab = SPoint{A1m.x+CL*sin(emb+M_PI), A1m.y+CL*cos(emb+M_PI)};
        }
    //	std::cout << epsi/M_PI*180 << ", a="  << (B0.y-pa.y) << ", b="  << (B0.x-pa.x) << ", c="  << d << ", " << '\n';

    LineWidth(cr, {lw+2});
    Color(cr, WHITE);
    if ( m_nSplit != D_OK )
        {
        Color(cr, BLACK);
        Line(cr, A0, A1m);
        if ( m_nSplit == D2LONG  ) { Color(cr, YELLOW); };
        if ( m_nSplit == D2SHORT ) { Color(cr, CYAN  ); };
        Line(cr, A1m, B0);
        }
    else if (m_bShowBlink)
        {
        Color(cr, WHITE);
        Line(cr, A0, A1m);
        Line(cr, A1m, pab);
        Line(cr, B0, B1m);
        }
    else
        {
        cr->set_source_rgb(.5,.0,.0); Line(cr, A0,  A1m);
        cr->set_source_rgb(.0,.5,.0); Line(cr, A1m, pab);
        cr->set_source_rgb(.0,.0,.5); Line(cr, B0,  B1m);
        }

    if ( Distance(A1, A1m)<20 )
        if ( Distance(B1, B1m)<30 )
        cr->set_source_rgb(.7, 1, .7);
        else
        cr->set_source_rgb(1, .4, .7);
    else if ( Distance(A2, A1m)<20 )
        if ( Distance(B2, B1m)<30 )
        cr->set_source_rgb(.7, 1, .7);
        else
        cr->set_source_rgb(1, .4, .7);
    else if ( Distance(A3, A1m)<20 )
        if ( Distance(B3, B1m)<30 )
        cr->set_source_rgb(.7, 1, .7);
        else
        cr->set_source_rgb(1, .4, .7);
    else
        cr->set_source_rgb(0,0,0);

    LineWidth(cr, {lw});

    if ( m_nSplit != D_OK )
        {
        Color(cr, BLACK);
        Line(cr, A0, A1m);
        if ( m_nSplit == D2LONG  ) { Color(cr, YELLOW); };
        if ( m_nSplit == D2SHORT ) { Color(cr, CYAN  ); };
        Line(cr, A1m, B0);
        }
    else if (m_bShowBlink)
        {
        Line(cr, A0,  A1m);
        Line(cr, A1m, pab);
        Line(cr, B0,  B1m);
        }
    else
        {
        cr->set_source_rgb(.5,.0,.0); Line(cr, A0, A1m);
        cr->set_source_rgb(.0,.5,.0); Line(cr, A1m,pab);
        cr->set_source_rgb(.0,.0,.5); Line(cr, B0, B1m);
        }

    Ring(cr, A1m, 3);
    Ring(cr, B1m, 3);

    Color(cr, WHITE);

    Circle(cr, A1m, 3);
    Circle(cr, B1m, 3);


    // Ebene **************************************
    SPoint const ep1{m_vEbenen[0].a.x,m_vEbenen[0].a.y};
    SPoint const ep2{m_vEbenen[0].b.x,m_vEbenen[0].b.y};
    auto   const  we{VectorSlope(ep1, ep2)};
    auto   const  le{Distance(ep1, ep2)};

    auto   const  wg{VectorSlope(A1m, B1m)};
    auto   const  wb{VectorSlope(B1,  ep1)};
    auto   const  dw{VectorDiff(B1, A1, ep1)};

    auto   const dwe{VectorDiff(ep1, B1, ep2)};

    auto   const lve{Distance(B1, ep1)};

//  std::cout << " dw: " << dw/M_PI*180 << ",  wg: " << wg/M_PI*180 << ",  wb: " << wb/M_PI*180 << ",  lve: " << lve << '\n';

    SPoint const be1{B1m.x+lve*sin(wg+dw-M_PI/2), B1m.y+lve*cos(wg+dw-M_PI/2)};
    SPoint const be2{be1.x+le*sin(wg+dw+dwe+M_PI/2), be1.y+le*cos(wg+dw+dwe+M_PI/2)};
    Color(cr, CYAN, .4);
    Polygon(cr, A1m, be1, B1m);

    Color(cr, {.25,.75,.75}, .75);
    LineWidth(cr, {2});
    Line(cr, A1m, be1, B1m);

    cr->set_source_rgba(.99,.2,.2,.5);
    LineWidth(cr, {5.5});
    Line(cr, be1, be2);
    
    DrawGehausePunkt(cr, A0, "A0");
    DrawGehausePunkt(cr, B0, "B0");

    // traces
    if ( m_bWithTraces )
        {
        if (360*m_tAnimStep+1 > m_vSpurE1.size() && !zero)
            {
            m_vSpurE1.emplace_back(SPointT{be1.x,be1.y,m_nSplit!=D_OK});
            m_vSpurE2.emplace_back(SPointT{be2.x,be2.y,m_nSplit!=D_OK});
            }
        if (m_tAnimStep<=1.1) m_tAnimStep += dAni; // m_dAnimate;
        cr->set_source_rgb(0,0,0);

        cr->set_line_width(2);
        bool b;
        if (m_vSpurE1.size())
            {
            cr->move_to(m_vSpurE1[0].x, m_vSpurE1[0].y);
            b = m_vSpurE1[0].nSplit!=D_OK;
            }
        for (auto const & a:m_vSpurE1)
            {
            if (b) Color(cr, RED); else Color(cr, BLACK);
            if ( a.nSplit!=D_OK != b )
                {
                cr->line_to(a.x, a.y);
                cr->stroke();
                }
            cr->line_to(a.x, a.y);
            b = a.nSplit!=D_OK;
            }
        cr->stroke();

        if (m_vSpurE2.size())
            {
            cr->move_to(m_vSpurE2[0].x, m_vSpurE2[0].y);
            b = m_vSpurE2[0].nSplit!=D_OK;
            }
        for (auto const & a:m_vSpurE2)
            {
            if (b) Color(cr, RED); else Color(cr, BLACK);
            if ( a.nSplit!=D_OK != b )
                {
                cr->line_to(a.x, a.y);
                cr->stroke();
                }
            cr->line_to(a.x, a.y);
            b = a.nSplit!=D_OK;
            }
        cr->stroke();
        }

    }

void C3Lagen::DrawGelenkViereck(CairoCtx cr, size_t const & i)
    {
    if ( !m_bShowHints ) return;
    
    if (i > 1) return;
    if (m_vGelenkPunkte[i].size() < 4) return;
    // G0-G3
    SGelenkPunkt const & G0 = m_vGelenkPunkte[i][0];
    SGelenkPunkt const & G1 = m_vGelenkPunkte[i][1];
    SGelenkPunkt const & G2 = m_vGelenkPunkte[i][2];
    SGelenkPunkt const & G3 = m_vGelenkPunkte[i][3];

    if (i==0)
        if (m_vGelenkPunkte[1].size() == 4)
            {
            LineWidth(cr, {2.5});   
            Color(cr, GRAY, .7);
            Line(cr,  G1, m_vGelenkPunkte[1][1]);
            Line(cr,  G2, m_vGelenkPunkte[1][2]);
            Line(cr,  G3, m_vGelenkPunkte[1][3]);
            }


    cr->save();

    LineWidth(cr, {2.5});
    SUmkreis const u = Umkreis(G1, G2, G3);
    Color(cr, WHITE, .2);
    Circle(cr, u.p, u.r);
    Color(cr, GRAY, .7);
    Ring(cr, u.p, u.r);
    Line(cr, u.p, G1);
    Line(cr, u.p, G2);
    Line(cr, u.p, G3);

    Color(cr, WHITE); Circle(cr,G1,5); Circle(cr,G2,5); Circle(cr,G3,5); 
    Color(cr, BLACK); Ring  (cr,G1,5); Ring  (cr,G2,5); Ring  (cr,G3,5); 
    SPoint const pto{.0,-11.3};
    std::string L{(char)(65+i)};
    draw_text(cr, G1 +pto, L + "1" , 1, true);
    draw_text(cr, G2 +pto, L + "2" , 1, true);
    draw_text(cr, G3 +pto, L + "3" , 1, true);

    DrawGehausePunkt(cr, G0, L + "0");

    cr->restore();
    }

void C3Lagen::DrawGehausePunkt(CairoCtx cr, SGehausePunkt const & G0,
                                            std::string   const & t)
    {
    double const d{11};
        
    cr->save();

    // G0 ------------------------------------------------
    Color(cr, WHITE);
    Circle(cr, G0, d);
    Color(cr, BLACK);
    LineWidth(cr, {2.5});
    Ring(cr,   G0, d);

    LineWidth(cr, {1});
    cr->move_to(G0.x,G0.y);
    cr->line_to(G0.x+4,G0.y);
    cr->arc(G0.x,G0.y,d,0,M_PI/2);
    cr->line_to(G0.x,G0.y);
    cr->fill();
    cr->move_to(G0.x,G0.y);
    cr->line_to(G0.x-4,G0.y);
    cr->arc(G0.x,G0.y,d,M_PI,M_PI/2*3);
    cr->line_to(G0.x,G0.y);
    cr->fill();

    cr->set_source_rgb(0,0,0);
    draw_text(cr, {G0.x,G0.y-d-7}, t);

    cr->restore();
    }
 
