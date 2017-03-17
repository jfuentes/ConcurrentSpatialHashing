
#ifndef CONCURRENTSPATIALHASHING_AABB_H
#define CONCURRENTSPATIALHASHING_AABB_H

#include <algorithm>
#include <vector>

template <unsigned int Dimension>
    class AABB
    {


    public:

        // info about axis aligned bounding box
        std::vector<unsigned> minPoint;
        std::vector<unsigned> maxPoint;


        AABB()
        {

        }


        AABB(std::vector<unsigned> min, std::vector<unsigned> max) : minPoint(min), maxPoint(max)
        {
        }


        bool isEmpty() const
        {
            for(unsigned i=0; i<Dimension; i++){
                if(maxPoint[i]<=minPoint[i])
                    return true;
            }
            return false;
        }

        bool overlaps(const AABB& other) const
        {
            /*
             * to check:
             *     maxX > other.minX &&
             *     minX < other.maxX &&
             *     maxY > other.minY &&
             *     minY < other.maxY &&
             *     maxZ > other.minZ &&
             *     minZ < other.maxZ;
             */
            for(unsigned i=0; i<Dimension; i++){
                if(maxPoint[i]<other.minPoint[i])
                    return false;
                if(minPoint[i]>other.maxPoint[i])
                    return false;
            }
            return true;
        }

        bool contains(const AABB& other) const
        {
            /*
             * to check:
             *     other.minX >= minX &&
             *     other.maxX <= maxX &&
             *     other.minY >= minY &&
             *     other.maxY <= maxY &&
             *     other.minZ >= minZ &&
             *     other.maxZ <= maxZ;
             */

            for(unsigned i=0; i<Dimension; i++){
                if(minPoint[i]<other.minPoint[i])
                    return false;
                if(maxPoint[i]<other.maxPoint[i])
                    return false;
            }
            return true;
        }

        AABB merge(const AABB& other) const
        {
            std::vector<unsigned> minMergedPoint(Dimension), maxMergedPoint(Dimension);

            for(unsigned i=0; i<Dimension; i++){
                minMergedPoint[i] = std::min(minPoint[i], other.minPoint[i]);
                maxMergedPoint[i] = std::max(maxPoint[i], other.maxPoint[i]);
            }

            return AABB(minMergedPoint, maxMergedPoint);

        }

        AABB intersection(const AABB& other) const
        {

            std::vector<unsigned> minMergedPoint(Dimension), maxMergedPoint(Dimension);

            for(unsigned i=0; i<Dimension; i++){
                minMergedPoint[i] = std::max(minPoint[i], other.minPoint[i]);
                maxMergedPoint[i] = std::min(maxPoint[i], other.maxPoint[i]);
            }
            return AABB(minMergedPoint, maxMergedPoint);

        }

        /*
         * dim : 1->width, 2->height, 3->depth
         */
        unsigned getLength(const unsigned dim){
            if(dim<=Dimension){
                return maxPoint[dim-1] - minPoint[dim-1];
            }
            return 0;
        }


    };


#endif //CONCURRENTSPATIALHASHING_AABB_H
