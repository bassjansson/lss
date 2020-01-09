#include <iostream>
#include <unistd.h>
#include <string>
#include <cstdlib>

#include <wiringPi.h>
#include <wiringSerial.h>

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

    cout << "Trying to open serial port." << endl;

    int fd = serialOpen(SERIAL_PORT_NAME, SERIAL_BAUD_RATE);

    if (fd < 0)
    {
        cout << "Couldn't open de serial port.\nExiting program." << endl;
        return 1;
    }

    cout << "Serial port opened!" << endl;

    serialFlush(fd);
    delay(1000);

    serialPuts(fd, "\r\r");
    delay(1000);

    if (argc > 1)
    {
        cout << "Sending: " << argv[1] << endl;

        serialPuts(fd, argv[1]);
        serialPutchar(fd, '\r');
        delay(1000);

        cout << "Command send. Listing to device..." << endl;
    }

    string str = "";

    while (true)
    {
        if (serialDataAvail(fd) > 0)
        {
            int c = serialGetchar(fd);

            if (c > 0)
            {
                if (c == '\n')
                {
                    if (str.length() > 4)
                    {
                        if (str.substr(0, 3) == "POS")
                        {
                            float xyzp[4];
                            int ipos = 4;
                            int opos = 0;

                            for (int i = ipos; i < (int) str.length(); ++i)
                            {
                                if (str[i] == ',' || str[i] == '\r')
                                {
                                    xyzp[opos] = atof(str.substr(ipos, i - ipos).c_str());
                                    ipos       = i + 1;
                                    opos++;
                                }
                            }

                            for (int i = 0; i < 4; ++i)
                            {
                                cout << xyzp[i] << ", ";
                            }

                            cout << endl;
                        }
                    }

                    str = "";
                }
                else
                {
                    str += (char) c;
                }
            }
        }
        else
        {
            usleep(100000);
        }
    }

    serialPuts(fd, "\r");
    delay(1000);

    cout << "End command send. Closing serial connection." << endl;

    serialClose(fd);

    audio.close();

    return 0;
} // main
