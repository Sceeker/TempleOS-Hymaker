#include "endianness.h"

int isBigEndian() {
    // test will be stored as [0x00,0x01] on a big endian machine
    // and as [0x01, 0x00] on a little endian machine
    int test = 1;
    char *p = (char*)&test;

    return p[0] == 0;
}

void reverseEndianness( const long long int size, void* value ) {
    int i;
    char result[32]; // never seen built-in types >8 bytes (have room for 32)
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