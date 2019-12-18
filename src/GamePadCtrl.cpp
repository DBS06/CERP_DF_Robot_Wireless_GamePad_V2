#include "GamePadCtrl.h"
#include <Arduino.h>
#include <assert.h>
#include <stdio.h>
#include "GamePadMsg.h"
#include "GamePadPins.h"

#define PRINT_AS_HEX 0
#define ANALOG_SETTLE_MEASUREMENTS 75
#define ANALOG_SETTLE_TIME_MS 25
#define START_UP_DELAY_MS 500

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

cerp::GamePadCtrl::GamePadCtrl(const uint8_t filterWeight,
                               const int8_t analogStickUpperVal,
                               const int8_t analogStickLowerVal) :
    mInpCtrlMsg(),
    mOutCtrlMsg(),
    mStickUpperVal(analogStickUpperVal),
    mStickLowerVal(analogStickLowerVal),
    mEfLStickX(constrain(filterWeight, 0, 100), 0),
    mEfLStickY(constrain(filterWeight, 0, 100), 0),
    mEfRStickX(constrain(filterWeight, 0, 100), 0),
    mEfRStickY(constrain(filterWeight, 0, 100), 0),
    mLStickXCenterVal(0),
    mLStickYCenterVal(0),
    mRStickXCenterVal(0),
    mRStickYCenterVal(0)
{
}

cerp::GamePadCtrl::~GamePadCtrl()
{
}

void cerp::GamePadCtrl::begin(void)
{
    initPins();
    calibrate();

    mInpCtrlMsg.header.magic  = GPMH_MAGIC;
    mInpCtrlMsg.header.cmd    = GPMH_CMD_INP;
    mInpCtrlMsg.header.length = sizeof(GamePadInpCtr);

    // Signal button initialization finish
    digitalWrite(VIB_MOTOR, HIGH);
    digitalWrite(LED, HIGH);
    delay(START_UP_DELAY_MS);
    digitalWrite(VIB_MOTOR, LOW);
    digitalWrite(LED, LOW);
}

void cerp::GamePadCtrl::initPins(void)
{
    for (size_t i = 0; i < iMAX; i++)
    {
        pinMode(buttonPins[i], INPUT_PULLUP);
    }
    pinMode(LED, OUTPUT);
    pinMode(VIB_MOTOR, OUTPUT);
}

void cerp::GamePadCtrl::calibrate(void)
{
    // Wait to Settle down the analog-stick values and button states
    for (size_t i = 0; i < ANALOG_SETTLE_MEASUREMENTS; i++)
    {
        readAnalogSticks();
        delay(ANALOG_SETTLE_TIME_MS);
    }

    mLStickXCenterVal = mEfLStickX.Current();
    mLStickYCenterVal = mEfLStickY.Current();
    mRStickXCenterVal = mEfRStickX.Current();
    mRStickYCenterVal = mEfRStickY.Current();
}

void cerp::GamePadCtrl::updateInpCtrlData(void)
{
    readAnalogSticks();
    readButtons();
}

void cerp::GamePadCtrl::readButtons(void)
{
    mInpCtrlMsg.ctr.select = (digitalRead(buttonPins[iSELECT]) == HIGH ? 0 : 1);
    mInpCtrlMsg.ctr.start  = (digitalRead(buttonPins[iSTART]) == HIGH ? 0 : 1);
    mInpCtrlMsg.ctr.up     = (digitalRead(buttonPins[iUP]) == HIGH ? 0 : 1);
    mInpCtrlMsg.ctr.left   = (digitalRead(buttonPins[iLEFT]) == HIGH ? 0 : 1);
    mInpCtrlMsg.ctr.down   = (digitalRead(buttonPins[iDOWN]) == HIGH ? 0 : 1);
    mInpCtrlMsg.ctr.right  = (digitalRead(buttonPins[iRIGTH]) == HIGH ? 0 : 1);
    mInpCtrlMsg.ctr.b1     = (digitalRead(buttonPins[i1]) == HIGH ? 0 : 1);
    mInpCtrlMsg.ctr.b2     = (digitalRead(buttonPins[i2]) == HIGH ? 0 : 1);
    mInpCtrlMsg.ctr.b3     = (digitalRead(buttonPins[i3]) == HIGH ? 0 : 1);
    mInpCtrlMsg.ctr.b4     = (digitalRead(buttonPins[i4]) == HIGH ? 0 : 1);
    mInpCtrlMsg.ctr.l1     = (digitalRead(buttonPins[iL1]) == HIGH ? 0 : 1);
    mInpCtrlMsg.ctr.l2     = (digitalRead(buttonPins[iL2]) == HIGH ? 0 : 1);
    mInpCtrlMsg.ctr.l3     = (digitalRead(buttonPins[iL3]) == HIGH ? 0 : 1);
    mInpCtrlMsg.ctr.r1     = (digitalRead(buttonPins[iR1]) == HIGH ? 0 : 1);
    mInpCtrlMsg.ctr.r2     = (digitalRead(buttonPins[iR2]) == HIGH ? 0 : 1);
    mInpCtrlMsg.ctr.r3     = (digitalRead(buttonPins[iR3]) == HIGH ? 0 : 1);
}

void cerp::GamePadCtrl::readAnalogSticks(void)
{
    mEfLStickX.Filter(analogRead(joystickPins[aJLX]));
    mEfLStickY.Filter(analogRead(joystickPins[aJLY]));
    mEfRStickX.Filter(analogRead(joystickPins[aJRX]));
    mEfRStickY.Filter(analogRead(joystickPins[aJRY]));

    mInpCtrlMsg.ctr.leftStickX  = normalizeAnalogStickVal(mEfLStickX.Current(), mLStickXCenterVal);
    mInpCtrlMsg.ctr.leftStickY  = normalizeAnalogStickVal(mEfLStickY.Current(), mLStickYCenterVal);
    mInpCtrlMsg.ctr.rightStickX = normalizeAnalogStickVal(mEfRStickX.Current(), mRStickXCenterVal);
    mInpCtrlMsg.ctr.rightStickY = normalizeAnalogStickVal(mEfRStickY.Current(), mRStickYCenterVal);
}

int8_t cerp::GamePadCtrl::normalizeAnalogStickVal(const int16_t stickVal, const int16_t centerVal)
{
    if (stickVal > centerVal)
    {
        return constrain(map(stickVal, centerVal, ANALOG_READ_MAX_VAL, 0, mStickUpperVal), 0, mStickUpperVal);
    }
    else if (stickVal < centerVal)
    {
        return constrain(map(stickVal, centerVal, ANALOG_READ_MIN_VAL, 0, mStickLowerVal), 0, mStickLowerVal);
    }
    else
    {
        return 0;
    }
}

void cerp::GamePadCtrl::transmitInpCtrMsg(Stream &stream)
{
    stream.write(mInpCtrlMsg.data, sizeof(GamePadInpCtrlMsg));
}

void cerp::GamePadCtrl::printInpCtrlData(Serial_ &serial)
{
    const size_t printBufSize   = 64;
    char printBuf[printBufSize] = {};

#if PRINT_AS_HEX == 1
    snprintf(printBuf, printBufSize, "%04X|%04X|%04X|%04X|" UINT16_TO_BINARY_PATTERN, mInpCtrlMsg.ctr.leftStickX,
             mInpCtrlMsg.ctr.leftStickY, mInpCtrlMsg.ctr.rightStickX, mInpCtrlMsg.ctr.rightStickY,
             UINT16_TO_BINARY(mInpCtrlMsg.ctr.buttons));
#else
    snprintf(printBuf, printBufSize, "%04d|%04d|%04d|%04d|" UINT16_TO_BINARY_PATTERN, mInpCtrlMsg.ctr.leftStickX,
             mInpCtrlMsg.ctr.leftStickY, mInpCtrlMsg.ctr.rightStickX, mInpCtrlMsg.ctr.rightStickY,
             UINT16_TO_BINARY(mInpCtrlMsg.ctr.buttons));
#endif

    serial.println(printBuf);
}

bool cerp::GamePadCtrl::parseOutCtrlData(Stream &stream, Serial_ *serial)
{
    if (stream.available())
    {
        size_t rb = stream.readBytes(mOutCtrlMsg.data, sizeof(GamePadOutCtrlMsg));

        if (rb == sizeof(GamePadOutCtrlMsg) &&
            ((mOutCtrlMsg.header.magic == GPMH_MAGIC) && (mOutCtrlMsg.header.cmd == GPMH_CMD_OUT)))
        {
            if (serial != nullptr)
            {
                char printBuf[PRINT_BUF_SIZE] = {};

                snprintf(printBuf, PRINT_BUF_SIZE, "CMD -> %02X|%02X|%02X|%02X", mOutCtrlMsg.header.magic,
                         mOutCtrlMsg.header.cmd, mOutCtrlMsg.header.length, mOutCtrlMsg.ctr.cmds);

                serial->println(printBuf);
            }
            return true;
        }
        stream.flush();
    }
    return false;
}

void cerp::GamePadCtrl::execOutCtrlMsg()
{
    digitalWrite(VIB_MOTOR, (mOutCtrlMsg.ctr.vibration == 1 ? HIGH : LOW));
}
