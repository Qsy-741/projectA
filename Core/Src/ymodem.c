#include "ymodem.h"


uint8_t file_name[128];     // 文件名
uint8_t file_name_len = 0;  // 文件名长度
uint8_t file_size[32];     // 文件大小
uint8_t file_size_len = 0; // 文件大小长度
uint16_t fram_cnt = 0;     // 帧序号
uint16_t fram_data_len = 0; // 数据帧长度

int Ymodem_check(uint8_t *buf, uint16_t len)
{
    uint8_t *p_file_name;
    uint8_t tmp_flag = 0;
    uint16_t crc_check = 0;
    static EOT_flag = 0;

    if(!(buf[0]==SOH || buf[0]==STX || buf[0]==EOT))       // 包头不正确
    {
        return YMODEM_ERROR;
    }
    else if(buf[0]==EOT)        // 结束命令
    {
        fram_cnt = 0;   // 重置帧序号
        EOT_flag = 1;
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
    else if(buf[1] < fram_cnt)  // 帧序号为重复帧
    {
        return YMODEM_FRAME_REPEAT;
    }
    else
    {
        fram_cnt++;
    }

    if(buf[1] == 0)     // 开始处理数据，帧0为文件名和长度
    {
        for(uint16_t i = 0; i < fram_data_len; i++)    // 获取文件名和文件大小,越过前三个字节
        {
            if(buf[3+i] == 0x00)
            {
                file_name[i] = '\0';
                file_name_len = i;
                for(uint16_t j = 0; j < fram_data_len; j++)
                {
                    if(buf[3+i+j] == 0x00 || buf[3+i+j] == 0x20)
                    {
                        file_size[j] = '\0';
                        file_size_len = j;
                        break;
                    }
                    file_size[j] = buf[3+i+j];
                }
                break;
            }
            file_name[i] = buf[3+i];
        }
        return YMODEM_HEAD_OK;   // 起始帧处理完毕
    }
    else                        
    {
        for(uint8_t z = 0; z < 16; z++)  // 验证前16个字节，如果不为NULL则为数据帧
        {
            if(EOT_flag == 1)
            {
                return YMODEM_FRAME_OK;   // 数据帧ok
            }
        }
        fram_cnt = 0;
        fram_data_len = 0;
        return YMODEM_TRANFSER_END_CONF;   // 数据传输确认结束
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


