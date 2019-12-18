
#ifndef GAME_PAD_CTRL_RX_H
#define GAME_PAD_CTRL_RX_H

#include <Arduino.h>
#include <stdint.h>
#include "GamePadMsg.h"

namespace cerp
{
class GamePadCtrlRx
{
public:
    GamePadCtrlRx(Stream &stream);
    GamePadCtrlRx(GamePadCtrlRx &&)      = default;
    GamePadCtrlRx(const GamePadCtrlRx &) = default;
    GamePadCtrlRx &operator=(GamePadCtrlRx &&) = default;
    GamePadCtrlRx &operator=(const GamePadCtrlRx &) = default;
    ~GamePadCtrlRx();

    void begin(uint32_t serialTimeoutMs);
    bool parseOutCtrlData(char *printBuf, size_t printBufSize = 43);
    const GamePadInpCtr &getInpCtrl(void);
    void setGamePadCmd(const GamePadOutCtr &cmd);

private:
    Stream &mStream;                      ///< serial connection to i.e. communication module
    cerp::GamePadInpCtrlMsg mInpCtrlMsg;  ///< input message
    cerp::GamePadOutCtrlMsg mOutCtrlMsg;  ///< output message

    static const size_t PRINT_BUF_SIZE = 42;  ///< minimum print buffer length
};

}  // namespace cerp

#endif  // !GAME_PAD_CTRL_RX_H