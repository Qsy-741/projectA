#ifndef __YMODEM_H__
#define __YMODEM_H__

#include "main.h"

#define     YMODEM_ERROR            -1
#define     YMODEM_HEAD_OK           0
#define     YMODEM_FRAME_OK          1
#define     YMODEM_FRAME_REPEAT      2
#define     YMODEM_TRANFSER_END      3
#define     YMODEM_TRANFSER_END_CONF 4

#define     SOH     0x01
#define     STX     0x02
#define     EOT     0x04
#define     CAN     0x18
#define     CRC16   0x43


extern uint8_t file_name[128];
extern uint8_t file_size[32];
extern uint16_t fram_data_len;

int Ymodem_check(uint8_t *buf, uint16_t len);

uint16_t crc16_xmodem(const uint8_t *data, uint16_t length);

#endif

