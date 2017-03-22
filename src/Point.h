//
// Created by Joel Fuentes on 3/21/17.
//

#ifndef CONCURRENTSPATIALHASHING_POINT_H
#define CONCURRENTSPATIALHASHING_POINT_H

#include <array>

#define STATIC_ASSERT( e ) static_assert( e, "!(" #e ")" )

template< typename T, unsigned Dimension>
class Point
{
private:
    std::array< T, Dimension > elements_;

public:
    typedef T ValueType;

    T& operator[]( int const i )
    {
        return elements_[i];
    }

    T const& operator[]( int const i ) const
    {
        return elements_[i];
    }

    void operator+=( Point const& other )
    {
        for( int i = 0; i < Dimension; ++i )
        {
            elements_[i] += other.elements_[i];
        }
    }

    void operator-=( Point const& other )
    {
        for( int i = 0; i < Dimension; ++i )
        {
            elements_[i] -= other.elements_[i];
        }
    }

    friend Point operator+( Point const& a, Point const& b )
    {
        Point ret( a );

        ret += b;
        return ret;
    }

    friend Point operator-( Point const&a, Point const& b )
    {
        Point ret( a );

        ret -= b;
        return ret;
    }

    Point(): elements_() {}

    Point( int x, int y )
    {
        //STATIC_ASSERT( Dimension == 2 );
        elements_[0] = x;
        elements_[1] = y;
    }

    Point( int x, int y, int z )
    {
        //STATIC_ASSERT( Dimension == 3 );
        elements_[0] = x;
        elements_[1] = y;
        elements_[2] = z;
    }
};

#endif //CONCURRENTSPATIALHASHING_POINT_H
