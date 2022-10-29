#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

typedef struct {
	uint32_t tag;
	int val;
} Cache;

uint32_t reverse_bytes( uint32_t bytes );

int main() {
	// variables
	int miss = 0, hit = 0, missC = 0, accesses = 0;
	float missRate = 0, hitRate = 0;
	size_t addressSize = 4; // 4 bytes = 32 bits

	// args
	int nSets = 256, bSize = 1, assoc = 1, flagOut = 1;
	char *subst = "r";
	FILE *inputFile;

	// setting values
	int bitsOffset = ( int ) log2( bSize );
	int bitsIndex = ( int ) log2( nSets );
	int bitsTag = 32 - bitsOffset - bitsIndex;

	// current address info
	uint32_t tag, index, address, mask = 0xffffffff >> bitsTag;

	// creates empty cache
	Cache *cache;
	cache = ( Cache * ) malloc( ( nSets * assoc ) * sizeof( Cache ) );
	
	// sets all valid bits to 0
	for ( int i = 0; i < ( nSets * assoc ); i++ ) {
		cache[i].val = 0;
	}

	fopen_s( &inputFile, "C:/Internet Explorer/The Basement/spaghetti code/cache sim/output/bin_100.bin", "rb" );

	//fseek( inputFile, 3, SEEK_SET );
	//while ( !feof( inputFile ) ) {
	while ( fread( &address, addressSize, 1, inputFile ) != 0 ) {
		//fread( &address, addressSize, 1, inputFile );
		accesses++;
		address = reverse_bytes( address );
		tag = address >> ( bitsOffset + bitsIndex );
		index = ( address & mask );
		index = index >> bitsOffset;

		if ( cache[index].val == 0 ) {
			missC++;
			cache[index].val = 1;
			cache[index].tag = tag;
		} else if ( cache[index].tag == tag ) {
			hit++;
		} else {
			miss++;
			cache[index].val = 1;
			cache[index].tag = tag;
		}
	}
	
	miss += missC;
	missRate = ( float ) miss / accesses;
	hitRate = ( float ) hit / accesses;

	printf( "%d %d %d %d \n", accesses, hit, miss, missC );
	printf( "%2f, %2f \n", hitRate, missRate );

	free( cache );
	fclose( inputFile );

	return ( 0 );
}

uint32_t reverse_bytes( uint32_t bytes ) {
    uint32_t aux = 0;
    uint8_t byte;
    int i;

    for ( i = 0; i < 32; i += 8 ) {
        byte = ( bytes >> i ) & 0xff;
        aux |= byte << ( 32 - 8 - i );
    }

    return ( aux );
}