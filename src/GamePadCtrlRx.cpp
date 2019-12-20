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
            msgValid = true;

            if (printBuf != nullptr && printBufSize >= PRINT_BUF_SIZE)
            {
                snprintf(printBuf, printBufSize, "CMD |%04d|%04d|%04d|%04d|" UINT16_TO_BINARY_PATTERN,
                         mInpCtrlMsg.ctr.leftStickX, mInpCtrlMsg.ctr.leftStickY, mInpCtrlMsg.ctr.rightStickX,
                         mInpCtrlMsg.ctr.rightStickY, UINT16_TO_BINARY(mInpCtrlMsg.ctr.buttons));
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

void cerp::GamePadCtrlRx::setGamePadCmd(const GamePadOutCtr &cmd)
{
    mOutCtrlMsg.ctr.cmds = cmd.cmds;
    mStream.write(mOutCtrlMsg.data, sizeof(GamePadInpCtrlMsg));
}
