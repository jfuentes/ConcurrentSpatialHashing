//
// Created by Joel Fuentes on 3/16/17.
//

#ifndef CONCURRENTSPATIALHASHING_SPATIALHASHTABLE_H
#define CONCURRENTSPATIALHASHING_SPATIALHASHTABLE_H

#include "AABB.h"

template <unsigned int Dimension>
class HashEntry {
private:
    int key;
    AABB<Dimension> value;  //we allow for now only one bbox per position in the hash table
    // to allow more than one, use a vector/list of bboxes
public:
    HashEntry(int key, AABB<Dimension> value) {
        this->key = key;
        this->value = value;
    }

    int getKey() {
        return key;
    }

    AABB<Dimension> getValue() {
        return value;
    }
};

template <unsigned int Dimension>
class SpatialHashTable {
private:
    int tableSize;
    HashEntry<Dimension> **table;
    float conversionFactor;
    int cellWidth;

public:
    SpatialHashTable(int size, float cFactor, int cWidth) {
        tableSize = size;
        conversionFactor = cFactor;
        cellWidth = cWidth;
        table = new HashEntry<Dimension>*[tableSize];
        for (int i = 0; i < tableSize; i++)
            table[i] = nullptr;
    }

    AABB<Dimension> get(int key) {

        int hash = (key % tableSize);
        while (table[hash] != nullptr && table[hash]->getKey() != key)
            hash = (hash + 1) % tableSize;

        return table[hash]->getValue();
    }

    void put(int key, AABB<Dimension> value) {
        //add synchronization
        int hash = (key % tableSize);
        while (table[hash] != nullptr && table[hash]->getKey() != key)
            hash = (hash + 1) % tableSize;
        if (table[hash] != nullptr)
            delete table[hash];
        table[hash] = new HashEntry<Dimension>(key, value);
    }

    ~SpatialHashTable() {
        for (int i = 0; i < tableSize; i++)
            if (table[i] != nullptr)
                delete table[i];
        delete[] table;
    }

private:
    int getCell(float x, float y, float z) {
        return (int)(x*conversionFactor)+(int)(y*conversionFactor)*cellWidth+(int)(z*conversionFactor)*cellWidth;
    }

};




#endif //CONCURRENTSPATIALHASHING_SPATIALHASHTABLE_H
