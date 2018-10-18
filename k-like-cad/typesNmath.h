#ifndef TYPES_AND_MATH_H
#define TYPES_AND_MATH_H



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
    }; // struct SPoint

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

struct SLine
    {
    SPoint a{0,0};
    SPoint b{0,0};
    };
    
using SEbene = SLine;

struct SRect
    {
    SPoint p{0,0};
    SPoint s{0,0};
    };

struct SFrame
    {
    SPoint p{0,0};
    SPoint s{0,0};
    };
    
struct SUmkreis
    {
    double r{0};
    SPoint p{.0,.0};
    };

struct SLineWidth
    {
    double w{1};
    };

struct SColor
    {
    double r{0},g{0},b{0};
    };

struct SHit
    {
    bool   bHit{false};
    SPoint tOffset{.0,.0};
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


SColor constexpr BLACK      {.0,.0,.0};
SColor constexpr DARKRED    {.6,.0,.0};
SColor constexpr DARKGREEN  {.0,.6,.0};
SColor constexpr DARKBLUE   {.0,.0,.6};
SColor constexpr DARKCYAN   {.0,.6,.6};
SColor constexpr DARKMAGENTA{.6,.0,.6};
SColor constexpr DARKYELLOW {.6,.6,.0};
SColor constexpr GRAY       {.6,.6,.6};
SColor constexpr RED        {1,0,0};
SColor constexpr GREEN      {0,1,0};
SColor constexpr BLUE       {0,0,1};
SColor constexpr CYAN       {0,1,1};
SColor constexpr MAGENTA    {1,0,1};
SColor constexpr YELLOW     {1,1,0};
SColor constexpr WHITE      {1,1,1};

int constexpr TEXT_LEFT   {0};
int constexpr TEXT_MIDDLE {1};
int constexpr TEXT_RIGHT  {2};


// calculation helpers

// angel at triangle point a of triangle a-b-c
inline double Alpha(double const & a, double const & b, double const & c)
    {
    auto cosinus = (pow(b,2)+pow(c,2)-pow(a,2)) / (2*b*c);
    if ((cosinus<-1)||(cosinus>1))
        {
        return 0.0;
        }
    return acos( cosinus );
    }

inline double VectorSlope(SPoint const & a, SPoint const & b)
    {
    auto const dba = SPoint{ b.x-a.x, b.y-a.y };
    double dbax{dba.x};
    if ( dba.x < 0 && dba.x > -.00001) dbax = -.00001;
    if ( dba.x > 0 && dba.x <  .00001) dbax =  .00001;
    auto       sba = atan(dba.y / dbax);

    if      ( (dba.x>0) && (dba.y<0) )
        sba = -sba;
    else if ( (dba.x<0) && (dba.y<0) )
        sba = M_PI - sba;
    else if ( (dba.x<0) && (dba.y>0) )
        sba = M_PI - sba;
    else if ( (dba.x>0) && (dba.y>0) )
        sba = 2*M_PI - sba;

    return sba;
    }

inline double VectorAngleDiff(SLine const & a, SLine const & b)
    {
    auto   sla = VectorSlope(a.a,a.b);
    auto   slb = VectorSlope(b.a,b.b);
    return sla - slb;
    }

inline double VectorDiff(SPoint const & a, SPoint const & b, SPoint const & c)
    {
    auto slb = VectorSlope(a,b);
    auto slc = VectorSlope(a,c);
    return slc - slb;
    }

template<typename P, typename T>
    double Distance( P const & a, T const & b )
        {
        return sqrt( pow((a.x-b.x),2) + pow((a.y-b.y),2) );
        }



inline SEbene FixedLenLine(SLine & roL, double const & crnLen, bool const & crbFirst)
    {
    auto const dx   { roL.a.x - roL.b.x };
    auto const dy   { roL.a.y - roL.b.y };
    auto const nLen { sqrt(dx*dx + dy*dy) };
    auto const q    { (double)crnLen / ((nLen!=0)?nLen:1) };
    if (crbFirst)
        {
        roL.b.x = roL.a.x - dx*q;
        roL.b.y = roL.a.y - dy*q;
        }
    else
        {
        roL.a.x = roL.b.x + dx*q;
        roL.a.y = roL.b.y + dy*q;
        }
    return roL;
    } // void FixedLenLine(...



inline SPoint Intersection(SLine const & E1, SLine const & E2)
    {
    auto const dx1 { E1.b.x - E1.a.x };
    auto const dx2 { E2.b.x - E2.a.x };

    auto const m1 { (E1.b.y - E1.a.y) / ((dx1==0)?1e-9:dx1) }; // Steigungen ermitteln
    auto const m2 { (E2.b.y - E2.a.y) / ((dx2==0)?1e-9:dx2) };

    // if (ROUND(m1,MAX_ACCURACY)==ROUND(m2,MAX_ACCURACY)) return false; // Geraden sind parallel

    auto const n1 { E1.a.y - (m1*E1.a.x) }; // Abstände von X-Achse ermitteln
    auto const n2 { E2.a.y - (m2*E2.a.x) };

    auto const  x { (n2-n1)/(m1-m2) }; // Schnittpunktkoordinate berechnen
    auto const  y { m1*x+n1 };

    return { x, y };
  } // Intersection


inline SLine Perpendicle(SLine const & l)
    {
    auto const dx = (l.b.x - l.a.x)/2.0;
    auto const dy = (l.b.y - l.a.y)/2.0;

    SLine I{ {l.b.x - dy - dx,
              l.b.y + dx - dy},
             {l.b.x + dy - dx,
              l.b.y - dx - dy} };

    return std::move(I);
    }

inline SPoint PointMirror(SPoint const & p, SLine const & m)
    {
    auto const dx = (m.b.x - m.a.x)/2.0;
    auto const dy = (m.b.y - m.a.y)/2.0;

    SLine I{ {p.x - dy,
              p.y + dx},
             {p.x + dy,
              p.y - dx} };

    auto const S  { Intersection(m, I) };
    auto const mx { (m.b.x + m.a.x)/2.0 };
    auto const my { (m.b.y + m.a.y)/2.0 };

    return { S.x + (S.x - p.x), S.y + (S.y - p.y) };
    }

inline SPoint Polpunkt(SEbene const & E1, SEbene const & E2)
    {
    SEbene const L1{ E1.a, E2.a };
    auto   const La{ Perpendicle(L1) };

    SEbene const L2{ E1.b, E2.b };
    auto   const Lb{ Perpendicle(L2) };

    return std::move(Intersection(La, Lb));
    }

inline SUmkreis Umkreis( SPoint const & P1, SPoint const & P2, SPoint const & P3 )
    {
    SLine const L1{{P1.x, P1.y},{P2.x, P2.y}};
    auto  const La{ Perpendicle(L1) };

    SLine const L2{{P1.x, P1.y},{P3.x, P3.y}};
    auto  const Lb{ Perpendicle(L2) };

    auto const M{Intersection(La, Lb)};
    auto const R{sqrt( (double)(M.x - L1.a.x)*(M.x - L1.a.x) + (M.y - L1.a.y)*(M.y - L1.a.y) )};

    return { R, M };
    }


// TYPES_AND_MATH_H
#endif
