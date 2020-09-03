#ifndef __TRACK_H__
#define __TRACK_H__

#include <iostream>
#include <cmath>
// #include <fftw3.h>

#include "Defines.h"
#include "AudioFile.h"

#define VOLUME_LOWPASS   0.1f
#define VOLUME_THRESHOLD 0.0001f // -80dB

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
      float x, float y, float r, const char * audioFilePath) :
        // inputChannelLeft(inputChannelLeft),
        // inputChannelRight(inputChannelRight),
        trackIndex(trackIndex),
        trackState(STOPPED),
        volume(0.0f), volumeSet(0.0f),
        xPos(x), yPos(y), radius(r),
        audioFile(audioFilePath)
    { }

    ~Track()
    { }

    int getTrackIndex()
    {
        return trackIndex;
    }

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
        float dx = x - xPos;
        float dy = y - yPos;

        float c = sqrtf(dx * dx + dy * dy) / radius;

        // Gaussian distribution
        volumeSet = expf(-0.69f * c * c);

        // Cosine distribution
        // if (c > 1.0f) c = 1.0f;
        // volumeSet = cosf(c * M_PI) * 0.5f + 0.5f;
    }

    void loadAudioFromFile()
    {
        audioFile.loadBuffersFromFile();
    }

    void process(
        const float * inputBuffer,
        float *       outputBuffer,
        frame_t       framesPerBuffer,
        int           numInputChannels,
        int           numOutputChannels,
        frame_t       currentFrame)
    {
        if (trackState == STOPPED)
            return;

        if (volume >= VOLUME_THRESHOLD)
        {
            float * trackBuffer    = audioFile.getNextBufferToProcess();
            int trackNumOfChannels = audioFile.getNumOfChannels();

            int outRight   = numOutputChannels > 1;
            int trackRight = trackNumOfChannels > 1;

            for (frame_t i = 0; i < framesPerBuffer; ++i)
            {
                volume = volume * (1.0f - VOLUME_LOWPASS) + volumeSet * VOLUME_LOWPASS;

                // Left channel
                outputBuffer[i * numOutputChannels] +=
                  trackBuffer[i * trackNumOfChannels] * volume;

                // Right channel
                outputBuffer[i * numOutputChannels + outRight] +=
                  trackBuffer[i * trackNumOfChannels + trackRight] * volume;
            }
        }
        else
        {
            if (volumeSet >= VOLUME_THRESHOLD)
                volume = VOLUME_THRESHOLD;
        }
    }

private:
    // const int inputChannelLeft;
    // const int inputChannelRight;

    const int trackIndex;
    TrackState trackState;

    float volume, volumeSet;
    const float xPos, yPos, radius;

    AudioFile audioFile;
};

#endif // __TRACK_H__
