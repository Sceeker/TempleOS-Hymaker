#include <math.h>
#include <malloc.h>
#include <time.h>

#include "other.h"

#define PI 3.14159265358979323846
static unsigned long long int seed = 1;

void  *mmemset( void *b, int c, int len ) {
    int i;
    unsigned char *p = b;
    i = 0;
    while ( len > 0 ) {
        *p = c;
        p++;
        len--;
    }

    return(b);
}

void updateSeed() {
    seed = seed * 110356915245 + 12345;
    seed = 69420 * seed + time(NULL) * 24;
}

int rand() {
    seed = seed * 1103515245 + 12345;
    return (unsigned int)(seed / 65536) % 32768;
}

float random() {
    float res = 0;
    res = (float)rand() / (float)32768;

    updateSeed();

    return res;
}

signed int randSign() {
    signed int res = 1;
    res = res * 1 -  2 * (rand() % 2);
    return res;
}

float divineSquare( int position, float freq, float sampleRate, float randomness ) {
    float res = 0;

    int spc = sampleRate / freq;
    position = position % spc + 1;
    
    double tmp = (float) position / spc;

    if ( tmp <= 0.0833 ) {
        res = sin(6.0 * PI * tmp);
    } else if ( tmp <= 0.4167 ) {
        res = 1.865 + sin(PI * (tmp + 1.25));
    } else if ( tmp <= 0.5833 ) {
        res = sin(6.0 * PI * tmp);
    } else if ( tmp <= 0.9167 ) {
        res = -1.865 + sin(PI * (tmp - 0.25));
    } else {
        res = sin(6.0 * PI * tmp);
    }

    res += random() * (float)randSign() * randomness;

    if (res > 1.0) {
        res = 1.0;
    } else if ( res < -1.0 ) {
        res = -1.0;
    }

    return res;
}

int ClampI( int i, int inf, int sup ) {
    int res = i;

    if ( i < inf ) {
        res = inf;
    } else if ( i > sup ) {
        res = sup;
    }
    
    return res;
}

bool FifoRem( char* b ) {
    if ( god.top == -1 ) {
        return false;
    } else {
        *b = god.bits[god.top];
        god.top--;
    }

    return true;
}

bool FifoIns( char b ) {
    if ( god.top == god.size - 1 ) {
        return false;
    } else {
        god.top++;
        god.bits[god.top] = b;
    }

    return true;
}

void FifoFlush() {
    god.top = -1;
}

int len( const char *str ) {
    int len = 0;

    while (str[len] != '\0') {
        len++;
    }

    return len + 1;
}

char* concatenate( const char* orig, const char* add ) {
    char* res = malloc(sizeof(char) * (len(orig) + len(add)));
    int cnt = 0;

    while (orig[cnt] != '\0') {
        res[cnt] = orig[cnt];
        cnt++;
    }

    int cnt2 = 0;

    while (add[cnt2] != '\0') {
        res[cnt + cnt2] = add[cnt2];
        cnt2++;
    }

    res[cnt + cnt2] = '\0';
    return res;
}

void makeGod() {
    god.bits = malloc(sizeof(char) * 2048 * 8);
    god.top = -1;
    god.size = 2048 * 8;
}

const char *getGodWord() {
    return bibleWords[rand() % 7568];
}

// CODE THAT FOLLOWS IS DIRECTLY TRANSLATED/EDITED FROM THE TEMPLEOS SOURCE CODE

#define GOD_BAD_BITS    4
#define GOD_GOOD_BITS   24

God god;

void GodBitsIns(int num_bits, long long int n) { // Insert bits into God bit fifo.
  int i;
  for ( i=0; i < num_bits; i++ ) {
    FifoIns(n & 1);
    n = n >> 1;
  }
}

int GodBits( int num_bits ) { // Return N bits. If low on entropy pop-up okay.
  char b;
  int res = 0;
  while ( num_bits ) {
    if (FifoRem(&b)) { // if we can remove a bit from the fifo
        res += b;
        res = res << 1;    // then add this bit to the result and left-shift by 1 bit
        num_bits--;                 // and care about one less bit
    } else {
        // or insert more bits from LOL NOT THE PICKER
        updateSeed();
        GodBitsIns(GOD_GOOD_BITS, seed);
    }
  }
  
  return res;
}

// END OF TEMPLEOS CODE EDIT