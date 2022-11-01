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

uint32_t ReverseBytes( uint32_t bytes );
void HowRU_LRU( int *dummy, int assoc, int nSets, uint32_t index,  uint32_t indexC );

int main( int argc, char *argv[ ] ) {
	if ( argc != 7 ){
		printf( "Numero de argumentos incorreto. Utilize: \n" );
		printf( "./cache_simulator <nsets> <bsize> <assoc> <substituição> <flag_saida> arquivo_de_entrada\n" );
		exit( EXIT_FAILURE );
	}

	// variables
	int miss = 0, hit = 0, missComp = 0, missConf = 0, missCap = 0, accesses = 0;
	float missRate = 0, hitRate = 0, missCompRate = 0, missConfRate = 0, missCapRate = 0;
	FILE *inputFile;
	size_t addressSize = 4; // 32 bit
	srand( time( NULL ) ); // randomize seed	

	/*
	// args (debug)
	int nSets = 128, bSize = 2, assoc = 4, flagOut = 0;
	char replace[2] = "r";

	// file path (debug, needs to be changed if running elsewhere)
	fopen_s( &inputFile, "C:/Internet Explorer/The Basement/spaghetti code/cache sim/output/bin_1000.bin", "rb" );
	*/
	
	// args
	int nSets = atoi( argv[1] ), bSize = atoi( argv[2] ), assoc = atoi( argv[3] ), flagOut = atoi( argv[5] );
	char *replace = argv[4];
	char *inputFilePath = argv[6];
	fopen_s( &inputFile, inputFilePath, "rb" );

	// setting values
	int bitsOffset = ( int ) log2( bSize );
	int bitsIndex = ( int ) log2( nSets );
	int bitsTag = 32 - bitsOffset - bitsIndex;

	// current address info
	uint32_t tag, index, address, mask = 0xffffffff >> bitsTag;

	// helper
	int *dummy, aux = 0, auxR = assoc;
	dummy = ( int * ) malloc( ( nSets * assoc ) * sizeof( int ) );

	// creates empty cache
	Cache *cache;
	cache = ( Cache * ) malloc( ( nSets * assoc ) * sizeof( Cache ) );
	
	// sets all allocated arrays' values to 0
	for ( int i = 0; i < ( nSets * assoc ); i++ ) {
		cache[i].val = 0;
		dummy[i] = 0;
	}

	// file reading loop
	while ( fread( &address, addressSize, 1, inputFile ) != 0 ) {
		accesses++;
		address = ReverseBytes( address ); // why is this even needed? :/

		// bitwise manipulation shenanigans
		tag = address >> ( bitsOffset + bitsIndex );
		index = ( address & mask ) >> bitsOffset;

		for ( int i = 0; i < assoc; i++ ) { // checks every block
			if ( cache[index + ( nSets * i )].val == 0 ) { // cache[index][i]
				if ( i == 0 ) {
					missComp++;
				} else {
					missConf++;
				}

				cache[index + ( nSets * i )].val = 1;
				cache[index + ( nSets * i )].tag = tag;
				if ( strcmp( replace, "l" ) == 0 ) { // treats lru case
					HowRU_LRU( dummy, assoc, nSets, index, ( index + ( nSets * i ) ) );
				}
				goto end;
			} else if ( cache[index + ( nSets * i )].tag == tag ) {
				hit++;
				if ( strcmp( replace, "l" ) == 0 ) {
					HowRU_LRU( dummy, assoc, nSets, index, ( index + ( nSets * i ) ) );
				}
				goto end;
			}
		}

		missCap++;

		// skips replacement policy for direct mapping
		if ( assoc == 1 ) { 
			cache[index].tag = tag;
			goto end;
		}

		// random
		if ( strcmp( replace, "r" ) == 0 ) {
			cache[index + ( nSets * ( rand() % ( assoc - 1 ) ) )].tag = tag;

		// fifo
		} else if ( strcmp( replace, "f" ) == 0 ) {
			if ( auxR < assoc ) { // increases the helper variable up to the associativity
				auxR++;
			} else { // then loops back to 0
				auxR = 0;
			}
			cache[index + ( nSets * auxR )].tag = tag;
			
		// lru
		} else if ( strcmp( replace, "l" ) == 0 ) {
			aux = index;
			for ( int i = 1; i < assoc; i++ ) { // checks every single one for their use value and keeps the highest
				if ( dummy[index + ( nSets * ( i ) )] > dummy[index + ( nSets * ( i - 1 ) )] ) {
					aux = index + ( nSets * ( i ) ); // an auxiliar array keeps track of every slot
				}
			}

			cache[aux].tag = tag; // sets the previously highest as the new address
			HowRU_LRU( dummy, assoc, nSets, index, aux );
		}

		end:
	}
	
	// miss values updating and calculation
	miss = missComp + missConf + missCap;

	missRate = ( float ) miss / accesses;
	hitRate = ( float ) hit / accesses;

	missCompRate = ( float ) missComp / miss;
	missConfRate = ( float ) missConf / miss;
	missCapRate = ( float ) missCap / miss;

	if ( flagOut == 1 ) {
		printf( "%d %.2f %.2f %.2f %.2f %.2f \n", accesses, hitRate, missRate, missCompRate, missCapRate, missConfRate );
	} else {
		printf( "============================================== \n" );
		printf( "%d sets, block size: %d, associativity: %d, ", nSets, bSize, assoc );
		if ( strcmp( replace, "r" ) == 0 ) {
			printf( "random \n" );
		} else if ( strcmp( replace, "f" ) == 0 ) {
			printf( "FIFO \n" );
		} else if ( strcmp( replace, "l" ) == 0 ) {
			printf( "LRU \n" );
		}
		printf( "---------------------------------------------- \n" );
		printf( "accesses   | %d \nhits       | %d ( %.2f%% ) \n", accesses, hit, hitRate * 100 );
		printf( "misses     | %d ( %.2f%% ) \n", miss, missRate * 100 );
		printf( "---------------------------------------------- \n" );
		printf( "compulsory | %d ( %.2f%% ) \ncapacity   | %d ( %.2f%% ) \nconflict   | %d ( %.2f%% ) \n", missComp, missCompRate * 100, missCap, missCapRate * 100, missConf, missConfRate * 100 );
		printf( "============================================== \n" );
	}
	
	free( cache );
	free( dummy );
	// fclose( inputFile );

	return ( 0 );
}

/*
=====
reverses bytes order cause they're backwards for whatever reason
-----
extracted shamelesly from https://stackoverflow.com/questions/32786493/reversing-byte-order-in-c
=====
*/
uint32_t ReverseBytes( uint32_t bytes ) { 
    uint32_t aux = 0;
    uint8_t byte;

    for ( int i = 0; i < 32; i += 8 ) {
        byte = ( bytes >> i ) & 0xff;
        aux |= byte << ( 32 - 8 - i );
    }

    return ( aux );
} // shoutouts to 4pie0

/*
=====
whole block use value inceased by 1
most recently used set to 0
=====
*/
void HowRU_LRU( int *dummy, int assoc, int nSets, uint32_t index,  uint32_t indexC ) {
	for ( int i = 0; i < assoc; i++ ) {
		dummy[index + ( nSets * i )] += 1;
	}
	dummy[indexC] = 0;
}