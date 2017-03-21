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
private:
    std::mutex lock;
    //static const HashEntry<Dimension> *HASHENTRY_NULL;//new HashEntry<Dimension>(-1, AABB<Dimension>(std::vector<unsigned>({1}), std::vector<unsigned>({1})));

public:
    int tableSize;
    std::atomic<std::uint32_t> *table;
    double conversionFactor;
    int cellSize;
    int width;
    uint32_t HASHENTRY_NULL = -1;


    //    template <unsigned int Dimension>
    SpatialHashTable(int size, double cFactor, int cWidth, int cSize): tableSize(size), conversionFactor(cFactor), width(cWidth), cellSize(cSize){
        table = new std::atomic<std::uint32_t>[tableSize];
        //HASHENTRY_NULL = new HashEntry<Dimension>(-1, AABB<Dimension>(std::vector<unsigned>({1}), std::vector<unsigned>({1})));
        for (int i = 0; i < tableSize; i++)
            table[i].store(-1);

        //conversionFactor = 0.04;

    }

   // template <unsigned int Dimension>
    int get(int key) {

        int hash = (key % tableSize);
        while (table[hash] != -1 && table[hash].load() != key)
            hash = (hash + 1) % tableSize;

        return table[hash].load();
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

    //template <unsigned int Dimension>
    void put(AABB<Dimension> aabb) {
        //add synchronization
        int posMin = getCell(aabb.minPoint);
        int posMax = getCell(aabb.maxPoint);


        if(posMin == posMax){
            //aabb falls into one cell
            //insert entry in table with atomic operations
            while(true){
                if(table[posMin].load()==-1){
                    if(table[posMin].compare_exchange_strong(HASHENTRY_NULL, aabb.minPoint[0]))
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
                    if(table[posMin].load()==-1){
                        if(table[posMin].compare_exchange_strong(HASHENTRY_NULL, aabb.minPoint[0]))
                            break;
                    }

                }

                // pos:Max
                while(true){
                    if(table[posMax].load()==-1){
                        if(table[posMax].compare_exchange_strong(HASHENTRY_NULL, aabb.minPoint[0]))
                            break;
                    }

                }

                // pos:3
                int pos3 = getCell(std::vector<unsigned>{aabb.maxPoint[0], aabb.minPoint[1]});
                if(pos3!=posMin && pos3!=posMax) {
                    while (true) {
                        if (table[pos3].load() == -1) {
                            if (table[pos3].compare_exchange_strong(HASHENTRY_NULL, aabb.minPoint[0]))
                                break;
                        }

                    }
                }

                // pos:4
                int pos4 = getCell(std::vector<unsigned>{aabb.minPoint[0], aabb.maxPoint[1]});
                if(pos4!=posMin && pos4!=posMax && pos4!=pos3) {
                    while (true) {
                        if (table[pos4].load() == -1) {
                            if (table[pos4].compare_exchange_strong(HASHENTRY_NULL, aabb.minPoint[0]))
                                break;
                        }

                    }
                }

            }else if(Dimension==3){
                //aabb falls in 2 or 4 or 8 cells
                // pos:Min
                while(true){
                    if(table[posMin].load()==-1){
                        if(table[posMin].compare_exchange_strong(HASHENTRY_NULL, aabb.minPoint[0]))
                            break;
                    }

                }

                // pos:Max
                while(true){
                    if(table[posMax].load()==-1){
                        if(table[posMax].compare_exchange_strong(HASHENTRY_NULL, aabb.minPoint[0]))
                            break;
                    }
                }

                // pos:3
                int pos3 = getCell(std::vector<unsigned>{aabb.minPoint[0], aabb.minPoint[1], aabb.maxPoint[2]});
                if(pos3!=posMin && pos3!=posMax) {
                    while (true) {
                        if (table[pos3].load() == -1) {
                            if (table[pos3].compare_exchange_strong(HASHENTRY_NULL, aabb.minPoint[0]))
                                break;
                        }

                    }
                }

                // pos:4
                int pos4 = getCell(std::vector<unsigned>{aabb.minPoint[0], aabb.maxPoint[1], aabb.minPoint[2]});
                if(pos4!=posMin && pos4!=posMax && pos4!=pos3) {
                    while (true) {
                        if (table[pos4].load() == -1) {
                            if (table[pos4].compare_exchange_strong(HASHENTRY_NULL, aabb.minPoint[0]))
                                break;
                        }
                    }
                }

                // pos:5
                int pos5 = getCell(std::vector<unsigned>{aabb.maxPoint[0], aabb.minPoint[1], aabb.minPoint[2]});
                if(pos5!=posMin && pos5!=posMax && pos5!=pos3 && pos5!=pos4) {
                    while (true) {
                        if (table[pos5].load() == -1) {
                            if (table[pos5].compare_exchange_strong(HASHENTRY_NULL, aabb.minPoint[0]))
                                break;
                        }
                    }
                }

                // pos:6
                int pos6 = getCell(std::vector<unsigned>{aabb.maxPoint[0], aabb.minPoint[1], aabb.minPoint[2]});
                if(pos6!=posMin && pos6!=posMax && pos6!=pos3 && pos6!=pos4 && pos6!=pos5) {
                    while (true) {
                        if (table[pos6].load() == -1) {
                            if (table[pos6].compare_exchange_strong(HASHENTRY_NULL, aabb.minPoint[0]))
                                break;
                        }
                    }
                }

                // pos:7
                int pos7 = getCell(std::vector<unsigned>{aabb.maxPoint[0], aabb.minPoint[1], aabb.maxPoint[2]});
                if(pos7!=posMin && pos7!=posMax && pos7!=pos3 && pos7!=pos4 && pos7!=pos5 && pos7!=pos6) {
                    while (true) {
                        if (table[pos7].load() == -1) {
                            if (table[pos7].compare_exchange_strong(HASHENTRY_NULL, aabb.minPoint[0]))
                                break;
                        }
                    }
                }

                // pos:8
                int pos8 = getCell(std::vector<unsigned>{aabb.maxPoint[0], aabb.maxPoint[1], aabb.minPoint[2]});
                if(pos8!=posMin && pos8!=posMax && pos8!=pos3 && pos8!=pos4 && pos8!=pos5 && pos8!=pos6 && pos8!=pos7) {
                    while (true) {
                        if (table[pos8].load() == -1) {
                            if (table[pos8].compare_exchange_strong(HASHENTRY_NULL, aabb.minPoint[0]))
                                break;
                        }
                    }
                }
            }

            lock.unlock();


        }

    }

    //template <unsigned int Dimension>
    void remove(AABB<Dimension> aabb) {
        int posMin = getCell(aabb.minPoint);
        int posMax = getCell(aabb.maxPoint);

        if(posMin == posMax){
            //aabb falls into one cell only
            //insert entry in table with atomic operations
            table[posMin].store(-1);

        }else{

            //acquire lock
            //lock.lock();
            // put entry into hash table
            if(Dimension==2){
                //aabb falls in 2 or 4 cells
                // pos:Min
                table[posMin].store(-1);

                // pos:Max
                table[posMax].store(-1);

                // pos:3
                int pos3 = getCell(std::vector<unsigned>{aabb.maxPoint[0], aabb.minPoint[1]});
                if(pos3!=posMin && pos3!=posMax)
                    table[pos3].store(-1);

                // pos:4
                int pos4 = getCell(std::vector<unsigned>{aabb.minPoint[0], aabb.maxPoint[1]});
                if(pos4!=posMin && pos4!=posMax && pos4!=pos3)
                    table[pos4].store(-1);

            }else if(Dimension==3){
                //aabb falls in 2 or 4 or 8 cells
                // pos:Min
                table[posMin].store(-1);

                // pos:Max
                table[posMax].store(-1);

                // pos:3
                int pos3 = getCell(std::vector<unsigned>{aabb.minPoint[0], aabb.minPoint[1], aabb.maxPoint[2]});
                if(pos3!=posMin && pos3!=posMax)
                    table[pos3].store(-1);

                // pos:4
                int pos4 = getCell(std::vector<unsigned>{aabb.minPoint[0], aabb.maxPoint[1], aabb.minPoint[2]});
                if(pos4!=posMin && pos4!=posMax && pos4!=pos3)
                    table[pos4].store(-1);

                // pos:5
                int pos5 = getCell(std::vector<unsigned>{aabb.maxPoint[0], aabb.minPoint[1], aabb.minPoint[2]});
                if(pos5!=posMin && pos5!=posMax && pos5!=pos3 && pos5!=pos4)
                    table[pos5].store(-1);

                // pos:6
                int pos6 = getCell(std::vector<unsigned>{aabb.maxPoint[0], aabb.minPoint[1], aabb.minPoint[2]});
                if(pos6!=posMin && pos6!=posMax && pos6!=pos3 && pos6!=pos4 && pos6!=pos5)
                    table[pos6].store(-1);

                // pos:7
                int pos7 = getCell(std::vector<unsigned>{aabb.maxPoint[0], aabb.minPoint[1], aabb.maxPoint[2]});
                if(pos7!=posMin && pos7!=posMax && pos7!=pos3 && pos7!=pos4 && pos7!=pos5 && pos7!=pos6)
                    table[pos7].store(-1);

                // pos:8
                int pos8 = getCell(std::vector<unsigned>{aabb.maxPoint[0], aabb.maxPoint[1], aabb.minPoint[2]});
                if(pos8!=posMin && pos8!=posMax && pos8!=pos3 && pos8!=pos4 && pos8!=pos5 && pos8!=pos6 && pos8!=pos7)
                    table[pos8].store(-1);
            }

            //lock.unlock();


        }

    }

    //template <unsigned int Dimension>
    ~SpatialHashTable() {
        delete[] table;
    }

    //template <unsigned int Dimension>
    int getCell(const std::vector<unsigned> point) {
        switch(Dimension){
            case 1: return (int)(point[0]*conversionFactor);
            case 2: return (point[0]/cellSize)+(point[1]/cellSize)*width;
            case 3: return (int)(point[0]*conversionFactor)+(int)(point[1]*conversionFactor)*width+(int)(point[2]*conversionFactor)*width;
        }
        return -1;
    }

    bool checkAvailabilityMinPoint(AABB<Dimension> aabb){
        int posMin = getCell(aabb.minPoint);
        return table[posMin].load()== -1?true:false;
    }




};




#endif //CONCURRENTSPATIALHASHING_SPATIALHASHTABLE_H
