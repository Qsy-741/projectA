#include "mainfunc.h"
#include "extension.h"
#include "FreeRTOS.h"
#include "cmsis_os.h" 
#include "cmsis_os2.h" 
#include "usart.h"


extern uint8_t flag;

void mainfunc(void)
{
    while(1)
    {
        osDelay(3000);
        if(flag == 0)
        {
            HAL_UART_Transmit(&huart6, "C", 1, 100);
        }
    }
}

