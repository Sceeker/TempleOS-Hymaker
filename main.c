#include "include/wave.h"
#include "include/sound.h"
#include "include/other.h"

int main() {
    updateSeed();     // Update seed for randomness. We don't use "God picker" here (we want this to run without a person)

    makeGod();      // Initialize fifo (used for the God bits (random generator))

    char* song = GodSong(Complex, false, false, 4);      // Create an hymn string: complex, without rests, not in 6/8 time signature, with octave 4

    Song mySong = makeSong(44100, 1);       // Create a song for the hymn at 44100Hz in mono
                                            // Stereo and more is broken atm...

    mySong = strToSong(song, 44100, 1);     // Convert the hymn string to a song

    Wave myWave = songMakeWave(mySong, 8);      // Create a wave corresponding to our song

    waveToFile( &myWave, "hymn.wav");       // Save our hymn's wave
    waveDestroy( &myWave );

    return 0;
}