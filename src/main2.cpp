#include <iostream>
#include "audio.h"

using namespace std;

#define DEVICE_INDEX 0

int main(int argc, char ** argv)
{
    Audio audio;

    if (!audio.open(DEVICE_INDEX))
        return 1;

    if (!audio.start())
        return 1;

    //while (true);
    Pa_Sleep(5000); // ms

    audio.stop();
    audio.close();

    return 0;
}
