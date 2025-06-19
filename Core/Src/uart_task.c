#include "uart_task.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "main.h"
#include "usart.h"
#include "dma.h"
#include "string.h"
#include "ymodem.h"
#include "extension.h"
#include "cmsis_os2.h" 

#define DoubleBuffer         // 控制是否启用双buffer   

#define UART_RX_BUF_SIZE    1280

#ifdef DoubleBuffer
#define UART_RX_BUF_NUM     2
static uint8_t uart_rx_buf[UART_RX_BUF_NUM][UART_RX_BUF_SIZE];
static uint8_t current_buf_index = 0;
static uint32_t uart_rx_len[UART_RX_BUF_NUM] = {0, 0};
#else
static uint8_t uart_rx_buf[UART_RX_BUF_SIZE];     // DMA接收缓冲区
static uint8_t process_buf[UART_RX_BUF_SIZE];     // 处理用的缓冲区
static uint32_t data_len = 0;   
#endif

extern osThreadId_t mainTaskHandle;
uint8_t flag = 0;

void Uart_Process(void *argument)
{
    uart_init();
    uint8_t ACK = 0x06;       
    uint8_t NAK = 0x15;
    uint8_t C = 0x43;
    uint16_t uart_pkg_len;      // 数据包长度
    uint8_t *p_uart_pkg;        // 数据包指针
    uint8_t *p_fram_data;       // 文件数据指针
    uint8_t ymodem_result;      // Ymodem数据包校验结果
    while(1)
    {
        // 等待串口事件
        if(osEventFlagsWait(UartEventHandle, 0x01, osFlagsWaitAny, osWaitForever) == 0x01)
        {
            
            // 处理接收到的数据
            #ifdef DoubleBuffer
            uint8_t process_buf_index = current_buf_index ^1;
            uart_pkg_len = uart_rx_len[process_buf_index];
            p_uart_pkg = uart_rx_buf[process_buf_index];
            #else
            uart_pkg_len = data_len;
            p_uart_pkg = process_buf;
            #endif
            // 这里添加数据处理代码 例如：解析数据、响应命令等
            // HAL_UART_Transmit(&huart6, p_uart_pkg, uart_pkg_len, 1000);
            ymodem_result = Ymodem_check(p_uart_pkg, uart_pkg_len);
            switch(ymodem_result)
            {
                case YMODEM_HEAD_OK:
                    flag = 1;
                    HAL_UART_Transmit(&huart6, &ACK, 1, 100);
                    HAL_UART_Transmit(&huart6, &C, 1, 100);
                    break;
                case YMODEM_FRAME_OK:
                    p_fram_data = &p_uart_pkg[3];   // 跳过开头3个字节，数据长度为fram_data_len
                    HAL_UART_Transmit(&huart6, &ACK, 1, 100);
                    break;
                case YMODEM_FRAME_REPEAT:
                    HAL_UART_Transmit(&huart6, &NAK, 1, 100);
                    break;
                case YMODEM_TRANFSER_END:
                    HAL_UART_Transmit(&huart6, &ACK, 1, 100);
                    HAL_UART_Transmit(&huart6, &C, 1, 100);
                    break;
                case YMODEM_TRANFSER_END_CONF:
                    HAL_UART_Transmit(&huart6, &ACK, 1, 100);
                    flag = 0;
                    break;
                case YMODEM_ERROR:
                    HAL_UART_Transmit(&huart6, &NAK, 1, 100);
                    break;
            }
          
        }
        
    }
}

// 回调中执行 DMA 切换
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART6)
    {
        // 保险做法：停止上一次 DMA（防止 HAL_BUSY）
        HAL_UART_DMAStop(huart);

        #ifdef DoubleBuffer
        uart_rx_len[current_buf_index] = Size;

        // 切换缓冲区
        current_buf_index ^= 1;

        // 重启 DMA
        HAL_UARTEx_ReceiveToIdle_DMA(huart, uart_rx_buf[current_buf_index], UART_RX_BUF_SIZE);
        #else
        // 快速复制数据
        memcpy(process_buf, uart_rx_buf, Size);

        data_len = Size;

        // 重启 DMA
        HAL_UARTEx_ReceiveToIdle_DMA(huart, uart_rx_buf, UART_RX_BUF_SIZE);
        #endif
        // 禁用DMA半满中断
        __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);

        osEventFlagsSet(UartEventHandle, 0x01);
    }
}

// 初始化串口和DMA
void uart_init(void)
{
    // 启动DMA接收
    #ifdef DoubleBuffer
    HAL_UARTEx_ReceiveToIdle_DMA(&huart6, uart_rx_buf[0], UART_RX_BUF_SIZE);
    #else
    HAL_UARTEx_ReceiveToIdle_DMA(&huart6, uart_rx_buf, UART_RX_BUF_SIZE);
    #endif
    // 禁用DMA半满中断
    __HAL_DMA_DISABLE_IT(huart6.hdmarx, DMA_IT_HT);
    
}
