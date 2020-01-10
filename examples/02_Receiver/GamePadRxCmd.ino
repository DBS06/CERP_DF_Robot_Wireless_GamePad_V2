/**
 * @file CERP_Robot.ino
 * @author DBS06
 * @brief
 * @version 0.1
 * @date 2019-12-17
 *
 * @copyright Copyright (c) 2019
 *
 */
#include "Arduino.h"

#include <CerpGamePad.h>
#include "ArduinoTimer.h"

#define xbee Serial1
#define pc Serial

static const uint32_t serialTimeoutMs = 50;
ArduinoTimer timerLed;
ArduinoTimer timerPrint;

const size_t printBufSize   = 42;
char printBuf[printBufSize] = {};

cerp::GamePadCtrlRx gpcr(xbee);
cerp::GamePadOutCtr gpocr;

uint8_t vibrateState = 0;

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    pc.begin(115200);
    pc.flush();

    xbee.begin(115200);
    gpcr.begin(serialTimeoutMs);
}

void loop()
{
    if (gpcr.parseInpCtrlData(printBuf, printBufSize))
    {
        pc.println(printBuf);
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

        const cerp::GamePadInpCtr &ctr = gpcr.getInpCtrl();

        // do whatever you want to do with input control data i.e.:
        gpocr.vibration = ctr.b3;
        if (vibrateState != gpocr.vibration)
        {
            pc.println("set vibration");
            vibrateState = gpocr.vibration;
            gpcr.setGamePadCmd(gpocr);
        }
    }
}