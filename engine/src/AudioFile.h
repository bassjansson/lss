#ifndef __AUDIO_FILE_H__
#define __AUDIO_FILE_H__

#include <iostream>
#include <string>
#include <cstring>
#include <sndfile.h>

#include "Defines.h"

using namespace std;

class AudioFile
{
public:
    AudioFile(const char * filePath) :
        readPointer(0), writePointer(1)
    {
        // Open sound file

        memset(&soundFileInfo, 0, sizeof(soundFileInfo));

        if (!(soundFile = sf_open(filePath, SFM_READ, &soundFileInfo)))
        {
            cout << "[AudioFile] Failed to open audio file \"" << filePath << "\" - " << sf_strerror(NULL) << endl;

            soundFileInfo.channels = 1;
        }


        // Allocate circular buffer

        circularBuffer = new float * [AUDIO_NUM_OF_BUFS];

        int bufferSize = AUDIO_BUFFER_SIZE * soundFileInfo.channels;

        for (int b = 0; b < AUDIO_NUM_OF_BUFS; ++b)
        {
            circularBuffer[b] = new float[bufferSize];

            for (int i = 0; i < bufferSize; ++i)
                circularBuffer[b][i] = 0.0f;
        }


        // Pre-load buffers from file

        loadBuffersFromFile();
    }

    ~AudioFile()
    {
        // Close sound file

        sf_close(soundFile);


        // Delete circular buffer

        for (int b = 0; b < AUDIO_NUM_OF_BUFS; ++b)
            delete[] circularBuffer[b];

        delete[] circularBuffer;
    }

    void loadBuffersFromFile()
    {
        while (writePointer != readPointer)
        {
            sf_count_t framesRead = sf_readf_float(soundFile, circularBuffer[writePointer], AUDIO_BUFFER_SIZE);

            if (framesRead != AUDIO_BUFFER_SIZE)
            {
                sf_seek(soundFile, 0, SEEK_SET);
                sf_readf_float(soundFile,
                    circularBuffer[writePointer] + framesRead * soundFileInfo.channels,
                    AUDIO_BUFFER_SIZE - framesRead);
            }

            writePointer = (writePointer + 1) % AUDIO_NUM_OF_BUFS;
        }
    }

    float * getNextBufferToProcess()
    {
        readPointer = (readPointer + 1) % AUDIO_NUM_OF_BUFS;

        return circularBuffer[readPointer];
    }

    int getNumOfChannels()
    {
        return soundFileInfo.channels;
    }

private:
    float ** circularBuffer; // c = circular
    int readPointer, writePointer;

    SNDFILE * soundFile;
    SF_INFO soundFileInfo;
};

#endif // __AUDIO_FILE_H__
