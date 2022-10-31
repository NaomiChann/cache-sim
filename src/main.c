#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

typedef struct {
	uint32_t tag;
	int val;
} Cache;

uint32_t reverse_bytes( uint32_t bytes );

int main() {
	// variables
	int miss = 0, hit = 0, missComp = 0, missConf = 0, missCap = 0, accesses = 0;
	float missRate = 0, hitRate = 0, missCompRate = 0, missConfRate = 0, missCapRate = 0;
	size_t addressSize = 4; // 4 bytes = 32 bits
	srand( time( NULL ) ); // randomize seed	

	// args (debug)
	int nSets = 512, bSize = 8, assoc = 1, flagOut = 0;
	char subst[2] = "r";
	FILE *inputFile;

	// setting values
	int bitsOffset = ( int ) log2( bSize );
	int bitsIndex = ( int ) log2( nSets );
	int bitsTag = 32 - bitsOffset - bitsIndex;

	// current address info
	uint32_t tag, index, address;

	// creates empty cache
	Cache *cache;
	cache = ( Cache * ) malloc( ( nSets * assoc ) * sizeof( Cache ) );
	
	// helper
	int *dummy, aux = 0, auxS = 0, auxM = nSets * assoc;
	dummy = ( int * ) malloc( ( nSets * assoc ) * sizeof( int ) );

	// sets all allocated arrays' values to 0
	for ( int i = 0; i < ( nSets * assoc ); i++ ) {
		cache[i].val = 0;
		dummy[i] = 0;
	}
	// file path (debug, needs to be changed if running elsewhere)
	fopen_s( &inputFile, "C:/Internet Explorer/The Basement/spaghetti code/cache sim/output/vortex.in.sem.persons.bin", "rb" );

	while ( fread( &address, addressSize, 1, inputFile ) != 0 ) { // loops until eof
		accesses++;
		address = reverse_bytes( address ); // why is this even needed? :/

		// bitwise manipulation shenanigans
		tag = address >> ( bitsOffset + bitsIndex );
		index = address % nSets;
		index = index >> bitsOffset;

		// this has never happened before
		if ( index > ( uint32_t ) nSets ) {
			printf( "wha? why? how? how do i even stop this?" );
		}

		for ( int i = 0; i < assoc; i++ ) { // checks every block
			if ( cache[index + ( nSets * i )].val == 0 ) {
				missComp++;
				auxM--;
				cache[index + ( nSets * i )].val = 1;
				cache[index + ( nSets * i )].tag = tag;
				goto end;
			} else if ( cache[index + ( nSets * i )].tag == tag ) {
				hit++;
				if ( strcmp( subst, "l" ) == 0 ) { // treats lru case
					for ( int j = 0; j < nSets; j++ ) { // increases "use value" for all other slots
						dummy[index + ( nSets * j )] += 1;
					}
					dummy[index + ( nSets * i )] = 0; // sets its use value to 0 since it's new
				}
				goto end;
			}
		}

		// other miss cases substitution behaviors
		if ( auxM == 0 ) {
			missCap++;
		} else {
			missConf++;
		}

		// random
		if ( strcmp( subst, "r" ) == 0 ) {
			cache[index + ( nSets * ( rand() % ( assoc - 1 ) ) )].tag = tag;
		// fifo
		} else if ( strcmp( subst, "f" ) == 0 ) {
			cache[auxS].tag = tag;
			if ( auxS < assoc ) { // increases the helper variable up to the associativity
				auxS++;
			} else { // then loops back to 0
				auxS = 0;
			}
		// lru
		} else if ( strcmp( subst, "l" ) == 0 ) {
			aux = index;
			for ( int i = 1; i < assoc; i++ ) { // checks every single one for their use value and keeps the highest
				if ( dummy[index + ( nSets * ( i ) )] > dummy[index + ( nSets * ( i - 1 ) )] ) {
					aux = index + ( nSets * ( i ) ); // an auxiliar array keeps track of every slot
				}
			}

			for ( int i = 0; i < assoc; i++ ) { // increases the use value of everyone else in this block
				dummy[index + ( nSets * ( i ) )] += 1;
			}
			cache[aux].tag = tag; // sets the previously highest as the new address
			dummy[aux] = 0; // abd its use value to 0
		}

		end:
	}
	
	// miss values updating and calculation
	miss += missComp + missConf + missCap;

	missRate = ( float ) miss / accesses;
	hitRate = ( float ) hit / accesses;

	missCompRate = ( float ) missComp / miss;
	missConfRate = ( float ) missConf / miss;
	missCapRate = ( float ) missCap / miss;

	if ( flagOut == 1 ) {
		printf( "%d %.2f %.2f %.2f %.2f %.2f \n", accesses, hitRate, missRate, missCompRate, missCapRate, missConfRate );
	} else {
		printf( "%d sets, block size: %d, associativity: %d \n", nSets, bSize, assoc );
		printf( "accesses: %d hits: %d misses: %d \nhit rate: %.2f \nmiss rate: %.2f \n", accesses, hit, miss, hitRate, missRate );
		printf( "compulsory: %d rate: %.2f \nconflict: %d rate: %.2f \ncapacity: %d rate %.2f \n", missComp, missCompRate, missConf, missConfRate, missCap, missCapRate );
	}
	
	// properly closes and frees everything
	free( cache );
	free( dummy );
	fclose( inputFile );

	return ( 0 );
}

// extracted shamelesly from https://stackoverflow.com/questions/32786493/reversing-byte-order-in-c
uint32_t reverse_bytes( uint32_t bytes ) { 
    uint32_t aux = 0;
    uint8_t byte;

    for ( int i = 0; i < 32; i += 8 ) {
        byte = ( bytes >> i ) & 0xff;
        aux |= byte << ( 32 - 8 - i );
    }

    return ( aux );
} // shoutouts to 4pie0