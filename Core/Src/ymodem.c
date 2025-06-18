#include "ymodem.h"


uint8_t file_name[128];
uint8_t file_size[32];
uint16_t fram_cnt = 0;
uint16_t fram_data_len = 0;

int Ymodem_check(uint8_t *buf, uint16_t len)
{
    uint8_t *p_file_name;
    uint16_t i = 0;
    uint8_t tmp_flag = 0;
    uint16_t crc_check = 0;

    if(!(buf[0]==SOH || buf[0]==STX || buf[0]==EOT))       // 包头不正确
    {
        return YMODEM_ERROR;
    }
    else if(buf[0]==EOT)        // 结束命令
    {
        fram_cnt = 0;   // 重置帧序号
        return YMODEM_TRANFSER_END;
    }
    else
    {
       fram_data_len = (buf[0]==SOH)?128:1024; 
    }

    if(len != (fram_data_len+5))     // 数据包长度不正确,数据包长度=数据长度+5
    {
        return YMODEM_ERROR;
    }

    crc_check = crc16_xmodem(&buf[3],fram_data_len);
    if(crc_check != (buf[fram_data_len+3]<<8 | buf[fram_data_len+4]))     // 进行CRC校验
    {
        return YMODEM_ERROR;
    }

    if((buf[1]^0xFF)!=buf[2])    // 帧序号校验不通过
    {
        return YMODEM_ERROR;
    }
    else if(buf[1] < fram_cnt)  // 帧序号不匹配
    {
        return YMODEM_FRAME_REPEAT;
    }
    else
    {
        fram_cnt++;
    }

    if(buf[1] == 0)     // 开始处理数据，帧0为文件名和长度
    {
        for(i=0;i<fram_data_len;i++)    // 获取文件名和文件大小,越过前三个字节
        {
            if(tmp_flag == 0)
            {
                file_name[i] = buf[3+i];
            }
            else
            {
                file_size[i-1] = buf[3+i];
            }
            if(buf[3+i] == 0x00)
            {
                if(tmp_flag == 0)
                {
                    file_name[i] = '\0';
                    tmp_flag = 1;
                }
                else
                {
                    file_size[i] = '\0';
                    tmp_flag = 0;
                    break;
                }
            }
        }
        return YMODEM_HEAD_OK;   // 起始帧处理完毕
    }
    else                        
    {
        return YMODEM_FRAME_OK;   // 数据帧ok
    }
            
}
    


uint16_t crc16_xmodem(const uint8_t *data, uint16_t length)
{
    uint16_t crc = 0x0000;
    for (uint16_t i = 0; i < length; i++) {
        crc ^= (uint16_t)data[i] << 8;
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x8000)
                crc = (crc << 1) ^ 0x1021;
            else
                crc <<= 1;
        }
    }
    return crc;
}


