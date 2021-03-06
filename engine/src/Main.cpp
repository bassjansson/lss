#include <iostream>
#include <unistd.h>
#include <fstream>
#include <string>

#include "Defines.h"
#include "Track.h"
#include "Audio.h"
#include "Gpio.h"

using namespace std;

int configFileReadError(const char * errorText, int i = 0)
{
    cout << "[Main] Config file read error!" << endl;
    cout << "\tError at: " << errorText << " " << i << endl;
    cout << "\tExiting program." << endl;

    return 1;
}

int main(int argc, const char * argv[])
{
    cout << "[Main] Usage:" << endl;
    cout << "engine <audio-dev> <send-lep>" << endl;
    cout << endl;

    // int inputChannelLeft  = 0;
    // int inputChannelRight = 1;

    int audioDeviceIndex = -1;
    bool sendLepToSensor = false;

    if (argc > 1) audioDeviceIndex = atoi(argv[1]);
    if (argc > 2) sendLepToSensor = atoi(argv[2]) > 0;

    // cout << "[Main] Selected audio input channel left (" << inputChannelLeft + 1;
    // cout << ") and right (" << inputChannelRight + 1 << ")" << endl;
    // cout << endl;

    ifstream config("config.txt");

    string label;
    float trackRadius;
    float volumeRadius;
    float volumeLowpass;
    float volumeThreshold;
    float volumeOutput;

    if (!(config >> label >> trackRadius)) return configFileReadError("trackRadius");

    if (!(config >> label >> volumeRadius)) return configFileReadError("volumeRadius");

    if (!(config >> label >> volumeLowpass)) return configFileReadError("volumeLowpass");

    if (!(config >> label >> volumeThreshold)) return configFileReadError("volumeThreshold");

    if (!(config >> label >> volumeOutput)) return configFileReadError("volumeOutput");

    if (!(config >> label >> label >> label >> label >> label)) return configFileReadError("track labels");

    string filePath;
    float x, y, w, h;

    Track ** tracks = new Track *[NUMBER_OF_TRACKS];

    for (int i = 0; i < NUMBER_OF_TRACKS; ++i)
    {
        if (!(config >> filePath >> x >> y >> w >> h)) return configFileReadError("track", i + 1);

        TrackData data;

        data.index = i;

        data.x = x + w / 2.0f;           // Center of track
        data.y = y + h / 2.0f;           // Center of track
        data.w = w / 2.0f - trackRadius; // Radius of track
        data.h = h / 2.0f - trackRadius; // Radius of track

        data.volumeRadius    = volumeRadius;
        data.volumeLowpass   = volumeLowpass;
        data.volumeThreshold = volumeThreshold;
        data.volumeOutput    = volumeOutput;

        tracks[i] = new Track(data, filePath.c_str());
        tracks[i]->startPlayback();
    }

    Audio audio(tracks);
    Gpio gpio(tracks);

    if (!audio.open(audioDeviceIndex))
        return 1;

    if (!gpio.setup(sendLepToSensor))
    {
        audio.close();
        return 1;
    }

    while (true)
    {
        for (int i = 0; i < NUMBER_OF_TRACKS; ++i)
            tracks[i]->loadAudioFromFile();

        usleep(10000L); // 10ms
    }

    gpio.close();
    audio.close();

    usleep(1000000L);

    return 0;
} // main
