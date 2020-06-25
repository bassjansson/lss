#ifndef __TRACK_H__
#define __TRACK_H__

#include <iostream>
#include <cmath>
#include <string>
#include <sndfile.h>
// #include <fftw3.h>

#include "Defines.h"

using namespace std;

enum TrackState
{
    STOPPED = 0,
    PLAYING
};

class Track
{
public:
    Track(int trackIndex, int inputChannelLeft, int inputChannelRight) :
        // inputChannelLeft(inputChannelLeft),
        // inputChannelRight(inputChannelRight),
        trackIndex(trackIndex),
        trackState(STOPPED),
        trackLengthFrames(0),
        trackNumOfChannels(0),
        trackBufferSize(0),
        trackBuffer(NULL)
    { }

    ~Track()
    {
        delete[] trackBuffer;
    }

    int getTrackIndex()
    {
        return trackIndex;
    }

    bool loadAudioFile(const char * filePath)
    {
        trackState = STOPPED;

        trackLengthFrames  = 0;
        trackNumOfChannels = 0;
        trackBufferSize    = 0;

        if (trackBuffer)
            delete[] trackBuffer;


        SNDFILE * audioFile;
        SF_INFO audioFileInfo;

        memset(&audioFileInfo, 0, sizeof(audioFileInfo));

        if (!(audioFile = sf_open(filePath, SFM_READ, &audioFileInfo)))
        {
            cout << "[Track] Failed to open audio file: " << sf_strerror(NULL) << endl;
            return false;
        }

        trackBufferSize = audioFileInfo.frames * audioFileInfo.channels;
        trackBuffer     = new float[trackBufferSize];

        frame_t blockBufferSize = AUDIO_BUFFER_SIZE * audioFileInfo.channels;
        float * blockBuffer     = trackBuffer;

        while (blockBuffer + blockBufferSize <= trackBuffer + trackBufferSize)
            blockBuffer += sf_read_float(audioFile, blockBuffer, blockBufferSize);

        sf_read_float(audioFile, blockBuffer, trackBuffer - blockBuffer);

        sf_close(audioFile);


        trackLengthFrames  = audioFileInfo.frames;
        trackNumOfChannels = audioFileInfo.channels;

        trackState = PLAYING;

        return true;
    } // loadAudioFile

    void startPlayback()
    {
        trackState = PLAYING;
    }

    void stopPlayback()
    {
        trackState = STOPPED;
    }

    void setFrequency(float freq)
    {
        // TODO
    }

    void process(
        const float * inputBuffer,
        float *       outputBuffer,
        frame_t       framesPerBuffer,
        int           numInputChannels,
        int           numOutputChannels,
        frame_t       currentFrame)
    {
        if (trackState == PLAYING && trackLengthFrames > 0)
        {
            frame_t p;

            for (frame_t i = 0; i < framesPerBuffer; ++i)
            {
                p = (currentFrame + i) % trackLengthFrames;

                outputBuffer[i * numOutputChannels + LEFT] +=
                  trackBuffer[p * trackNumOfChannels + LEFT];
                outputBuffer[i * numOutputChannels + RIGHT] +=
                  trackBuffer[p * trackNumOfChannels + RIGHT];
            }
        }
        else
        {
            for (frame_t i = 0; i < framesPerBuffer; ++i)
            {
                outputBuffer[i * numOutputChannels + LEFT]  = 0.0f;
                outputBuffer[i * numOutputChannels + RIGHT] = 0.0f;
            }
        }
    }

private:
    // const int inputChannelLeft;
    // const int inputChannelRight;

    const int trackIndex;
    TrackState trackState;

    frame_t trackLengthFrames;
    int trackNumOfChannels;

    frame_t trackBufferSize;
    float * trackBuffer;
};

#endif // __TRACK_H__
