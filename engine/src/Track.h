#ifndef __TRACK_H__
#define __TRACK_H__

#include <iostream>
#include <cmath>
// #include <fftw3.h>

#include "Defines.h"
#include "AudioFile.h"

using namespace std;

enum TrackState
{
    STOPPED = 0,
    PLAYING
};

struct TrackData
{
    int   index;
    float x, y, w, h, r;
    float volumeLowpass;
    float volumeThreshold;
};

class Track
{
public:
    Track(TrackData& trackData, const char * audioFilePath) :
        trackData(trackData),
        trackState(STOPPED),
        volume(0.0f),
        volumeSet(0.0f),
        audioFile(audioFilePath)
    { }

    ~Track()
    { }

    int getTrackIndex()
    {
        return trackData.index;
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
        float dx = (trackData.x - x) / trackData.w * 2.0f + 1.0f;
        float dy = (trackData.y - y) / trackData.h * 2.0f + 1.0f;

        float c = sqrtf(dx * dx + dy * dy) / trackData.r;

        // Gaussian distribution
        volumeSet = expf(-0.69f * c * c); // Need the square?

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

        if (volume >= trackData.volumeThreshold)
        {
            float * trackBuffer    = audioFile.getNextBufferToProcess();
            int trackNumOfChannels = audioFile.getNumOfChannels();

            int outRight   = numOutputChannels > 1;
            int trackRight = trackNumOfChannels > 1;

            float v1 = trackData.volumeLowpass;
            float v2 = 1.0f - v2;

            for (frame_t i = 0; i < framesPerBuffer; ++i)
            {
                volume = volume * v1 + volumeSet * v2;

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
            if (volumeSet >= trackData.volumeThreshold)
                volume = trackData.volumeThreshold;
        }
    } // process

private:
    TrackData trackData;
    TrackState trackState;

    float volume;
    float volumeSet;

    AudioFile audioFile;
};

#endif // __TRACK_H__
