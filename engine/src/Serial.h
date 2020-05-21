#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <iostream>
#include <cmath>
#include <string>
#include <cstdlib>
#include <porttime.h>
#include <wiringPi.h>
#include <wiringSerial.h>

#include "Defines.h"
#include "Track.h"

#define XYZP_SIZE 4

using namespace std;

class Serial
{
public:
    Serial(Track ** tracks) :
        fd(-1),
        str(""),
        tracks(tracks)
    {
        for (int i = 0; i < XYZP_SIZE; ++i)
            xyzp[i] = 0.0f;
    }

    ~Serial()
    { }

    bool open(string& cmd)
    {
        cout << "[Serial] Trying to open serial port." << endl;

        fd = serialOpen(SERIAL_PORT_NAME, SERIAL_BAUD_RATE);

        if (fd < 0)
        {
            cout << "[Serial] Couldn't open de serial port." << endl;
            return false;
        }

        cout << "[Serial] Serial port opened!" << endl;

        PtError ptError = Pt_Start(
            SERIAL_TIMER_INT,
            &Serial::ptCallback,
            this);

        if (ptError != ptNoError)
        {
            cout << "[Serial] Failed to start serial timer callback." << endl;
            serialClose(fd);
            return false;
        }

        cout << "[Serial] Sending command: " << cmd << endl;

        serialFlush(fd);
        delay(1000);

        serialPuts(fd, "\r\r");
        delay(1000);

        serialPuts(fd, cmd);
        serialPutchar(fd, '\r');
        delay(1000);

        cout << "[Serial] Command send. Listening to device..." << endl;

        return true;
    }

    void close()
    {
        Pt_Stop();

        serialPuts(fd, "\r");
        delay(1000);

        cout << "[Serial] End command send. Closing serial connection." << endl;

        serialClose(fd);
    }

private:
    void onReceivedPosition()
    {
        cout << "[Serial] Position: ";

        for (int i = 0; i < XYZP_SIZE; ++i)
            cout << xyzp[i] << ", ";

        cout << endl;
    }

    void ptCallbackMethod(PtTimestamp timeStamp)
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
                            int ipos = 4;
                            int opos = 0;

                            for (int i = ipos; i < (int) str.length() && opos < XYZP_SIZE; ++i)
                            {
                                if (str[i] == ',' || str[i] == '\r')
                                {
                                    xyzp[opos] = atof(str.substr(ipos, i - ipos).c_str());
                                    ipos       = i + 1;
                                    opos++;
                                }
                            }

                            onReceivedPosition();
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
    }

    static void ptCallback(PtTimestamp timeStamp, void * userData)
    {
        ((Serial *) userData)->ptCallbackMethod(timeStamp);
    }

    int fd;
    string str;
    float xyzp[XYZP_SIZE];

    Track ** tracks;
};

#endif // __SERIAL_H__
