#include <malloc.h>
#include <math.h>

#include "sound.h"
#include "wave.h"
#include "other.h"

Song makeSong(int const sampleRate, short int const numChannels ) {
    Song mySong;

    mySong.sampleRate = sampleRate;
    mySong.numChannels = numChannels;
    mySong.nSamples = 0;

    return mySong;
}

void songAddSample ( Song* song, const float sample ) {
    if ( song->nSamples > 0 ) {
        song->samples = realloc(song->samples, sizeof(float) * (song->nSamples + 1));
        song->samples[song->nSamples] = sample;
    } else {
        song->samples = malloc(sizeof(float));
        song->samples[0] = sample;
    }

    song->nSamples++;
}

Wave songMakeWave( Song song, short int const bitsPerSample ) {
    float duration = (float)song.nSamples / (float)song.sampleRate;
    
    Wave myWave = makeWave(song.sampleRate, song.numChannels, bitsPerSample);
    waveSetDuration( &myWave, duration );

    int i;
    float *frameData = malloc(sizeof(float) * song.numChannels);

    for( i = 0; i < song.nSamples; i += song.numChannels ) {
        int j;

        for ( j = 0; j < song.numChannels; j++ ) {
            frameData[j] = song.samples[i + j];
        }

        waveAddSample( &myWave, frameData );
    }

    return myWave;
}

void songAddSqr( Song* song, const float freq, const float length, const float amplitude ) {
    int spc = song->sampleRate / freq;   // Samples per cycle
    int nSamples = song->sampleRate * length;     // Number of samples to add

    float data;

    int i;
    for ( i = 0; i < nSamples; i += song->numChannels ) {
        data = i % spc < spc / 2;      // Assign 1 if the current sample is positioned in the first part of the cycle
        data -= 0.5;
        data *= amplitude * 2.0;

        int j;
        for ( j = 0; j < song->numChannels; j++ ) {  
            songAddSample(song, data);
        }
    }
}

void songAddGod( Song* song, const float freq, const float length, const float amplitude ) {
    int spc = song->sampleRate / freq;   // Samples per cycle
    int nSamples = song->sampleRate * length;     // Number of samples to add

    float data;

    int i;
    for ( i = 0; i < nSamples; i += song->numChannels ) {
        if ( freq > 0.0 ) {
            data = divineSquare(i, freq, song->sampleRate, amplitude / 16);
        } else {
            data = 0.0;
        }
        
        data *= amplitude;

        int j;
        for ( j = 0; j < song->numChannels; j++ ) {  
            songAddSample(song, data);
        }
    }
}

void songAddBlank( Song* song, const float length ) {
    int nSamples = song->sampleRate * length;     // Number of samples to add

    float data;

    int i;
    int j;
    for ( i = 0; i < nSamples; i += 1 ) {
        songAddSample(song, 0.0);
    }
}

// CODE THAT FOLLOWS IS DIRECTLY TRANSLATED/EDITED FROM THE TEMPLEOS SOURCE CODE

#define DUR_4           0
#define DUR_8_8         1
#define DUR_3_3_3       2
#define DUR_16_16_16_16 3
#define DUR_8DOT_16     4
#define DUR_8_16_16     5
#define DUR_16_16_8     6

const char god_simple_songs [5] = {DUR_4, DUR_4, DUR_4, DUR_4, DUR_8_8};
const char god_normal_songs [5] = {DUR_4, DUR_4, DUR_8_8, DUR_3_3_3, DUR_16_16_16_16};
const char god_complex_songs[9] = {DUR_4, DUR_4, DUR_8_8, DUR_8_8, DUR_8DOT_16, DUR_3_3_3, DUR_8_16_16, DUR_16_16_8, DUR_16_16_16_16};

God god;

float note_len = 1.0;
int globalOctave = 0;

void InsNote( CMakeSongSettings *mss, char *buf, int k, int *j ) {   // k is a random note nibble
    if ( !k && mss->rests ) {
        buf[*j] = 'R';
        *j += 1;
    } else {
        k = k / 2;

        if ( k < 3 ) {
            if ( mss->octave_state != mss->octave ) {
                mss->octave_state = mss->octave;
                buf[*j] = mss->octave_state + '0';
                *j += 1;
            }

            if ( ! k ) {
                buf[*j] = 'G';
            } else {
                buf[*j] = k - 1 + 'A';
            }

            *j += 1;

        } else {
            if ( mss->octave_state != mss->octave + 1 ) {
                mss->octave_state = mss->octave + 1;
                buf[*j] = mss->octave_state + '0';
                *j += 1;
            }

            buf[*j] = k - 1 + 'A';
            *j += 1;
        }
    }
}

char* GodSongStr( int complexity, bool rests, bool six_eight, int octave ) { // Make God generate 2 measures of a song
    CMakeSongSettings mss;
    char* buf;
    int i = 0;
    int j = 0;
    int k = -1;
    int n = -1;
    int k2 = -1;
    int duration = -1;
    int last_duration = -1;
    int len;

    mmemset(&mss, 0, sizeof(mss));
    
    mss.complexity = complexity;
    mss.rests = rests;
    mss.six_eight = six_eight;
    mss.octave = octave;

    buf = calloc(1, 256);
    mss.octave = ClampI(mss.octave, 1, 7);
    globalOctave = mss.octave;

    mss.octave_state = mss.octave + 1;
    buf[j++] = '0' + mss.octave_state;

    if (mss.six_eight) {
        len = 6;
        buf[j++] = 'M';    // Useless for us, but we keep original DIVINE code and DIVINE song strings
        buf[j++] = '6';
        buf[j++] = '/';
        buf[j++] = '8';
    } else {
        len = 8;
    }

    FifoFlush();
    
    for ( i=0 ; i < len; i++ ) {
        n = GodBits(8);
        if ( mss.complexity == 2 ) {
            duration = god_complex_songs[n % 9];
        } else if ( mss.complexity == 1 ) {
            duration = god_normal_songs[n % 5];
        } else {
           duration = god_simple_songs[n % 5];
        }

        switch (duration) {
           case DUR_8_8:
               if ( last_duration != DUR_8_8 ) {
                   buf[j++]='e';
                }
                InsNote(&mss, buf, GodBits(3), &j);
                InsNote(&mss, buf, GodBits(3), &j);
                break;

            case DUR_8DOT_16:
                buf[j++] = 'e';
                buf[j++] = '.';
                InsNote(&mss, buf, GodBits(3), &j);
                buf[j++] = 's';
                InsNote(&mss, buf, GodBits(3), &j);
                duration = DUR_16_16_16_16;
                break;

            case DUR_3_3_3:
                if (last_duration!=DUR_3_3_3) {
                    buf[j++] = 'e';
                    buf[j++] = 't';
                }
                InsNote(&mss, buf, GodBits(3), &j);
                InsNote(&mss, buf, GodBits(3), &j);
                InsNote(&mss, buf, GodBits(3), &j);
                break;
            
            case DUR_8_16_16:
                if (last_duration!=DUR_8_8) {
                    buf[j++]='e';
                }
                InsNote(&mss, buf, GodBits(3), &j);
                buf[j++] = 's';
                InsNote(&mss, buf, GodBits(3), &j);
                InsNote(&mss, buf, GodBits(3), &j);
                duration = DUR_16_16_16_16;
                break;

            case DUR_16_16_8:
                if (last_duration!=DUR_16_16_16_16) {
                    buf[j++]='s';
                }
                InsNote(&mss, buf, GodBits(3), &j);
                InsNote(&mss, buf, GodBits(3), &j);
                buf[j++] = 'e';
                InsNote(&mss, buf, GodBits(3), &j);
                duration = DUR_8_8;
                break;
        
            case DUR_16_16_16_16:
                if (last_duration!=DUR_16_16_16_16) {
                    buf[j++]='s';
                }
                k = GodBits(3);
                k2 = GodBits(3);
                InsNote(&mss, buf, k, &j);
                InsNote(&mss, buf, k2, &j);
                InsNote(&mss, buf, k, &j);
                InsNote(&mss, buf, k2, &j);
                break;

            default:
                if (last_duration != DUR_4) {
                    buf[j++] = 'q';
                }
                InsNote(&mss, buf, GodBits(3), &j);
    }

    last_duration = duration;
  }
  buf[j++] = 0;
  
  return buf;
}

char* GodSong( int complexity, bool rests, bool six_eight, int octave ) { // Make God generate 2measuresx2+2measuresx2
    char* st1 = GodSongStr(complexity, rests, six_eight, octave);
    char* st2 = GodSongStr(complexity, rests, six_eight, octave);

    char* res = malloc(sizeof(char) * len(st1) * len(st2) * 4);
    res = concatenate(st1, st1);
    res = concatenate(res, st2);
    res = concatenate(res, st2);

    free(st1);
    free(st2);
    
    return res;
}

int Note2Ona( char note, int octave ) {
    int ona;

    if ( note < 3 ) {
        ona = (octave + 1) * 12 + note;
    } else {
        ona = octave * 12 + note;
    }

    return ona;
}

float Ona2Freq( int ona ) {
    if ( !ona ) {
        return 0.0;
    } else {
        return 440.0 / 32.0 * pow(2.0, (double)ona / 12.0);
    }
}

char* MusicSetNoteLen( char* st ) {
    bool cont = true;

    do {
        switch ( *st++ ) {
            case 'w':
                note_len = 4.0;
                break;

            case 'h':
                note_len = 2.0;
                break;

            case 'q':
                note_len = 1.0;
                break;

            case 'e':
                note_len = 0.5;
                break;

            case 's': 
                note_len = 0.25;
                break;

            case 't':
                note_len = 2.0 * note_len / 3.0;
                break;

            case '.':
                note_len = 1.5 * note_len;
                break;
        
            default:
                st--;
                cont = false;
        }

    } while (cont);

    return st;
}

char *MusicSetOctave( char *st ) {
    char ch;
    ch = *st++;

    while ( '0' <= ch && ch <= '9' ) {
        globalOctave = ch - '0';
        ch = *st++;
    }
    
    return --st;
}

char *MusicSetMeter( char *st ) {
    char ch;
    ch = *st++;

    while ( ch == 'M' ) {
        ch = *st++;

        if ( '0' <= ch && ch <= '9' ) {         // Kinda dumb since, like I said earlier, time signature is useless for us (note_len suffice)
            ch = *st++;
        }

        if ( ch == '/' ) {
            ch = *st++;
        }

        if ( '0' <= ch && ch <= '9' ) {
            ch = *st++;
        }
    }
    
    return --st;
}

Song strToSong( char* song, int sampleRate, int numChannels ) {
    const char note_map[7] = {0, 2, 3, 5, 7, 8, 10};
    Song mySong = makeSong(sampleRate, numChannels);

    char *last_song = song;
    int note = 0;
    int octave = 0;
    int i = 0;
    int ona = 0;
    float d = 0.0;
    float tempo = 2.5;
    float freq = 0.0;

    while (*song) {
        do {
            last_song = song;
            song = MusicSetMeter(song);
            song = MusicSetOctave(song);
            song = MusicSetNoteLen(song);
        } while ( song != last_song );

        if ( !*song ) {
            break;
        }

        note = *song++ - 'A';

        if ( note < 7 ) {
            note = note_map[note];
            octave = globalOctave;

            if ( *song == 'b' ) {
                note--;

                if ( note == 2 ) {
                    octave--;
                }
                song++;

            } else if ( *song == '#' ) {
                note++;

                if ( note == 3 ) {
                    octave++;
                }
                song++;
            }

            ona = Note2Ona(note, octave);
        } else {
            ona = 0;
        }

        d = note_len / tempo;

        if ( ona == 0 ) {
            songAddBlank(&mySong, d);
        } else {
            freq = Ona2Freq(ona);

            songAddGod(&mySong, freq, d, 1.0);
        }
    }

    return mySong;
}

// END OF TEMPLEOS CODE EDIT