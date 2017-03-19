//
// Created by Joel Fuentes on 3/18/17.
//

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hh"
#include "../src/SpatialHashTable.h"


unsigned int Factorial( unsigned int number ) {
    return number <= 1 ? number : Factorial(number-1)*number;
}


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