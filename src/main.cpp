#include <iostream>
#include <unistd.h>

#include "defines.h"
#include "track.h"
#include "audio.h"

#include <sndfile.h>

using namespace std;

int main(int argc, const char * argv[])
{
    cout << "[Main] Usage:" << endl;
    cout << "lssapp <audio-dev>" << endl;
    cout << endl;

    int audioDeviceIndex = -1;
    if (argc > 1) audioDeviceIndex = atoi(argv[1]);

    Track ** tracks = new Track *[NUMBER_OF_TRACKS];
    for (int i = 0; i < NUMBER_OF_TRACKS; ++i)
        tracks[i] = new Track(i);

    Audio audio(tracks);

    if (!audio.open(audioDeviceIndex))
        return 1;

    audio.close();

    return 0;
} // main
