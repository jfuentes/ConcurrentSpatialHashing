//
// Created by Joel Fuentes on 3/16/17.
//

#include "SpatialHashTable.h"


template <unsigned int Dimension>
SpatialHashTable<Dimension>::SpatialHashTable(int size, float cFactor, int cWidth): tableSize(size), conversionFactor(cFactor), cellWidth(cWidth){
    table = new std::atomic<HashEntry<Dimension>*>[tableSize];
    for (int i = 0; i < tableSize; i++)
        table[i].store(nullptr, std::memory_order_relaxed);
}

template <unsigned int Dimension>
AABB<Dimension> SpatialHashTable<Dimension>::get(int key) {

    int hash = (key % tableSize);
    while (table[hash] != nullptr && table[hash].load(std::memory_order_relaxed)->key != key)
        hash = (hash + 1) % tableSize;

    return table[hash].load(std::memory_order_relaxed)->value;
}

/*
 * Obs: to insert an element in the hash table, the points min and max from the AABB are used
 * if both min and max fall into the same cell, atomic operations are used to try to insert the entry in the table;
 * else a unique lock is used to insert the entry in 2 or 4 cells if 2D; up to 8 if 3D
 *
 * if hash(min) != hash(max) then
 *
 * if 2D:
 *      check vertices:
 *          1) (min.x, min.y)
 *          2) (max.x, max.y)
 *          3) (max.x, min.y)
 *          4) (min.x, max.y)
 *
 * if 3D:
 *      check vertices:
 *          1) (min.x, min.y, min.z)
 *          2) (max.x, max.y, max.z)
 *          3) (min.x, min.y, max.z)
 *          4) (min.x, max.y, min.z)
 *          5) (max.x, min.y, min.z)
 *          6) (min.x, max.y, max.z)
 *          7) (max.x, min.y, max.z)
 *          8) (max.x, max.y, min.z)
 */
template <unsigned int Dimension>
void SpatialHashTable<Dimension>::put(AABB<Dimension> aabb) {
    //add synchronization
    int posMin = getCell(aabb.minPoint);
    int posMax = getCell(aabb.maxPoint);

    if(posMin == posMax){
        //aabb falls into one cell
        //insert entry in table with atomic operations
        while(true){
            if(table[posMin].load(std::memory_order_relaxed)==nullptr){
                if(table[posMin].compare_exchange_weak(nullptr, new HashEntry<Dimension>(posMin, aabb), std::memory_order_release, std::memory_order_relaxed))
                    break;
            }

        }
    }else{

        //acquire lock
        lock.lock();
        // put entry into hash table
        if(Dimension==2){
            //aabb falls in 2 or 4 cells
            // pos:Min
            while(true){
                if(table[posMin].load(std::memory_order_relaxed)==nullptr){
                    if(table[posMin].compare_exchange_weak(nullptr, new HashEntry<Dimension>(posMin, aabb), std::memory_order_release, std::memory_order_relaxed))
                        break;
                }

            }

            // pos:Max
            while(true){
                if(table[posMax].load(std::memory_order_relaxed)==nullptr){
                    if(table[posMax].compare_exchange_weak(nullptr, new HashEntry<Dimension>(posMax, aabb), std::memory_order_release, std::memory_order_relaxed))
                        break;
                }

            }

            // pos:3
            int pos3 = getCell(std::vector<unsigned>{aabb.maxPoint[0], aabb.minPoint[1]});
            if(pos3!=posMin && pos3!=posMax) {
                while (true) {
                    if (table[pos3].load(std::memory_order_relaxed) == nullptr) {
                        if (table[pos3].compare_exchange_weak(nullptr, new HashEntry<Dimension>(pos3, aabb),
                                                              std::memory_order_release, std::memory_order_relaxed))
                            break;
                    }

                }
            }

            // pos:4
            int pos4 = getCell(std::vector<unsigned>{aabb.minPoint[0], aabb.maxPoint[1]});
            if(pos4!=posMin && pos4!=posMax && pos4!=pos3) {
                while (true) {
                    if (table[pos4].load(std::memory_order_relaxed) == nullptr) {
                        if (table[pos4].compare_exchange_weak(nullptr, new HashEntry<Dimension>(pos4, aabb),
                                                              std::memory_order_release, std::memory_order_relaxed))
                            break;
                    }

                }
            }

        }else if(Dimension==3){
            //aabb falls in 2 or 4 or 8 cells
            // pos:Min
            while(true){
                if(table[posMin].load(std::memory_order_relaxed)==nullptr){
                    if(table[posMin].compare_exchange_weak(nullptr, new HashEntry<Dimension>(posMin, aabb), std::memory_order_release, std::memory_order_relaxed))
                        break;
                }

            }

            // pos:Max
            while(true){
                if(table[posMax].load(std::memory_order_relaxed)==nullptr){
                    if(table[posMax].compare_exchange_weak(nullptr, new HashEntry<Dimension>(posMax, aabb), std::memory_order_release, std::memory_order_relaxed))
                        break;
                }
            }

            // pos:3
            int pos3 = getCell(std::vector<unsigned>{aabb.minPoint[0], aabb.minPoint[1], aabb.maxPoint[2]});
            if(pos3!=posMin && pos3!=posMax) {
                while (true) {
                    if (table[pos3].load(std::memory_order_relaxed) == nullptr) {
                        if (table[pos3].compare_exchange_weak(nullptr, new HashEntry<Dimension>(pos3, aabb),
                                                              std::memory_order_release, std::memory_order_relaxed))
                            break;
                    }

                }
            }

            // pos:4
            int pos4 = getCell(std::vector<unsigned>{aabb.minPoint[0], aabb.maxPoint[1], aabb.minPoint[2]});
            if(pos4!=posMin && pos4!=posMax && pos4!=pos3) {
                while (true) {
                    if (table[pos4].load(std::memory_order_relaxed) == nullptr) {
                        if (table[pos4].compare_exchange_weak(nullptr, new HashEntry<Dimension>(pos4, aabb),
                                                              std::memory_order_release, std::memory_order_relaxed))
                            break;
                    }
                }
            }

            // pos:5
            int pos5 = getCell(std::vector<unsigned>{aabb.maxPoint[0], aabb.minPoint[1], aabb.minPoint[2]});
            if(pos5!=posMin && pos5!=posMax && pos5!=pos3 && pos5!=pos4) {
                while (true) {
                    if (table[pos5].load(std::memory_order_relaxed) == nullptr) {
                        if (table[pos5].compare_exchange_weak(nullptr, new HashEntry<Dimension>(pos5, aabb),
                                                              std::memory_order_release, std::memory_order_relaxed))
                            break;
                    }
                }
            }

            // pos:6
            int pos6 = getCell(std::vector<unsigned>{aabb.maxPoint[0], aabb.minPoint[1], aabb.minPoint[2]});
            if(pos6!=posMin && pos6!=posMax && pos6!=pos3 && pos6!=pos4 && pos6!=pos5) {
                while (true) {
                    if (table[pos6].load(std::memory_order_relaxed) == nullptr) {
                        if (table[pos6].compare_exchange_weak(nullptr, new HashEntry<Dimension>(pos6, aabb),
                                                              std::memory_order_release, std::memory_order_relaxed))
                            break;
                    }
                }
            }

            // pos:7
            int pos7 = getCell(std::vector<unsigned>{aabb.maxPoint[0], aabb.minPoint[1], aabb.maxPoint[2]});
            if(pos7!=posMin && pos7!=posMax && pos7!=pos3 && pos7!=pos4 && pos7!=pos5 && pos7!=pos6) {
                while (true) {
                    if (table[pos7].load(std::memory_order_relaxed) == nullptr) {
                        if (table[pos7].compare_exchange_weak(nullptr, new HashEntry<Dimension>(pos7, aabb),
                                                              std::memory_order_release, std::memory_order_relaxed))
                            break;
                    }
                }
            }

            // pos:8
            int pos8 = getCell(std::vector<unsigned>{aabb.maxPoint[0], aabb.maxPoint[1], aabb.minPoint[2]});
            if(pos8!=posMin && pos8!=posMax && pos8!=pos3 && pos8!=pos4 && pos8!=pos5 && pos8!=pos6 && pos8!=pos7) {
                while (true) {
                    if (table[pos8].load(std::memory_order_relaxed) == nullptr) {
                        if (table[pos8].compare_exchange_weak(nullptr, new HashEntry<Dimension>(pos8, aabb),
                                                              std::memory_order_release, std::memory_order_relaxed))
                            break;
                    }
                }
            }
        }

        lock.unlock();


    }

}

template <unsigned int Dimension>
void SpatialHashTable<Dimension>::remove(AABB<Dimension> aabb) {
    int pos1 = getCell(aabb.minPoint);
    int pos2 = getCell(aabb.maxPoint);

    if(pos1 == pos2){
        //aabb falls into one cell
        //insert entry in table with atomic operations
        table[pos1].store(nullptr, std::memory_order_release);
    }else{
        //aabb falls in 2 or 4 cells


    }

}

template <unsigned int Dimension>
SpatialHashTable<Dimension>::~SpatialHashTable() {
    for (int i = 0; i < tableSize; i++)
        if (table[i].load() != nullptr)
            delete table[i];
    delete[] table;
}

template <unsigned int Dimension>
int SpatialHashTable<Dimension>::getCell(const std::vector<unsigned> point) {
    switch(Dimension){
        case 1: return (int)(point[0]*conversionFactor);
        case 2: return (int)(point[0]*conversionFactor)+(int)(point[1]*conversionFactor)*cellWidth;
        case 3: return (int)(point[0]*conversionFactor)+(int)(point[1]*conversionFactor)*cellWidth+(int)(point[2]*conversionFactor)*cellWidth;
    }
    return -1;

}