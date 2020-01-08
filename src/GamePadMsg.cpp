#include "GamePadMsg.h"

uint8_t cerp::internal::crc8(uint8_t data[], size_t len)
{
    uint8_t crc = 0x00;
    uint8_t extract;
    uint8_t sum;

    for (size_t i = 0; i < len; i++)
    {
        extract = *data;
        for (char tempI = 8; tempI; tempI--)
        {
            sum = (crc ^ extract) & 0x01;
            crc >>= 1;
            if (sum)
                crc ^= 0x8C;
            extract >>= 1;
        }
        data++;
    }
    return crc;
}
