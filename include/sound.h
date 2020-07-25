#include "wave.h"
#include "other.h"

typedef struct Song {
    short int numChannels;
    int sampleRate;
    long long int nSamples;
    float* samples;
} Song;

Song makeSong(int const sampleRate, short int const numChannels );

void songAddSample ( Song* song, const float sample );

Wave songMakeWave( Song song, short int const bitsPerSample );

void songAddSqr( Song* song, const float freq, const float length, const float amplitude );

void songAddGod( Song* song, const float freq, const float length, const float amplitude );

// CODE THAT FOLLOWS IS DIRECTLY TRANSLATED/EDITED FROM THE TEMPLEOS SOURCE CODE

enum { Simple, Normal, Complex };

typedef struct CMakeSongSettings {
    int complexity;
    bool rests;
    bool six_eight;
    int octave;
    int octave_state;
} CMakeSongSettings;

void InsNote( CMakeSongSettings* mss, char* buf, int k, int* j );

char* GodSongStr( int complexity, bool rests, bool six_eight, int octave );

char* GodSong( int complexity, bool rests, bool six_eight, int octave );

int Note2Ona( char note, int octave );

float Ona2Freq( int ona );

char* MusicSetNoteLen( char* st );

char *MusicSetOctave( char *st );

char *MusicSetMeter( char *st );

Song strToSong( char* song, int sampleRate, int numChannels );

// END OF EDITED CODE