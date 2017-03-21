
#ifndef CONCURRENTSPATIALHASHING_AABB_H
#define CONCURRENTSPATIALHASHING_AABB_H

#include <vector>
#include "Point.h"

template<typename T, unsigned Dimension>
class AABB {

public:

    // info about axis aligned bounding box
    Point<T, Dimension> minPoint;
    Point<T, Dimension> maxPoint;

    AABB() {}
    AABB(Point<T, Dimension> min, Point<T, Dimension> max) : minPoint(min), maxPoint(max) {};
    bool isEmpty() const;
    bool overlaps(const AABB &other) const;
    bool contains(const AABB &other) const;
    AABB merge(const AABB &other) const;
    AABB intersection(const AABB &other) const;
    /*
     * dim : 1->width, 2->height, 3->depth
     */
    unsigned getLength(const unsigned dim);


};


#endif //CONCURRENTSPATIALHASHING_AABB_H
