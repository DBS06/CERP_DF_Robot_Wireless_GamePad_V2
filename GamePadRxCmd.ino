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

#include "./src/GamePadCtrlRx.h"
#include "./src/GamePadMsg.h"
#include "ArduinoTimer.h"

#define xbee Serial1
#define pc Serial
#define led0 LED_BUILTIN

#define UINT16_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c"
#define UINT16_TO_BINARY(byte)                              \
    (byte & 0x8000 ? '1' : '0'),     /* 1000000000000000 */ \
        (byte & 0x4000 ? '1' : '0'), /* 0100000000000000 */ \
        (byte & 0x2000 ? '1' : '0'), /* 0010000000000000 */ \
        (byte & 0x1000 ? '1' : '0'), /* 0001000000000000 */ \
        (byte & 0x0800 ? '1' : '0'), /* 0000100000000000 */ \
        (byte & 0x0400 ? '1' : '0'), /* 0000010000000000 */ \
        (byte & 0x0200 ? '1' : '0'), /* 0000001000000000 */ \
        (byte & 0x0100 ? '1' : '0'), /* 0000000100000000 */ \
        (byte & 0x0080 ? '1' : '0'), /* 0000000010000000 */ \
        (byte & 0x0040 ? '1' : '0'), /* 0000000001000000 */ \
        (byte & 0x0020 ? '1' : '0'), /* 0000000000100000 */ \
        (byte & 0x0010 ? '1' : '0'), /* 0000000000010000 */ \
        (byte & 0x0008 ? '1' : '0'), /* 0000000000001000 */ \
        (byte & 0x0004 ? '1' : '0'), /* 0000000000000100 */ \
        (byte & 0x0002 ? '1' : '0'), /* 0000000000000010 */ \
        (byte & 0x0001 ? '1' : '0')  /* 0000000000000001 */

static const uint32_t serialTimeoutMs = 50;
gpl::GamePadInpCtrlMsg mInpCtrlMsg;
ArduinoTimer timerLed;
ArduinoTimer timerPrint;
static const uint32_t keepAliveMs = 500;
static const uint32_t printMs     = 250;
volatile bool msgValid            = false;

const size_t printBufSize   = 64;
char printBuf[printBufSize] = {};

void setup()
{
    pinMode(led0, OUTPUT);
    digitalWrite(led0, HIGH);

    pc.begin(115200);
    pc.flush();

    xbee.begin(115200);
    xbee.setTimeout(serialTimeoutMs);
    xbee.flush();
}

void loop()
{
    if (xbee.available())
    {
        size_t rb = xbee.readBytes(mInpCtrlMsg.data, sizeof(gpl::GamePadInpCtrlMsg));

        if (rb == sizeof(gpl::GamePadInpCtrlMsg) &&
            ((mInpCtrlMsg.header.magic == gpl::GPMH_MAGIC) && (mInpCtrlMsg.header.cmd == gpl::GPMH_CMD_INP)))
        {
            msgValid = true;
            digitalWrite(led0, !digitalRead(led0));
            snprintf(printBuf, printBufSize, "CMD |%04d|%04d|%04d|%04d|" UINT16_TO_BINARY_PATTERN,
                     mInpCtrlMsg.ctr.leftStickX, mInpCtrlMsg.ctr.leftStickY, mInpCtrlMsg.ctr.rightStickX,
                     mInpCtrlMsg.ctr.rightStickY, UINT16_TO_BINARY(mInpCtrlMsg.ctr.buttons));
        }
        else
        {
            msgValid = false;
        }
    }
    xbee.flush();

    if (timerPrint.TimePassed_Milliseconds(printMs) && msgValid)
    {
        msgValid = false;
        pc.println(printBuf);
    }
}