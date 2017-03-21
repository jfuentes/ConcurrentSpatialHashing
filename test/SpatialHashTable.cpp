//
// Created by Joel Fuentes on 3/18/17.
//

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hh"
#include "../src/SpatialHashTable.h"
#include <tbb/parallel_for.h>
#include <tbb/task_scheduler_init.h>
#include <thread>
#include <tbb/tick_count.h>



//work to be executed by threads
struct ParallelWork {
    SpatialHashTable<2> *hashTable2d;
    std::vector<AABB<2>> aabbs;
    void operator()( const tbb::blocked_range<int>& range ) const {
        for( int i=range.begin(); i!=range.end(); ++i ) {
            const auto aabb = aabbs[i];
            //std::cout << "thread instering " << aabb.minPoint[0]<< "," << aabb.minPoint[1] << " and " << aabb.maxPoint[0]<< "," << aabb.maxPoint[1] << std::endl;
            hashTable2d->put(aabb);
            // Sleep 
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            hashTable2d->remove(aabb);
            //std::cout << "thread frees " << aabb.minPoint[0]<< "," << aabb.minPoint[1] << " and " << aabb.maxPoint[0]<< "," << aabb.maxPoint[1] << std::endl;

        }
    }


};



TEST_CASE( "HashTable creation 2D", "[hashtable]" ) {
    //float factor = ;
    SpatialHashTable<2> hashTable2D(16,1.0/25,4, 25);
    REQUIRE( hashTable2D.getCell(std::vector<unsigned>{0,0}) == 0 );
    REQUIRE( hashTable2D.getCell(std::vector<unsigned>{5,5}) == 0 );
    REQUIRE( hashTable2D.getCell(std::vector<unsigned>{99,99}) == 15 );
    REQUIRE( hashTable2D.getCell(std::vector<unsigned>{25,40}) == 5 );
    REQUIRE( hashTable2D.getCell(std::vector<unsigned>{25,50}) == 9 );
    REQUIRE( hashTable2D.getCell(std::vector<unsigned>{99,99}) == 15 );
}


TEST_CASE( "HashTable insertion 2D", "[hashtableInsertion1]" ) {
    //float factor = ;
    SpatialHashTable<2> hashTable2D(16,1.0/25,4, 25);
    AABB<2> aabb1(std::vector<unsigned>{0,0},std::vector<unsigned>{2,2});
    AABB<2> aabb2(std::vector<unsigned>{1,1},std::vector<unsigned>{3,3});

    SECTION("Inserting and removing AABB 1 cell"){
        REQUIRE(hashTable2D.checkAvailabilityMinPoint(aabb1) == true);
        hashTable2D.put(aabb1);
        REQUIRE(hashTable2D.checkAvailabilityMinPoint(aabb2) == false);
        hashTable2D.remove(aabb1);
        REQUIRE(hashTable2D.checkAvailabilityMinPoint(aabb2) == true);
    }

    AABB<2> aabb3(std::vector<unsigned>{20,2},std::vector<unsigned>{30,10}); // should take 2 cells
    AABB<2> aabb4(std::vector<unsigned>{33,5},std::vector<unsigned>{45,12});

    SECTION("Inserting and removing AABB 2 cells"){
        //REQUIRE(hashTable2D.checkAvailabilityMinPoint(aabb1) == true);
        hashTable2D.put(aabb3);
        REQUIRE(hashTable2D.checkAvailabilityMinPoint(aabb4) == false);
        hashTable2D.remove(aabb3);
        REQUIRE(hashTable2D.checkAvailabilityMinPoint(aabb4) == true);
    }

    AABB<2> aabb5(std::vector<unsigned>{20,20},std::vector<unsigned>{30,30}); // should take 4 cells
    AABB<2> aabb6(std::vector<unsigned>{10,30},std::vector<unsigned>{20,40});
    AABB<2> aabb7(std::vector<unsigned>{30,30},std::vector<unsigned>{40,40});

    SECTION("Inserting and removing AABB 4 cells"){
        //REQUIRE(hashTable2D.checkAvailabilityMinPoint(aabb1) == true);
        hashTable2D.put(aabb5);
        REQUIRE(hashTable2D.checkAvailabilityMinPoint(aabb1) == false); //first cell
        REQUIRE(hashTable2D.checkAvailabilityMinPoint(aabb3) == false); //2nd cell
        REQUIRE(hashTable2D.checkAvailabilityMinPoint(aabb6) == false); //3rd cell
        REQUIRE(hashTable2D.checkAvailabilityMinPoint(aabb7) == false); //4th cell
        hashTable2D.remove(aabb5);
        REQUIRE(hashTable2D.checkAvailabilityMinPoint(aabb1) == true); //first cell
        REQUIRE(hashTable2D.checkAvailabilityMinPoint(aabb3) == true); //2nd cell
        REQUIRE(hashTable2D.checkAvailabilityMinPoint(aabb6) == true); //3rd cell
        REQUIRE(hashTable2D.checkAvailabilityMinPoint(aabb7) == true); //4th cell
    }

}



TEST_CASE( "Multithreaded HashTable insertion 2D", "[hashtableInsertion2]" ){
    int number_aabbs = 50;
    int number_threads = 4;

    std::vector<AABB<2>> aabbs(number_aabbs);
    SpatialHashTable<2> hashTable2D(16,1.0/25,4, 25);

    //creation of vertices
    for(int i=0; i<number_aabbs; i++){
        aabbs[i] = AABB<2>(std::vector<unsigned>{(unsigned int)i, (unsigned int)i}, std::vector<unsigned>{(unsigned int)i+2, (unsigned int)i+2});
    }

    //initialize the TBB task scheduler with the number of threads specified
    tbb::task_scheduler_init init(number_threads);

    ParallelWork pw;
    pw.hashTable2d = &hashTable2D;
    pw.aabbs = aabbs;
    std::cout << "Multithreaded operations. This test will take some time..." << std::endl;
    //perform the parallel_for based on ranges managed by the scheduler
    tbb::parallel_for( tbb::blocked_range<int>( 0, number_aabbs ), pw );
}



