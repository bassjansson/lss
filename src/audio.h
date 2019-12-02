#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <iostream>
#include <math.h>
#include "portaudio.h"

using namespace std;

#define SAMPLE_RATE 48000
#define FRAMES_PER_BUFFER 1024
#define CHANNELS_PER_FRAME 2

// TODO: Pa_GetErrorText(err)

class Audio
{
public:
    Audio()
    {
        paInitError = Pa_Initialize();

        if (paInitError != paNoError)
            cout << "Failed to initialize PortAudio." << endl;
    }

    ~Audio()
    {
        if (paInitError == paNoError)
            Pa_Terminate();
    }

    bool open(PaDeviceIndex deviceIndex)
    {
        if (paInitError != paNoError)
        {
            cout << "Cannot open audio stream if PortAudio failed to initialize." << endl;
            return false;
        }

        const PaDeviceInfo * deviceInfo = Pa_GetDeviceInfo(deviceIndex);

        if (deviceIndex == paNoDevice || !deviceInfo)
        {
            cout << "Failed to get device with given index." << endl;
            return false;
        }

        cout << "Output device name: " << deviceInfo->name << endl;

        PaStreamParameters outputParameters;

        outputParameters.device = deviceIndex;
        outputParameters.channelCount = CHANNELS_PER_FRAME;
        outputParameters.sampleFormat = paFloat32;
        outputParameters.suggestedLatency = deviceInfo->defaultLowOutputLatency;
        outputParameters.hostApiSpecificStreamInfo = NULL;

        PaError paError = Pa_OpenStream(
            &stream,
            NULL, // no input parameters
            &outputParameters,
            SAMPLE_RATE,
            FRAMES_PER_BUFFER,
            paClipOff,
            &Audio::paCallback,
            this);

        if (paError != paNoError)
        {
            cout << "Failed to open audio stream." << endl;
            return false;
        }

        paError = Pa_SetStreamFinishedCallback(stream, &Audio::paStreamFinished);

        if (paError != paNoError)
        {
            cout << "Failed to assign stream finished callback." << endl;
            Pa_CloseStream(stream);
            return false;
        }

        return true;
    }

    bool close()
    {
        if (!stream) return false;

        return Pa_CloseStream(stream) == paNoError;
    }

    bool start()
    {
        if (!stream) return false;

        return Pa_StartStream(stream) == paNoError;
    }

    bool stop()
    {
        if (!stream) return false;

        return Pa_StopStream(stream) == paNoError;
    }

private:
    int paCallbackMethod(
        const void * inputBuffer,
        void * outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo * timeInfo,
        PaStreamCallbackFlags statusFlags)
    {
        float * out = (float *)outputBuffer;
        float sine;
        float sineStep = M_PI * 2.0f * sineFreq / SAMPLE_RATE;

        for (unsigned long i = 0; i < framesPerBuffer; ++i)
        {
            sine = sinf(sinePhase + sineStep * i) * 0.5f;

            out[i * CHANNELS_PER_FRAME + 0] = sine;
            out[i * CHANNELS_PER_FRAME + 1] = sine;
        }

        sinePhase = fmodf(sinePhase + sineStep * framesPerBuffer, M_PI * 2.0f);

        return paContinue;
    }

    void paStreamFinishedMethod()
    {
        cout << "Stream Finished." << endl;
    }

    static int paCallback(
        const void * inputBuffer,
        void * outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo * timeInfo,
        PaStreamCallbackFlags statusFlags,
        void * userData)
    {
        return ((Audio *)userData)->paCallbackMethod(
            inputBuffer,
            outputBuffer,
            framesPerBuffer,
            timeInfo,
            statusFlags);
    }

    static void paStreamFinished(void * userData)
    {
        return ((Audio *)userData)->paStreamFinishedMethod();
    }

    PaError paInitError = -1;
    PaStream * stream = NULL;

    float sinePhase = 0.0f;
    float sineFreq = 220.0f;
};


#endif // __AUDIO_H__
