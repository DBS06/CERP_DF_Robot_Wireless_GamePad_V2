#include "GamePadCtrlRx.h"

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

cerp::GamePadCtrlRx::GamePadCtrlRx(Stream &stream) : mStream(stream), mInpCtrlMsg(), mOutCtrlMsg()
{
}

cerp::GamePadCtrlRx::~GamePadCtrlRx()
{
}

void cerp::GamePadCtrlRx::begin(uint32_t serialTimeoutMs)
{
    mOutCtrlMsg.header.magic  = GPMH_MAGIC;
    mOutCtrlMsg.header.cmd    = GPMH_CMD_OUT;
    mOutCtrlMsg.header.length = sizeof(GamePadOutCtr);

    mStream.setTimeout(serialTimeoutMs);
    mStream.flush();
}

bool cerp::GamePadCtrlRx::parseInpCtrlData(char *printBuf, size_t printBufSize)
{
    bool msgValid = false;

    if (mStream.available())
    {
        size_t rb = mStream.readBytes(mInpCtrlMsg.data, sizeof(GamePadInpCtrlMsg));

        if (rb == sizeof(GamePadInpCtrlMsg) &&
            ((mInpCtrlMsg.header.magic == GPMH_MAGIC) && (mInpCtrlMsg.header.cmd == GPMH_CMD_INP)))
        {
            uint8_t crc8 = cerp::internal::crc8(mInpCtrlMsg.data, sizeof(GamePadInpCtrlMsg) - sizeof(uint8_t));

            if (crc8 == mInpCtrlMsg.crc8)
            {
                msgValid = true;
            }

            if (printBuf != nullptr && printBufSize >= PRINT_BUF_SIZE)
            {
                snprintf(printBuf, printBufSize, "CMD |%04d|%04d|%04d|%04d|" UINT16_TO_BINARY_PATTERN "|%02X",
                         mInpCtrlMsg.ctr.leftStickX, mInpCtrlMsg.ctr.leftStickY, mInpCtrlMsg.ctr.rightStickX,
                         mInpCtrlMsg.ctr.rightStickY, UINT16_TO_BINARY(mInpCtrlMsg.ctr.buttons), mInpCtrlMsg.crc8);
            }
            else
            {
                snprintf(printBuf, PRINT_BUF_SIZE, "CMD INVALID CRC -> %02X != %02X", mOutCtrlMsg.crc8, crc8);
            }
        }
        mStream.flush();
    }
    return msgValid;
}

const cerp::GamePadInpCtr &cerp::GamePadCtrlRx::getInpCtrl(void)
{
    return mInpCtrlMsg.ctr;
}

void cerp::GamePadCtrlRx::setGamePadCmd(const GamePadOutCtr &cmd, char *printBuf, size_t printBufSize)
{
    mOutCtrlMsg.ctr.cmds      = cmd.cmds;
    mOutCtrlMsg.ctr._reserved = 0xE;  // not 0x0, because if a byte-stream with '66|10|01|00|17' (vibration off), on the
                                      // receiver side only 4 instead of 5 bytes will be received. With setting this to
                                      // a value, everything is ok...
    mOutCtrlMsg.crc8 = cerp::internal::crc8(mOutCtrlMsg.data, sizeof(GamePadOutCtrlMsg) - sizeof(uint8_t));
    mStream.write(mOutCtrlMsg.data, sizeof(GamePadOutCtrlMsg));

    snprintf(printBuf, printBufSize, "CMD -> %02X|%02X|%02X|%02X|%02X", mOutCtrlMsg.header.magic,
             mOutCtrlMsg.header.cmd, mOutCtrlMsg.header.length, mOutCtrlMsg.ctr.cmds, mOutCtrlMsg.crc8);
}
