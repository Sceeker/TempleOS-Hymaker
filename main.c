#include <time.h>
#include "include/wave.h"
#include "include/sound.h"
#include "include/other.h"

int main() {
    updateSeed(time(NULL));

    makeGod();

    char* song = GodSong(Simple, false, false, 4);

    Song mySong = makeSong(44100, 1);

    mySong = strToSong(song, 44100, 1);

    Wave myWave = songMakeWave(mySong, 8);

    waveToFile( &myWave, "song.wav");
    waveDestroy( &myWave );

    return 0;
}