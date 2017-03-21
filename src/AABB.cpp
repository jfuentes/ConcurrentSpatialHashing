//
// Created by Joel Fuentes on 3/16/17.
//

#include "AABB.h"

template<typename T, unsigned Dimension>
bool AABB<T, Dimension>::isEmpty() const {
    for (unsigned i = 0; i < Dimension; i++) {
        if (maxPoint[i] <= minPoint[i])
            return true;
    }
    return false;
}

template<typename T, unsigned Dimension>
bool AABB<T, Dimension>::overlaps(const AABB &other) const {
    /*
     * to check:
     *     maxX > other.minX &&
     *     minX < other.maxX &&
     *     maxY > other.minY &&
     *     minY < other.maxY &&
     *     maxZ > other.minZ &&
     *     minZ < other.maxZ;
     */
    for (unsigned i = 0; i < Dimension; i++) {
        if (maxPoint[i] < other.minPoint[i])
            return false;
        if (minPoint[i] > other.maxPoint[i])
            return false;
    }
    return true;
}

template<typename T, unsigned Dimension>
bool AABB<T, Dimension>::contains(const AABB &other) const {
    /*
     * to check:
     *     other.minX >= minX &&
     *     other.maxX <= maxX &&
     *     other.minY >= minY &&
     *     other.maxY <= maxY &&
     *     other.minZ >= minZ &&
     *     other.maxZ <= maxZ;
     */

    for (unsigned i = 0; i < Dimension; i++) {
        if (minPoint[i] < other.minPoint[i])
            return false;
        if (maxPoint[i] < other.maxPoint[i])
            return false;
    }
    return true;
}

template<typename T, unsigned Dimension>
AABB<T, Dimension> AABB<T, Dimension>::merge(const AABB<T, Dimension> &other) const {
    Point<T, Dimension> minMergedPoint, maxMergedPoint;

    for (unsigned i = 0; i < Dimension; i++) {
        minMergedPoint[i] = std::min(minPoint[i], other.minPoint[i]);
        maxMergedPoint[i] = std::max(maxPoint[i], other.maxPoint[i]);
    }

    return AABB(minMergedPoint, maxMergedPoint);

}

template<typename T, unsigned Dimension>
AABB<T, Dimension> AABB<T, Dimension>::intersection(const AABB<T, Dimension> &other) const {

    Point<T, Dimension> minMergedPoint, maxMergedPoint;

    for (unsigned i = 0; i < Dimension; i++) {
        minMergedPoint[i] = std::max(minPoint[i], other.minPoint[i]);
        maxMergedPoint[i] = std::min(maxPoint[i], other.maxPoint[i]);
    }
    return AABB(minMergedPoint, maxMergedPoint);

}

/*
 * dim : 1->width, 2->height, 3->depth
 */
template<typename T, unsigned Dimension>
unsigned AABB<T, Dimension>::getLength(const unsigned dim) {
    if (dim <= Dimension) {
        return maxPoint[dim - 1] - minPoint[dim - 1];
    }
    return 0;
}