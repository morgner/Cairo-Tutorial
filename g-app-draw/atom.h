#ifndef CATOM_H
#define CATOM_H

/**
 * @file atom.h
 *
 * @author Manfred Morgner
 * @date 10.10.2018
 */

#include <memory>
#include <iostream>
#include <vector>

#include "canvas.h"



using namespace std::string_literals;




inline bool MoveAtom(SRect & r, SPoint const & p)
    {
    r.p = p;
    return true;
    }

inline bool MoveAtom(SFrame & r, SPoint const & p)
    {
    r.p = p;
    return true;
    }

inline bool MoveAtom(SFleck & f, SPoint const & p)
    {
    f.x = p.x;
    f.y = p.y;
    return true;
    }

template<typename T>
    bool MoveAtom(T & t, SPoint const & p)
        {
        return false;
        }


inline SHit CollAtom(SPoint const & p, SFrame const & r)
    {
    if ( p.x > r.p.x && p.x < r.p.x+r.s.x && p.y < r.p.y && p.y > r.p.y+r.s.y )
        {
        return {true, p-r.p};
        }
    return {false, {.0,.0}};
    }

inline SHit CollAtom(SPoint const & p, SRect const & r)
    {
    if ( p.x > r.p.x && p.x < r.p.x+r.s.x && p.y < r.p.y && p.y > r.p.y+r.s.y )
        {
        return {true, p-r.p};
        }
    return {false, {.0,.0}};
    }

inline SHit CollAtom(SPoint const & p, SFleck const & f)
    {
    if ( Distance(f, p) < f.r )
        {
        return {true, p-SPoint{f.x,f.y}};
        }
    return {false, {.0,.0}};
    }

template<typename T>
    SHit CollAtom(SPoint const & p, T const & t)
        {
        return {false, {.0,.0}};
        }



inline void DrawAtom(Cairo::RefPtr<Cairo::Context> const & cr, SLineWidth const & w)
    {
    LineWidth( cr, w );
    }

inline void DrawAtom(Cairo::RefPtr<Cairo::Context> const & cr, SColor const & c)
    {
    Color( cr, c );
    }

inline void DrawAtom(Cairo::RefPtr<Cairo::Context> const & cr, SRect const & t)
    {
    Rectangle( cr, t.p, t.s );
    }

inline void DrawAtom(Cairo::RefPtr<Cairo::Context> const & cr, SFrame const & t)
    {
    Frame( cr, t.p, t.s );
    }

inline void DrawAtom(Cairo::RefPtr<Cairo::Context> const & cr, SFleck const & t)
    {
    cr->save();
    Color( cr, {.0,.0,.0} );
    Ring  ( cr, SPoint{t.x,t.y}, t.r );
    cr->restore();
    Circle( cr, SPoint{t.x,t.y}, t.r );
    }

template<typename T>
    void DrawAtom(Cairo::RefPtr<Cairo::Context> const & cr, T const & t)
        {
        Line( cr, t );
        }



class CAtom 
    {
    public:

        template<typename T>
            CAtom(T tAtomData)
                : m_pAtomData(new SAtomData<T>(std::move(tAtomData)))
                {
                }

    /// Destruction as usual (=default)
    virtual ~CAtom() noexcept = default;


    SPoint operator = (SPoint const & p)
        {
        m_pAtomData->move_element(p);
        return p;
        }

    bool ShiftAtom(SPoint const & p)
        {
        return m_pAtomData->move_element(p);
        }

    void ShowAtom(Cairo::RefPtr<Cairo::Context> const & cr) const
        {
        m_pAtomData->draw_element(cr);
        }

    SHit HitAtom(SPoint const & p) const
        {
        return m_pAtomData->hit_element(p);
        }


    /// start of data implementation
    struct SAtomDataConcept
        {
        virtual ~SAtomDataConcept() = default;
        /// Will send the data of the atom to the given output
        virtual void draw_element(Cairo::RefPtr<Cairo::Context> const &) const = 0;
        /// Will check for collision with given point
        virtual SHit hit_element(SPoint const &) const = 0;
        /// Will move geometric position
        virtual bool move_element(SPoint const &) = 0;
        }; // struct SAtomDataConcept

    /// The templated data structure to hold an arbitrary data element
    template<typename T>
    struct SAtomData : SAtomDataConcept
        {
        SAtomData(T tData) : m_tData(std::move(tData)) {}

        void draw_element(Cairo::RefPtr<Cairo::Context> const & cr) const
            {
            DrawAtom(cr, m_tData);
            }

        SHit hit_element(SPoint const & p) const
            {
            return CollAtom(p, m_tData);
            }

        bool move_element(SPoint const & p) 
            {
            return MoveAtom(m_tData, p);
            }

        /// @brief The decalartion of the data element of type T
        T m_tData;
        }; // struct SAtomData

    /// @brief The pointer and holder of the data element of type T
    std::unique_ptr<SAtomDataConcept> m_pAtomData;
    }; // class CAtom


// CATOM_H
#endif
