#ifndef GAME_PAD_MSG_H
#define GAME_PAD_MSG_H

/* Do it only if this file is used with an Arduino */
#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#elif defined(ARDUINO) && ARDUINO < 100
#include "WProgram.h"
#include "pins_arduino.h"
#endif

#include <stdint.h>

namespace cerp
{
/**
 * @brief   Structure to hold GamePad-Input-Controls.
 * @note    Will be gathered from Inside and sent to a Receiver.
 */
struct __attribute__((packed)) GamePadInpCtr
{
    // Sticks
    int8_t leftStickX;
    int8_t leftStickY;
    int8_t rightStickX;
    int8_t rightStickY;
    // Buttons
    union
    {
        struct __attribute__((packed))
        {
            uint16_t select : 1;
            uint16_t start : 1;
            uint16_t up : 1;
            uint16_t left : 1;
            uint16_t down : 1;
            uint16_t right : 1;
            uint16_t b1 : 1;
            uint16_t b2 : 1;
            uint16_t b3 : 1;
            uint16_t b4 : 1;
            uint16_t l1 : 1;
            uint16_t l2 : 1;
            uint16_t l3 : 1;
            uint16_t r1 : 1;
            uint16_t r2 : 1;
            uint16_t r3 : 1;
        };
        uint16_t buttons;
    };
};

/**
 * @brief   Structure to hold GamePad-Output-Controls.
 * @info    Will be set/received from outside.
 */
struct __attribute__((packed)) GamePadOutCtr
{
    union
    {
        struct __attribute__((packed))
        {
            uint8_t vibration : 1;
            uint8_t _reserved : 7;  // Reserved for future commands
        };
        uint8_t cmds;
    };
};

/**
 * @brief   Defines how the message header will look like
 */
struct __attribute__((packed)) GamePadMsgHeader
{
    uint8_t magic;   // Identifier to indicate that the message is from ourself.
    uint8_t cmd;     // Indicates if it s a TX or RX message.
    uint8_t length;  // Length of the data.
};

/**
 * @brief   Combination of GamePadMsgHeader and GamePadInpCtr.
 */
union GamePadInpCtrlMsg
{
    struct __attribute__((packed))
    {
        GamePadMsgHeader header;
        GamePadInpCtr ctr;
        uint8_t crc8;
    };
    uint8_t data[sizeof(GamePadMsgHeader) + sizeof(GamePadInpCtr) + sizeof(uint8_t)];
};

/**
 * @brief   Combination of GamePadMsgHeader and GamePadOutCtr.
 */
union GamePadOutCtrlMsg
{
    struct __attribute__((packed))
    {
        GamePadMsgHeader header;
        GamePadOutCtr ctr;
        uint8_t crc8;
    };
    uint8_t data[sizeof(GamePadMsgHeader) + sizeof(GamePadOutCtr) + sizeof(uint8_t)];
};

/* globale constants */
static const uint8_t GPMH_MAGIC   = 0x66;
static const uint8_t GPMH_CMD_INP = 0x01;
static const uint8_t GPMH_CMD_OUT = 0x10;

namespace internal
{
/**
 * @brief
 *
 * @param data
 * @param len
 * @return uint8_t
 */
uint8_t crc8(uint8_t data[], size_t len);
}  // namespace internal

}  // namespace cerp

#endif  // !GAME_PAD_MSG_H