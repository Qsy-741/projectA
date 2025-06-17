#include "mainfunc.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"


void mainfunc(void)
{
    while(1)
    {
        osDelay(1000);
        // printf("nihao\r\n");
    }
}
