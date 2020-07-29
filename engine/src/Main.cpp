#include <iostream>
#include <unistd.h>

#include "Defines.h"
#include "Track.h"
#include "Audio.h"
#include "Gpio.h"

using namespace std;

int main(int argc, const char * argv[])
{
    cout << "[Main] Usage:" << endl;
    cout << "engine <audio-dev>" << endl;
    cout << endl;

    int inputChannelLeft  = 0;
    int inputChannelRight = 1;

    int audioDeviceIndex = -1;

    if (argc > 1) audioDeviceIndex = atoi(argv[1]);

    // cout << "[Main] Selected audio input channel left (" << inputChannelLeft + 1;
    // cout << ") and right (" << inputChannelRight + 1 << ")" << endl;
    // cout << endl;

    Track ** tracks = new Track *[NUMBER_OF_TRACKS];

    char fileName[16];

    for (int i = 0; i < NUMBER_OF_TRACKS; ++i)
    {
        float x = 1.5f + i;
        float y = 4.0f;
        float r = 0.35f;

        sprintf(fileName, "track%d.wav", i);

        tracks[i] = new Track(i, inputChannelLeft, inputChannelRight, x, y, r);
        tracks[i]->loadAudioFile(fileName);
    }

    Audio audio(tracks);
    Gpio gpio(tracks);

    if (!audio.open(audioDeviceIndex))
        return 1;

    if (!gpio.setup())
    {
        audio.close();
        return 1;
    }

    while (true)
        usleep(1000000L);

    gpio.close();
    audio.close();

    usleep(1000000L);

    return 0;
} // main
