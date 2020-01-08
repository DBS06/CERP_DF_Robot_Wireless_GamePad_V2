#ifndef GAME_PAD_CTRL_H
#define GAME_PAD_CTRL_H

/* Do it only if this file is used with an Arduino */
#include <Arduino.h>
#include <stdint.h>
#include "Filter.h"
#include "GamePadMsg.h"
#include "GamePadPins.h"
#include "MegunoLink.h"

namespace cerp
{
/**
 * @brief Chaja Embedded Robot Platform - Gamepad Class represents the
 * "Wireless GamePad V2.0 for Arduino" from DF-Robot.
 *
 * **Features:**
 * *Messages are received and transmitted in a binary format.
 * *Code in main (setup() & loop()) is reduced to the minimum.
 * *Easy to use Class (GamePadCtrl) to get/set Input-/Output-Controls.
 * *Auto calibration for Analog-Sticks at startup.
 * *Noise filtering for Analog-Sticks.
 * *Analog-Stick's can be mapped to a user defined lower and upper bound.
 */
class GamePadCtrl
{
public:
    /**
     * @brief Construct a new GamePadCtrl Object
     *
     * @param filterWeight          Weight for new values, as a percentage (0-100).
     * @param analogStickUpperVal   Maximum upper value for Analog-Stick.
     * @param analogStickLowerVal   Minimum lower value for Analog-Stick.
     */
    GamePadCtrl(const uint8_t filterWeight       = 40,
                const int8_t analogStickUpperVal = 127,
                const int8_t analogStickLowerVal = -128);
    GamePadCtrl(GamePadCtrl &&)      = default;
    GamePadCtrl(const GamePadCtrl &) = default;
    GamePadCtrl &operator=(GamePadCtrl &&) = default;
    GamePadCtrl &operator=(const GamePadCtrl &) = default;
    ~GamePadCtrl();

    /**
     * @brief   Initializes the pin-modes and set default values.
     */
    void begin(void);

    /**
     * @brief   Updates game-pad-controls
     */
    void updateInpCtrlData(void);

    /**
     * @brief   Transmit game-pad-message as binary to given stream.
     *
     * @param stream    Stream where binary data will be sent.
     */
    void transmitInpCtrMsg(Stream &stream);

    /**
     * @brief   Prints game-pad-control data to serial-out in a human readable format.
     *
     * @param serial    Serial where binary data will be print.
     */
    void printInpCtrlData(Stream &serial);

    /**
     * @brief   Parses incoming command messages.
     *
     * @param stream    Stream where command message will be received.
     * @return true     Data available and verified.
     * @return false    No data availabler or verification failed.
     */
    bool parseOutCtrlData(Stream &stream, Stream *serial = nullptr);

    /**
     * @brief   Execute received command message.
     */
    void execOutCtrlMsg(void);

private:
    /**
     * @brief Initializes pins.
     * @note If initialization is finished, the gampad will blink and vibrate once.
     */
    void initPins(void);

    /**
     * @brief Calibrates Analog-Sticks.
     */
    void calibrate(void);

    /**
     * @brief Read Button-States.
     */
    void readButtons(void);

    /**
     * @brief Reads, filters and normalizes the Analog-Sticks.
     */
    void readAnalogSticks(void);

    /**
     * @brief   Normalizes the Analog-Sticks.
     *          *Analog-Stick-Center-Position Value = 0
     *          *Analog-Stick-Max-Postion Value = mStickUpperVal.
     *          *Analog-Stick-Min-Postion Value = mStickLowerVal.
     *
     * @param stickVal  filtered Analog-Stick-Axis Value.
     * @param centerVal Raw filtered Center-Value for Analog-Stick-Axis
     * @return int8_t  Normalized Analog-Stick-Axis Value.
     */
    int8_t normalizeAnalogStickVal(const int16_t stickVal, const int16_t centerVal);

    void calcCrc8Value(void);

    cerp::GamePadInpCtrlMsg mInpCtrlMsg;  ///< input control message
    cerp::GamePadOutCtrlMsg mOutCtrlMsg;  ///< output control message

    const uint8_t mStickUpperVal;  ///< Analog-Stick-Max-Postion Value
    const uint8_t mStickLowerVal;  ///< Analog-Stick-Min-Postion Value

    ExponentialFilter<long> mEfLStickX;  ///< filter for Left-StickX
    ExponentialFilter<long> mEfLStickY;  ///< filter for Left-StickY
    ExponentialFilter<long> mEfRStickX;  ///< filter for Right-StickX
    ExponentialFilter<long> mEfRStickY;  ///< filter for Right-StickY

    int16_t mLStickXCenterVal;  ///< filtered center vale for Left-StickX
    int16_t mLStickYCenterVal;  ///< filtered center vale for Left-StickY
    int16_t mRStickXCenterVal;  ///< filtered center vale for Right-StickX
    int16_t mRStickYCenterVal;  ///< filtered center vale for Right-StickY

    static const uint16_t ANALOG_READ_MIN_VAL = 0x0;        ///< analog read minimum value
    static const uint16_t ANALOG_READ_MAX_VAL = 0x400 - 1;  ///< analog read maximum value
    static const size_t PRINT_BUF_SIZE        = 42;         ///< minimum print buffer length
};

}  // namespace cerp

#endif  // GAME_PAD_CTRL_H