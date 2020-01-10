/**
 * @file GamePad.ino
 * @author DBS06
 * @brief see README.md
 * @version 1.0.6
 * @date 2019-12-17
 *
 * @copyright Copyright DBS06 (c) 2019
 *
 */
#include "Arduino.h"

#include <CerpGamePad.h>
#include "ArduinoTimer.h"

#define PRINT_INP_CTRL_DATA 1
#define PRINT_OUT_CTRL_DATA 1
#define FAST_PRINT 0

#define pc Serial
#define comModule Serial1
cerp::GamePadCtrl gpCtrl;

ArduinoTimer timerSendMsg;
ArduinoTimer timerPrint;

static const uint32_t intervalMs      = 50;
static const uint32_t serialTimeoutMs = 25;
static const uint32_t printMs         = 250;

#if PRINT_INP_CTRL_DATA == 1
Serial_ *pcPtr = &pc;
#else
Serial_ *pcPtr = nullptr;
#endif

void setup()
{
    /* init pc serial */
    pc.begin(115200);
    pc.setTimeout(serialTimeoutMs);

    /* init communcation module serial */
    comModule.begin(115200);
    comModule.setTimeout(serialTimeoutMs);

    /* init gamepadcontrol */
    gpCtrl.begin();

    /* flush serial rx buffer */
    delay(100);
    comModule.flush();
    pc.flush();
}

void loop()
{
    // Updates input control data
    gpCtrl.updateInpCtrlData();

    /* Transmit Input Controls */
    if (timerSendMsg.TimePassed_Milliseconds(intervalMs))
    {
        gpCtrl.transmitInpCtrMsg(comModule);

#if PRINT_INP_CTRL_DATA == 1 && FAST_PRINT == 1
        gpCtrl.printInpCtrlData(pc);
#endif
    }

#if PRINT_INP_CTRL_DATA == 1 && FAST_PRINT == 0
    if (timerPrint.TimePassed_Milliseconds(printMs))
    {
        gpCtrl.printInpCtrlData(pc);
    }
#endif

    /* msg's from communication module are prioritized over msg's from PC-serial,
     * to avoid contrary commands in the same loop */
    if (gpCtrl.parseOutCtrlData(comModule, pcPtr))
    {
        /* Execute Output-Controls */
        gpCtrl.execOutCtrlMsg();
    }
    else if (gpCtrl.parseOutCtrlData(pc, pcPtr))
    {
        gpCtrl.execOutCtrlMsg();
    }
}