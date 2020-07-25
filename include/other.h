#ifndef OTHERDEF
#define OTHERDEF

#include "bible.h"

typedef int bool;
enum { false, true };

typedef struct God {
    char *bits;
    int top;
    int size;
} God;

#endif

extern God god;

void  *mmemset( void *b, int c, int len );

void updateSeed();

int rand();

float random();

signed int randSign();

float divineSquare( int position, float freq, float sampleRate, float randomness );

int ClampI( int i, int inf, int sup );

bool FifoRem( char* b );

bool FifoIns( char b );

void FifoFlush();

int len( const char *str );

char* concatenate( const char* orig, const char* add );

void makeGod();

char *getGodWord();

// CODE THAT FOLLOWS IS DIRECTLY TRANSLATED/EDITED FROM THE TEMPLEOS SOURCE CODE

void GodBitsIns( int num_bits, long long int n );

int GodBits( int num_bits );

// END OF TEMPLEOS CODE EDIT