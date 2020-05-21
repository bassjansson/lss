#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <iostream>
#include <cmath>
#include <porttime.h>

#include "Defines.h"
#include "Track.h"

using namespace std;

class Serial
{
public:
    Serial(Track ** tracks) :
        tracks(tracks)
    { }

    ~Serial()
    { }

    bool open()
    {
        PtError ptError = Pt_Start(
            SERIAL_TIMER_INT,
            &Serial::ptCallback,
            this);

        if (ptError != ptNoError)
        {
            cout << "[Serial] Failed to start serial timer callback." << endl;
            return false;
        }

        return true;
    }

    void close()
    {
        Pt_Stop();
    }

private:
    void ptCallbackMethod(PtTimestamp timeStamp)
    { }

    static void ptCallback(PtTimestamp timeStamp, void * userData)
    {
        ((Serial *) userData)->ptCallbackMethod(timeStamp);
    }

    Track ** tracks;
};

#endif // __SERIAL_H__
