#include <iostream>
#include <unistd.h>

#include "Defines.h"
#include "Track.h"
#include "Audio.h"
// #include "Midi.h"

// #include <sndfile.h>
// #include <fftw3.h>

using namespace std;

int main(int argc, const char * argv[])
{
    cout << "[Main] Usage:" << endl;
    cout << "engine <audio-dev>" << endl;
    cout << endl;

    int inputChannelLeft  = 0;
    int inputChannelRight = 1;

    int audioDeviceIndex = -1;
    // int midiDeviceIndex   = -1;

    if (argc > 1) audioDeviceIndex = atoi(argv[1]);
    // if (argc > 2) midiDeviceIndex = atoi(argv[2]);

    cout << "[Main] Selected audio input channel left (" << inputChannelLeft + 1;
    cout << ") and right (" << inputChannelRight + 1 << ")" << endl;
    cout << endl;

    Track ** tracks = new Track *[NUMBER_OF_TRACKS];
    for (int i = 0; i < NUMBER_OF_TRACKS; ++i)
        tracks[i] = new Track(i, inputChannelLeft, inputChannelRight);

    Audio audio(tracks);
    // Midi midi(tracks);

    if (!audio.open(audioDeviceIndex))
        return 1;

    // if (!midi.open(midiDeviceIndex))
    // {
    //     audio.close();
    //     return 1;
    // }

    while (true)
        usleep(1000000);

    // midi.close();
    audio.close();

    return 0;
} // main
