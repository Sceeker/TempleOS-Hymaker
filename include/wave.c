#include <stdio.h>
#include <math.h>
#include <malloc.h>

#include "endianness.h"
#include "wave.h"
#include "other.h"

WaveHeader makeWaveHeader( int const sampleRate, short int const numChannels, short int bitsPerSample ) {
    WaveHeader myHeader;

    // RIFF WAVE Header
    myHeader.chunkId[0] = 'R';
    myHeader.chunkId[1] = 'I';
    myHeader.chunkId[2] = 'F';
    myHeader.chunkId[3] = 'F';
    myHeader.format[0] = 'W';
    myHeader.format[1] = 'A';
    myHeader.format[2] = 'V';
    myHeader.format[3] = 'E';

    // Format subchunk
    myHeader.subChunk1Id[0] = 'f';
    myHeader.subChunk1Id[1] = 'm';
    myHeader.subChunk1Id[2] = 't';
    myHeader.subChunk1Id[3] = ' ';
    myHeader.audioFormat = 1; // FOR PCM
    myHeader.numChannels = numChannels; // 1 for MONO, 2 for stereo
    myHeader.sampleRate = sampleRate; // ie 44100 hertz, cd quality audio
    myHeader.bitsPerSample = bitsPerSample;

    if( bitsPerSample < 8 && bitsPerSample > 1 ) {
        myHeader.byteRate = myHeader.sampleRate * myHeader.numChannels;
        myHeader.blockAlign = myHeader.numChannels;
    } else {
        myHeader.byteRate = myHeader.sampleRate * myHeader.numChannels * myHeader.bitsPerSample / 8;
        myHeader.blockAlign = myHeader.numChannels * myHeader.bitsPerSample / 8;
    }

    // Data subchunk
    myHeader.subChunk2Id[0] = 'd';
    myHeader.subChunk2Id[1] = 'a';
    myHeader.subChunk2Id[2] = 't';
    myHeader.subChunk2Id[3] = 'a';

    // All sizes for later:
    // chuckSize = 4 + (8 + subChunk1Size) + (8 + subChubk2Size)
    // subChunk1Size is constanst, i'm using 16 and staying with PCM
    // subChunk2Size = nSamples * nChannels * bitsPerSample/8
    // Whenever a sample is added:
    //    chunkSize += (nChannels * bitsPerSample/8)
    //    subChunk2Size += (nChannels * bitsPerSample/8)
    myHeader.chunkSize = 4 + 8 + 16 + 8 + 0;
    myHeader.subChunk1Size = 16;
    myHeader.subChunk2Size = 0;
    
    return myHeader;
}

Wave makeWave(int const sampleRate, short int const numChannels, short int const bitsPerSample) {
    Wave myWave;
    myWave.header = makeWaveHeader(sampleRate, numChannels, bitsPerSample);

    return myWave;
}

void waveDestroy( Wave* wave ) {
    free( wave->data );
}

void waveSetDuration( Wave* wave, const float seconds ) {
    long long int totalBytes = (long long int)(wave->header.byteRate*seconds);
    wave->data = (char*)malloc(totalBytes);
    wave->index = 0;
    wave->size = totalBytes;
    wave->nSamples = (long long int) wave->header.numChannels * wave->header.sampleRate * seconds;
    wave->header.chunkSize = 4 + 8 + 16 + 8 + totalBytes;
    wave->header.subChunk2Size = totalBytes;
}

void waveAddSample( Wave* wave, const float* samples ) {
    int i;
    short int sample8bit;
    int sample16bit;
    long int sample32bit;
    char* sample;

    if( wave->header.bitsPerSample < 8 && wave->header.bitsPerSample > 1 ) {
        for( i=0; i < wave->header.numChannels; i++ ){
            sample8bit = (short int) (128 + 128.0 * samples[i]);
            sample8bit = round(wave->header.bitsPerSample * (float)sample8bit / 128.0);
            sample8bit = sample8bit * (128 / wave->header.bitsPerSample);
            sample8bit = ClampI(sample8bit, 0, 254);

            toLittleEndian(1, (void*) &sample8bit);
            sample = (char*)&sample8bit;
            (wave->data)[ wave->index ] = sample[0];
            wave->index += 1;
        }
    }

    if( wave->header.bitsPerSample == 8 ) {
        for( i=0; i < wave->header.numChannels; i++ ){
            sample8bit = (short int) (127 + 127.0 * samples[i]);
            toLittleEndian(1, (void*) &sample8bit);
            sample = (char*)&sample8bit;
            (wave->data)[ wave->index ] = sample[0];
            wave->index += 1;
        }
    }

    if( wave->header.bitsPerSample == 16 ) {
        for( i=0; i < wave->header.numChannels; i++ ){
            sample16bit = (int) (32767 * samples[i]);
            //sample = (char*)&litEndianInt( sample16bit );
            toLittleEndian(2, (void*) &sample16bit);
            sample = (char*)&sample16bit;
            wave->data[ wave->index + 0 ] = sample[0];
            wave->data[ wave->index + 1 ] = sample[1];
            wave->index += 2;
        }
    }

    if( wave->header.bitsPerSample == 32 ){
        for( i=0; i < wave->header.numChannels; i+= 1){
            sample32bit = (long int) ((pow(2,32-1)-1)*samples[i]);
            //sample = (char*)&litEndianLong( sample32bit );
            toLittleEndian(4, (void*) &sample32bit);
            sample = (char*)&sample32bit;
            wave->data[ wave->index + 0 ] = sample[0];
            wave->data[ wave->index + 1 ] = sample[1];
            wave->data[ wave->index + 2 ] = sample[2];
            wave->data[ wave->index + 3 ] = sample[3];
            wave->index += 4;
        }
    }
}

void waveToFile( Wave* wave, const char* filename ) {

    bool inf = false;
    short int last = 0;

    if( wave->header.bitsPerSample < 8 && wave->header.bitsPerSample > 1 ) {
        last = wave->header.bitsPerSample;
        wave->header.bitsPerSample = 8;
        inf = true;
    }

    // First make sure all numbers are little endian
    toLittleEndian(sizeof(int), (void*)&(wave->header.chunkSize));
    toLittleEndian(sizeof(int), (void*)&(wave->header.subChunk1Size));
    toLittleEndian(sizeof(short int), (void*)&(wave->header.audioFormat));
    toLittleEndian(sizeof(short int), (void*)&(wave->header.numChannels));
    toLittleEndian(sizeof(int), (void*)&(wave->header.sampleRate));
    toLittleEndian(sizeof(int), (void*)&(wave->header.byteRate));
    toLittleEndian(sizeof(short int), (void*)&(wave->header.blockAlign));
    toLittleEndian(sizeof(short int), (void*)&(wave->header.bitsPerSample));
    toLittleEndian(sizeof(int), (void*)&(wave->header.subChunk2Size));

    // Open the file, write header, write data
    FILE *file;
    file = fopen(filename, "wb");
    fwrite( &(wave->header), sizeof(WaveHeader), 1, file );
    fwrite( (void*)(wave->data), sizeof(char), wave->size, file );
    fclose( file );

    // Convert back to system endian-ness
    toLittleEndian(sizeof(int), (void*)&(wave->header.chunkSize));
    toLittleEndian(sizeof(int), (void*)&(wave->header.subChunk1Size));
    toLittleEndian(sizeof(short int), (void*)&(wave->header.audioFormat));
    toLittleEndian(sizeof(short int), (void*)&(wave->header.numChannels));
    toLittleEndian(sizeof(int), (void*)&(wave->header.sampleRate));
    toLittleEndian(sizeof(int), (void*)&(wave->header.byteRate));
    toLittleEndian(sizeof(short int), (void*)&(wave->header.blockAlign));
    toLittleEndian(sizeof(short int), (void*)&(wave->header.bitsPerSample));
    toLittleEndian(sizeof(int), (void*)&(wave->header.subChunk2Size));

    if ( inf ) {
        wave->header.bitsPerSample = last;
    }
}