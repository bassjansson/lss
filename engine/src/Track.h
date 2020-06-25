#ifndef __TRACK_H__
#define __TRACK_H__

#include <iostream>
#include <cmath>
#include <string>
#include <sndfile.h>
// #include <fftw3.h>

#include "Defines.h"

#define VOLUME_LOWPASS 0.1f

using namespace std;

enum TrackState
{
    STOPPED = 0,
    PLAYING
};

class Track
{
public:
    Track(int trackIndex, int inputChannelLeft, int inputChannelRight,
        float x, float y, float r) :
        // inputChannelLeft(inputChannelLeft),
        // inputChannelRight(inputChannelRight),
        trackIndex(trackIndex),
        trackState(STOPPED),
        trackLengthFrames(0),
        trackNumOfChannels(0),
        trackBufferSize(0),
        trackBuffer(NULL),
        volume(0.0f), volumeSet(0.0f),
        xPos(x), yPos(y), radius(r)
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

    void updateVolumeByUserPosition(float x, float y)
    {
        float xDiff = xPos - x;
        float yDiff = yPos - y;

        float distance = (xDiff * xDiff + yDiff * yDiff) / radius;

        if (distance > 1.0f)
            distance = 1.0f;

        volumeSet = cosf(distance * M_PI) * 0.5f + 0.5f;
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

                volume = volume * (1.0f - VOLUME_LOWPASS) + volumeSet * VOLUME_LOWPASS;

                outputBuffer[i * numOutputChannels + LEFT] +=
                  trackBuffer[p * trackNumOfChannels + LEFT] * volume;
                outputBuffer[i * numOutputChannels + RIGHT] +=
                  trackBuffer[p * trackNumOfChannels + RIGHT] * volume;
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

    float volume, volumeSet;
    const float xPos, yPos, radius;
};

#endif // __TRACK_H__
