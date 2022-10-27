#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
	int validBit, tag;
} Cache;

int main( int argc, char *argv[] ) {
	/*if ( argc != 7 ) {
		printf( "Numero de argumentos incorreto. Utilize: \n" );
		printf( "./cache_simulator <nsets> <bsize> <assoc> <substituicao> <flag_saida> arquivo_de_entrada \n" );
		exit( EXIT_FAILURE );
	}

	int nsets = atoi( argv[1] ), bsize = atoi( argv[2] ), assoc = atoi( argv[3] );
	char *subst = argv[4];
	int flagOut = atoi( argv[5] );
	FILE *inputFile = argv[6];
	*/
	int nsets = 256, bsize = 4, assoc = 1;
	char *subst = "r";
	int flagOut = 1;
	FILE *inputFile = fopen( "C:/Internet Explorer/The Basement/spaghetti code/cache sim/output/bin_100.bin", "rb" );

	int address, tag, index;
	int offsetBits, indexBits, tagBits, accesses = 0;
	float miss = 0, hit = 0, compulsoryMiss = 0;
	float hitRate, missRate, compulsoryMissRate, capacityMissRate, conflictMissRate;
	Cache *cache;

	int buffer[1];

	printf( "nsets = %d \n", nsets );
	printf( "bsize = %d \n", bsize );
	printf( "assoc = %d \n", assoc );
	printf( "subst = %s \n", subst );
	printf( "flagOut = %d \n", flagOut );
	printf( "arquivo = %s \n", inputFile );

	cache = ( Cache * ) malloc( ( nsets * assoc ) * sizeof( Cache ) );

	for ( int i = 0; i < ( nsets * assoc ); i++ ) {
		cache[i].validBit = 0;
	}
	//criar uma estrutura de dados para armazenar os tags e os bits de validade.
	offsetBits = log2( bsize );
	indexBits = log2( nsets );
	tagBits = 32 - offsetBits - indexBits;
	//descobre o número de bits de cada parcela do address
	//para todos os addresss do arquivo
	int i = 0;
	while ( fgetc( inputFile ) != EOF ) {
		accesses++;
		address = fread( buffer, sizeof( buffer ), 1, inputFile );
		tag = address >> ( offsetBits + indexBits );
		index = ( address >> offsetBits );
		index = index >> ( 2^( indexBits - 1 ) );
		//isso é uma máscara que vai deixar apenas os bits do índice na variável “address”.
		// para o mapeamento direto
		if ( cache[index].validBit == 0 ) {
			compulsoryMiss++;
			miss++;
			cache[index].validBit = 1;
			cache[index].tag = tag;
			// estas duas últimas instruções representam o tratamento da falta.
		} else if ( cache[index].tag == tag ) {
			hit++;
		} else {
			miss++;
			//conflito ou capacidade?
			cache[index].validBit = 1;
			cache[index].tag = tag;
		}
		i++;
	}

	missRate = ( miss / accesses );
	hitRate = ( hit / accesses );

	if ( flagOut == 1 ) {
		printf( "%d, %f, %f, f, f, f \n", accesses, hitRate, missRate );
	}

	return ( 0 );
}