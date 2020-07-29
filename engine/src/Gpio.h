#ifndef __GPIO_H__
#define __GPIO_H__

#include <iostream>
#include <cmath>
#include <string>
#include <cstdlib>
#include <stdint.h>

// Needed for GPIO pins
#include <wiringPi.h>
#include <wiringSerial.h>

// Needed for I2C port
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#include "porttime.h"
#include "Defines.h"
#include "Track.h"

#define XYZP_SIZE 4

using namespace std;

class Gpio
{
public:
    Gpio(Track ** tracks) :
        fd(-1),
        str(""),
        userX(0.0f), userY(0.0f),
        tracks(tracks)
    {
        for (int i = 0; i < XYZP_SIZE; ++i)
            xyzp[i] = 0.0f;
    }

    ~Gpio()
    { }

    bool setup()
    {
        // ===== Setup serial port ===== //

        cout << "[Gpio] Trying to open serial port." << endl;

        fd = serialOpen(SERIAL_PORT_NAME, SERIAL_BAUD_RATE);

        if (fd < 0)
        {
            cout << "[Gpio] Couldn't open de serial port." << endl;
            return false;
        }

        cout << "[Gpio] Serial port opened!" << endl;

        string cmd = "lep";

        cout << "[Gpio] Sending command: " << cmd << endl;

        serialFlush(fd);
        delay(1000);

        serialPuts(fd, "\r\r");
        delay(1000);

        serialPuts(fd, cmd.c_str());
        serialPutchar(fd, '\r');
        delay(1000);

        cout << "[Gpio] Command send. Listening to device..." << endl;


        // ===== Setup I2C ===== //

        string fileName = "/dev/i2c-" + to_string(I2C_BUS_NUMBER);

        if ((i2cFile = open(fileName.c_str(), O_RDWR)) < 0)
        {
            cout << "[Gpio] Failed to open the i2c bus." << endl;
            return false;
        }

        if (ioctl(i2cFile, I2C_SLAVE, I2C_SLAVE_ADDRESS) < 0)
        {
            cout << "[Gpio] Failed to acquire bus access and/or talk to the i2c slave." << endl;
            return false;
        }

        // writeSensorValue(i2cFile, 0x00, 0); // Set something


        // ===== Start timer callback ===== //

        PtError ptError = Pt_Start(
            GPIO_TIMER_INTERVAL,
            &Gpio::ptCallback,
            this);

        if (ptError != ptNoError)
        {
            cout << "[Gpio] Failed to start GPIO timer callback." << endl;
            serialClose(fd);
            return false;
        }


        // ===== Return true on succes ===== //

        return true;
    } // setup

    void close()
    {
        // ===== Stop timer callback ===== //

        Pt_Stop();


        // ===== Close serial port ===== //

        serialPuts(fd, "\r");
        delay(1000);

        cout << "[Gpio] End command send. Closing serial connection." << endl;

        serialClose(fd);
    }

private:
    void onReceivedPosition()
    {
        /*
         * position = x, y, z, pqf : bytes 0-12, position coordinates and quality factor
         * x : bytes 0-3, 32-bit integer, in meters
         * y : bytes 4-7, 32-bit integer, in meters
         * z : bytes 8-11, 32-bit integer, in meters
         * pqf : bytes 12, 8-bit integer, position quality factor in percent
         */


        float pqf = xyzp[3] / 100.0f;

        if (pqf < 0.0f) pqf = 0.0f;
        if (pqf > 1.0f) pqf = 1.0f;

        float pqfInv = 1.0f - pqf;

        userX = userX * pqfInv + xyzp[0] * pqf;
        userY = userY * pqfInv + xyzp[1] * pqf;

        //cout << "[Gpio] User Position: " << userX << "m, " << userY << "m" << endl;


        for (int i = 0; i < NUMBER_OF_TRACKS; ++i)
            tracks[i]->updateVolumeByUserPosition(userX, userY);
    }

    void readSerialData()
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
                        else
                        {
                            cout << "[Gpio] Received serial message: " << str << endl;
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
    } // readSerialData

    bool writeSensorValue(int fd, byte reg, byte val)
    {
        const int size = 2;
        byte buf[size] = { reg, val };

        if (write(fd, buf, size) != size)
        {
            cout << "[Gpio] Failed to write to the i2c bus." << endl;
            return false;
        }

        return true;
    }

    bool readSensorValues(int fd, byte reg, byte * buf, int size)
    {
        if (write(fd, &reg, 1) != 1)
        {
            cout << "[Gpio] Failed to write to the i2c bus." << endl;
            return false;
        }

        if (read(fd, buf, size) != size)
        {
            cout << "[Gpio] Failed to read from the i2c bus." << endl;
            return false;
        }

        return true;
    }

    int bytePairToInt(byte * pair)
    {
        byte flip[2] = { pair[1], pair[0] };

        return ((int16_t *) flip)[0];
    }

    void ptCallbackMethod(PtTimestamp timeStamp)
    {
        readSerialData();

        // readSensorValues(i2cFile, 0x3B, i2cBuffer, I2C_BUFFER_SIZE);
        //
        // float z    = bytePairToInt(i2cBuffer + 4) / 16384.0f;
        // float freq = z * 100.0f + 350.0f;
        //
        // for (int i = 0; i < NUMBER_OF_TRACKS; ++i)
        //     tracks[i]->setFrequency(freq);
    }

    static void ptCallback(PtTimestamp timeStamp, void * userData)
    {
        ((Gpio *) userData)->ptCallbackMethod(timeStamp);
    }

    // Serial
    int fd;
    string str;
    float xyzp[XYZP_SIZE];
    float userX, userY;

    // I2C
    int i2cFile;
    byte i2cBuffer[I2C_BUFFER_SIZE];

    // Engine
    Track ** tracks;
};

#endif // __GPIO_H__
