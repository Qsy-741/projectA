#ifndef __EXTENSION_H__
#define __EXTENSION_H__

#include "main.h"

typedef enum _LOG_LEVEL_E {    
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
	LOG_LEVEL_DEBUG,
}LOG_LEVEL_E;

// 日志打印
#define LOG_ENABLE

extern LOG_LEVEL_E g_level;


#ifdef LOG_ENABLE

#define log_printf(format, ...) \
    do { \
         if(g_level>=LOG_LEVEL_DEBUG)\
			 printf( ""format"", ##__VA_ARGS__);\
    } while (0)

#define log_info(format, ...) \
    do { \
         if(g_level>=LOG_LEVEL_INFO)\
             printf("[%s][info](%d): "format "\r\n", __func__, __LINE__, ##__VA_ARGS__);\
    } while (0)

#define log_warn(format, ...) \
    do { \
         if(g_level>=LOG_LEVEL_WARN)\
             printf("[%s][warn](%d): "format "\r\n", __func__, __LINE__, ##__VA_ARGS__);\
    } while (0)

#define log_error(format, ...) \
    do { \
         if(g_level>=LOG_LEVEL_ERROR)\
             printf("[%s][error](%d): "format "\r\n", __func__, __LINE__, ##__VA_ARGS__);\
    } while (0)
#else

#define log_printf(format, ...)
#define log_info(format, ...)
#define log_warn(format, ...)
#define log_error(format, ...)

#endif

void my_memcpy(void *dst, void *src, uint16_t size);


#endif

