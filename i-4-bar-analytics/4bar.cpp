#include "4bar.h"




bool C4Bar::MoveObject(CCanvas::SCollision const & tCollision, SPoint const & pos)
    {
    SPoint d{.0,.0};
    switch (tCollision.eWhat)
        {
        case CCanvas::SCollision::EWhat::A0:
            A0 = {pos - tCollision.tOffset};
            break;
        case CCanvas::SCollision::EWhat::A1:
            A1m = {pos - tCollision.tOffset};
            break;
        case CCanvas::SCollision::EWhat::B0:
            B0 = {pos - tCollision.tOffset};
            break;
        case CCanvas::SCollision::EWhat::B1:
            B1m = {pos - tCollision.tOffset};
            break;
        case CCanvas::SCollision::EWhat::a:
            d = (A1m-A0)/2;
            A0 = pos - tCollision.tOffset - d;
            A1m = pos - tCollision.tOffset + d;
            break;
        case CCanvas::SCollision::EWhat::b:
            d = (B1m-B0)/2;
            B0 = pos - tCollision.tOffset - d;
            B1m = pos - tCollision.tOffset + d;
            break;
        case CCanvas::SCollision::EWhat::c:
            d = (B1m-A1m)/2;
            A1m = pos - tCollision.tOffset - d;
            B1m = pos - tCollision.tOffset + d;
            break;
        case CCanvas::SCollision::EWhat::g:
            d = (B0-A0)/2;
            A0 = pos - tCollision.tOffset - d;
            B0 = pos - tCollision.tOffset + d;
            break;
        }
    return true;
    }

/*
    struct SCollision
        {
        SPoint tWhere  { .0,.0 };
        SPoint tOffset { .0,.0 };
        double dRadius { 20.0 };
        enum class EWhat
            {
            none,
            A0, A1, B0, B1,
            a, b, c, g
            }eWhat {EWhat::none};
        } m_tCollision;
*/

CCanvas::SCollision C4Bar::Collision(SPoint const & tPoint)
    {
    double const dRCol{13};
    CCanvas::SCollision tCollision;
    tCollision.eWhat = CCanvas::SCollision::EWhat::none;

    if ( Distance(A0, tPoint) < dRCol )
        {
        tCollision.tWhere  = tPoint;
        tCollision.tOffset = tPoint - A0;
        tCollision.dRadius = dRCol;
        tCollision.eWhat   = CCanvas::SCollision::EWhat::A0;
        return std::move(tCollision);
        }

    if ( Distance(A1m, tPoint) < dRCol )
        {
        tCollision.tWhere  = tPoint;
        tCollision.tOffset = tPoint - A1m;
        tCollision.dRadius = dRCol;
        tCollision.eWhat   = CCanvas::SCollision::EWhat::A1;
        return std::move(tCollision);
        }

    if ( Distance(B0, tPoint) < dRCol )
        {
        tCollision.tWhere  = tPoint;
        tCollision.tOffset = tPoint - B0;
        tCollision.dRadius = dRCol;
        tCollision.eWhat   = CCanvas::SCollision::EWhat::B0;
        return std::move(tCollision);
        }

    if ( Distance(B1m, tPoint) < dRCol )
        {
        tCollision.tWhere  = tPoint;
        tCollision.tOffset = tPoint - B1m;
        tCollision.dRadius = dRCol;
        tCollision.eWhat   = CCanvas::SCollision::EWhat::B1;
        return std::move(tCollision);
        }

    SPoint hit;
    
    if ( Distance(hit = (A0+A1m)/2, tPoint) < dRCol )
        {
        tCollision.tWhere  = tPoint;
        tCollision.tOffset = tPoint - hit;
        tCollision.dRadius = dRCol;
        tCollision.eWhat   = CCanvas::SCollision::EWhat::a;
        return std::move(tCollision);
        }

    if ( Distance(hit = (B0+B1m)/2, tPoint) < dRCol )
        {
        tCollision.tWhere  = tPoint;
        tCollision.tOffset = tPoint - hit;
        tCollision.dRadius = dRCol;
        tCollision.eWhat   = CCanvas::SCollision::EWhat::b;
        return std::move(tCollision);
        }

    if ( Distance(hit = (A1m+B1m)/2, tPoint) < dRCol )
        {
        tCollision.tWhere  = tPoint;
        tCollision.tOffset = tPoint - hit;
        tCollision.dRadius = dRCol;
        tCollision.eWhat   = CCanvas::SCollision::EWhat::c;
        return std::move(tCollision);
        }

    if ( Distance(hit = (A0+B0)/2, tPoint) < dRCol )
        {
        tCollision.tWhere  = tPoint;
        tCollision.tOffset = tPoint - hit;
        tCollision.dRadius = dRCol;
        tCollision.eWhat   = CCanvas::SCollision::EWhat::g;
        return std::move(tCollision);
        }

    return std::move(tCollision);
    }



void C4Bar::Show(CairoCtx cr, double const & dScale)
    {
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

void C4Bar::CalcGetriebe(CairoCtx cr, double const & ani, double const & dAni)
    {
    auto const GL = Distance( A0, B0 );
    auto const AL = Distance( A0, A1 );
    auto const BL = Distance( B0, B1 );
    auto const CL = Distance( A1, B1 );

    auto const  S = sin(ani*2*M_PI+M_PI);
    auto const  C = cos(ani*2*M_PI+M_PI);

              A1m = SPoint{A0.x+AL*S, A0.y+AL*C};
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

    LineWidth(cr, {2}); Color(cr, GRAY); Line(cr, A1m, B0);

    LineWidth(cr, {lw+2});
    Color(cr, BLACK);
    if ( m_nSplit != D_OK )
        {
        Color(cr, BLACK);
        Line(cr, A0, A1m);
        Line(cr, A0, B0);
        if ( m_nSplit == D2LONG  ) { Color(cr, YELLOW); };
        if ( m_nSplit == D2SHORT ) { Color(cr, CYAN  ); };
        Line(cr, A1m, B0);
        }
    else
        {
        Color(cr, BLACK);
        Line(cr, A0, B0);
        Line(cr, A0, A1m);
        Line(cr, A1m, pab);
        Line(cr, B0, B1m);
        }

    Ring(cr, A1m, 3);
    Ring(cr, B1m, 3);

    Color(cr, WHITE);

    Circle(cr, A1m, 3);
    Circle(cr, B1m, 3);

    
    DrawGehausePunkt(cr, A0, "A0");
    DrawGehausePunkt(cr, B0, "B0");

    Color(cr, BLACK);
    SPoint const pto{.0,.0};
    draw_text(cr, (A0 +A1m)/2+pto, std::to_string((int)AL),1,true);
    draw_text(cr, (A1m+pab)/2+pto, std::to_string((int)CL),1,true);
    draw_text(cr, (B0 +B1m)/2+pto, std::to_string((int)BL),1,true);
    draw_text(cr, (A0 +B0 )/2+pto, std::to_string((int)GL),1,true);

    Color(cr, BLACK);
    draw_text(cr, (A1m+B0 )/2+pto, std::to_string((int)d),1,true);

/*
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
*/
    }

void C4Bar::DrawGetriebe(CairoCtx cr, double const & ani, double const & dAni)
    {
    auto const GL = Distance( A0, B0 );
    auto const AL = Distance( A0, A1m );
    auto const BL = Distance( B0, B1m );
    auto const CL = Distance( A1m, B1m );
    auto const  d = Distance( A1m, B0 );

    double const lw{7};
    LineWidth(cr, {lw});
    Color(cr, BLACK);

    LineWidth(cr, {2}); Color(cr, GRAY); Line(cr, A1m, B0);

    LineWidth(cr, {lw+2});
    Color(cr, BLACK);

    Line(cr, A0, B0);
    Line(cr, A0, A1m);
    Line(cr, A1m, B1m);
    Line(cr, B0, B1m);

    Ring(cr, A1m, 3);
    Ring(cr, B1m, 3);

    Color(cr, WHITE);

    Circle(cr, A1m, 3);
    Circle(cr, B1m, 3);

    
    DrawGehausePunkt(cr, A0, "A0");
    DrawGehausePunkt(cr, B0, "B0");

    Color(cr, BLACK);
    SPoint const pto{.0,.0};
    draw_text(cr, (A0+A1m)/2+pto, std::to_string((int)AL),1,true);
    draw_text(cr, (A1m+B1m)/2+pto, std::to_string((int)CL),1,true);
    draw_text(cr, (B0+B1m)/2+pto, std::to_string((int)BL),1,true);
    draw_text(cr, (A0+B0)/2+pto, std::to_string((int)GL),1,true);

    Color(cr, BLACK);
    draw_text(cr, (A1m+B0)/2+pto, std::to_string((int)d),1,true);

    }

void C4Bar::DrawGehausePunkt(CairoCtx cr, SPoint      const & G0,
                                          std::string const & t = "G0")
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
