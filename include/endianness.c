#include "endianness.h"

int isBigEndian() {
    int test = 1;
    char *p = (char*)&test;

    return p[0] == 0;
}

void reverseEndianness( const long long int size, void* value ) {
    int i;
    char result[32];
    for( i = 0; i < size; i += 1 ){
        result[i] = ((char*)value)[size - i - 1];
    }
    for( i = 0; i < size; i += 1 ){
        ((char*)value)[i] = result[i];
    }
}

void toBigEndian( const long long int size, void* value ) {
    if( ! isBigEndian() ){
        reverseEndianness(size, value);
    }
}
void toLittleEndian( const long long int size, void* value ) {
    if( isBigEndian() ){
        reverseEndianness(size, value);
    }
}