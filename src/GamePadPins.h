#ifndef GAME_PAD_PINS_H
#define GAME_PAD_PINS_H

/* Do it only if this file is used with an Arduino */
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#elif defined(ARDUINO) && ARDUINO < 100
#include "WProgram.h"
#include "pins_arduino.h"
#endif

#include <stdint.h>

namespace gpl
{
/**
 * @brief   GamePad-Control to Pin-Numbers.
 */
enum Pins
{
    BP_SELECT = 3,
    BP_START  = 4,
    BP_UP     = 5,
    BP_LEFT   = 6,
    BP_DOWN   = 7,
    BP_RIGHT  = 8,
    BP_1      = 9,
    BP_2      = 11,
    BP_3      = 12,
    BP_4      = 10,
    BP_L1     = 15,
    BP_L2     = 16,
    BP_L3     = A1,
    BP_R1     = 13,
    BP_R2     = 14,
    BP_R3     = A0,
    JPL_X     = A4,
    JPL_Y     = A5,
    JPR_X     = A2,
    JPR_Y     = A3,
    VIB_MOTOR = 2,
    LED       = 17,
};

/**
 * @brief   Digital-Input-Names.
 */
enum DigitalInputs
{
    iSELECT = 0,
    iSTART,
    iUP,
    iLEFT,
    iDOWN,
    iRIGTH,
    i1,
    i2,
    i3,
    i4,
    iL1,
    iL2,
    iL3,
    iR1,
    iR2,
    iR3,
    iMAX
};

/**
 * @brief   Analog-Input-Names.
 */
enum AnalogInputs
{
    aJLX = 0,
    aJLY,
    aJRX,
    aJRY,
    aMAX
};

/**
 * @brief   Digital-Input-Names to Pin-Number
 */
static const uint8_t buttonPins[iMAX] = {
    BP_SELECT, BP_START, BP_UP, BP_LEFT, BP_DOWN, BP_RIGHT, BP_1,  BP_2,
    BP_3,      BP_4,     BP_L1, BP_L2,   BP_L3,   BP_R1,    BP_R2, BP_R3,
};

/**
 * @brief   Analog-Input-Names to Pin-Number
 */
static const uint8_t joystickPins[aMAX] = {
    JPL_X,
    JPL_Y,
    JPR_X,
    JPR_Y,
};
}  // namespace gpl

#endif  // !GAME_PAD_PINS_H