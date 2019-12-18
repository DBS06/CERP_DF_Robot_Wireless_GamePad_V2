
#ifndef GAME_PAD_CTRL_RX_H
#define GAME_PAD_CTRL_RX_H

#include <stdint.h>
#include "Arduino.h"
#include "GamePadMsg.h"

namespace gpl
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

    bool parseOutCtrlData(Stream *serial);

private:
    Stream &mHwSerial;                   ///< serial connection to i.e. communication module
    gpl::GamePadInpCtrlMsg mInpCtrlMsg;  ///< input message
    gpl::GamePadOutCtrlMsg mOutCtrlMsg;  ///< output message
};

}  // namespace gpl

#endif  // !GAME_PAD_CTRL_RX_H