#include <stdio.h>

#include "include/wave.h"
#include "include/sound.h"
#include "include/other.h"

int main() {
    makeGod();      // Initialize fifo (used for the God bits (random generator))

    updateSeed();     // Update seed for randomness. We don't use "God picker" here (we want this to run without a person)

    int complexity;
    bool rests;
    bool six_eight;
    int octave;

    char input[64];
    mmemset(&input, 0, 64);

    printf("Song complexity [Simple/Normal/Complex]: ");
    gets( input );

    switch ( *input ) {
        case 'S':
            complexity = Simple;
            break;

        case 'C':
            complexity = Complex;
            break;

        default:
            complexity = Normal;
    }

    printf("\nRests [Yes/No]: ");
    gets( input );

    if ( *input == 'Y' ) {
        rests = true;
    } else {
        rests = false;
    }

    printf("\n6/8 time signature [Yes/No]: ");
    gets( input );

    if ( *input == 'Y' ) {
        six_eight = true;
    } else {
        six_eight = false;
    }

    printf("\nOctave [1-7]: ");
    gets( input );

    if ( *input ) {
        octave = ClampI(*input - '0', 1, 7);
    } else {
        octave = 4;
    }

    char* song = GodSong(complexity, rests, six_eight, octave);
    printf("\nHere is your hymn's string:\n%s\n", song);

    Song mySong = makeSong(32000, 1);       // Create a song for the hymn at 32000Hz in mono
    mySong = strToSong(song, 32000, 1);     // Convert the hymn string to a song

    Wave myWave = songMakeWave(mySong, 8);  // Create a wave corresponding to our song

    char *name = concatenate(getGodWord(), ".wav");
    printf("\nIt has been saved as %s", name);
    
    waveToFile( &myWave, name);       // Save our hymn's wave
    waveDestroy( &myWave );

    printf("\nPress any key to exit.");
    gets( input );

    return 0;
}