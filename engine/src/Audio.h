#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <iostream>
#include <cmath>
#include <portaudio.h>

#include "Defines.h"
#include "Track.h"

using namespace std;

class Audio
{
public:
    Audio(Track ** tracks) :
        stream(NULL),
        numInputChannels(0),
        numOutputChannels(0),
        tracks(tracks),
        currentFrame(0)
    {
        paInitError = Pa_Initialize();

        if (paInitError != paNoError)
            cout << "[Audio] Failed to initialize PortAudio." << endl;
    }

    ~Audio()
    {
        if (paInitError == paNoError)
            Pa_Terminate();
    }

    bool open(PaDeviceIndex deviceIndexAsArg = -1)
    {
        if (paInitError != paNoError)
        {
            cout << "[Audio] Cannot open audio stream if PortAudio failed to initialize." << endl;
            return false;
        }

        PaDeviceIndex numDevices = Pa_GetDeviceCount();
        PaDeviceIndex deviceIndex;
        const PaDeviceInfo * deviceInfo;

        cout << "[Audio] Available audio devices:" << endl;
        for (deviceIndex = 0; deviceIndex < numDevices; ++deviceIndex)
        {
            deviceInfo = Pa_GetDeviceInfo(deviceIndex);
            cout << deviceIndex << ": (";
            cout << deviceInfo->maxInputChannels << "i";
            cout << deviceInfo->maxOutputChannels << ") ";
            cout << deviceInfo->name << endl;
        }

        if (deviceIndexAsArg < 0)
        {
            cout << "[Audio] Select audio device by index: ";
            cin >> deviceIndex;
        }
        else
        {
            deviceIndex = deviceIndexAsArg;
        }

        deviceInfo = Pa_GetDeviceInfo(deviceIndex);
        if (deviceIndex == paNoDevice || !deviceInfo)
        {
            cout << "[Audio] Failed to get selected audio device info." << endl;
            return false;
        }

        cout << "[Audio] Selected audio device: " << deviceInfo->name << endl << endl;

        numInputChannels  = deviceInfo->maxInputChannels;
        numOutputChannels = deviceInfo->maxOutputChannels;

        PaSampleFormat sampleFormat = paFloat32;
        PaTime lowLatency = deviceInfo->defaultLowInputLatency;

        PaStreamParameters inputParameters;

        inputParameters.device           = deviceIndex;
        inputParameters.channelCount     = numInputChannels;
        inputParameters.sampleFormat     = sampleFormat;
        inputParameters.suggestedLatency = lowLatency;
        inputParameters.hostApiSpecificStreamInfo = NULL;

        PaStreamParameters outputParameters;

        outputParameters.device           = deviceIndex;
        outputParameters.channelCount     = numOutputChannels;
        outputParameters.sampleFormat     = sampleFormat;
        outputParameters.suggestedLatency = lowLatency;
        outputParameters.hostApiSpecificStreamInfo = NULL;

        PaError paError = Pa_OpenStream(
            &stream,
            NULL,// &inputParameters,
            &outputParameters,
            AUDIO_SAMPLE_RATE,
            AUDIO_BUFFER_SIZE,
            paNoFlag,
            &Audio::paCallback,
            this);

        if (paError != paNoError)
        {
            cout << "[Audio] Failed to open audio stream." << endl;
            return false;
        }

        paError = Pa_SetStreamFinishedCallback(stream, &Audio::paStreamFinished);

        if (paError != paNoError)
        {
            cout << "[Audio] Failed to assign stream finished callback." << endl;
            Pa_CloseStream(stream);
            return false;
        }

        paError = Pa_StartStream(stream);

        if (paError != paNoError)
        {
            cout << "[Audio] Failed to start audio stream." << endl;
            Pa_CloseStream(stream);
            return false;
        }

        return true;
    } // open

    void close()
    {
        Pa_StopStream(stream);
        Pa_CloseStream(stream);
    }

private:
    int paCallbackMethod(
        const float *                    inputBuffer,
        float *                          outputBuffer,
        frame_t                          framesPerBuffer,
        const PaStreamCallbackTimeInfo * timeInfo,
        PaStreamCallbackFlags            statusFlags)
    {
        for (frame_t i = 0; i < framesPerBuffer; ++i)
            for (int c = 0; c < numOutputChannels; ++c)
                outputBuffer[i * numOutputChannels + c] = 0.0f;

        for (int i = 0; i < NUMBER_OF_TRACKS; ++i)
            tracks[i]->process(
                inputBuffer,
                outputBuffer,
                framesPerBuffer,
                numInputChannels,
                numOutputChannels,
                currentFrame);

        currentFrame += framesPerBuffer;

        return paContinue;
    }

    void paStreamFinishedMethod()
    {
        cout << "[Audio] Stream Finished." << endl;
    }

    static int paCallback(
        const void *                     inputBuffer,
        void *                           outputBuffer,
        unsigned long                    framesPerBuffer,
        const PaStreamCallbackTimeInfo * timeInfo,
        PaStreamCallbackFlags            statusFlags,
        void *                           userData)
    {
        return ((Audio *) userData)->paCallbackMethod(
            (float *) inputBuffer,
            (float *) outputBuffer,
            framesPerBuffer,
            timeInfo,
            statusFlags);
    }

    static void paStreamFinished(void * userData)
    {
        ((Audio *) userData)->paStreamFinishedMethod();
    }

    PaError paInitError;
    PaStream * stream;

    int numInputChannels;
    int numOutputChannels;

    Track ** tracks;
    frame_t currentFrame;
};

#endif // __AUDIO_H__
