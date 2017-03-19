//
// Created by Joel Fuentes on 3/16/17.
//

#ifndef CONCURRENTSPATIALHASHING_SPATIALHASHTABLE_H
#define CONCURRENTSPATIALHASHING_SPATIALHASHTABLE_H

#include "AABB.h"
#include <mutex>

template <unsigned int Dimension>
class HashEntry {
public:
    int key;
    AABB<Dimension> value;  //we allow for now only one bbox per position in the hash table
    // to allow more than one, use a vector/list of bboxes

    HashEntry(int key, AABB<Dimension> value): key(key), value(value){}
};

template <unsigned int Dimension>
class SpatialHashTable {
public:
    int tableSize;
    std::atomic<HashEntry<Dimension>*> *table;
    float conversionFactor;
    int cellWidth;

    SpatialHashTable(int size, float cFactor, int cWidth);
    AABB<Dimension> get(int key);
    void put(AABB<Dimension> aabb);
    void remove(AABB<Dimension> aabb);
    ~SpatialHashTable();

private:
    std::mutex lock;
    int getCell(const std::vector<unsigned> point);
};




#endif //CONCURRENTSPATIALHASHING_SPATIALHASHTABLE_H
