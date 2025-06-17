#ifndef __UART_TASK_H
#define __UART_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cmsis_os.h"

// 声明事件句柄
extern osEventFlagsId_t UartEventHandle;

// 函数声明
void uart_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __UART_TASK_H */
