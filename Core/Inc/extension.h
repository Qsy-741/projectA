#ifndef __EXTENSION_H__
#define __EXTENSION_H__

#include "main.h"

typedef enum _LOG_LEVEL_E {    
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
	LOG_LEVEL_DEBUG,
}LOG_LEVEL_E;


#define LOG_ENABLE

extern LOG_LEVEL_E g_level;

// 日志打印
#ifdef LOG_ENABLE

// VT100 颜色转义序列
#define ANSI_COLOR_RED     "\033[31m"
#define ANSI_COLOR_GREEN   "\033[32m"
#define ANSI_COLOR_YELLOW  "\033[33m"
#define ANSI_COLOR_BLUE    "\033[34m"
#define ANSI_COLOR_RESET   "\033[0m"

#define log_printf(format, ...) \
    do { \
         if(g_level>=LOG_LEVEL_DEBUG)\
			 printf( ""format"", ##__VA_ARGS__);\
    } while (0)

#define log_info(format, ...) \
    do { \
         if(g_level>=LOG_LEVEL_INFO)\
             printf(ANSI_COLOR_GREEN"[%s][info](%d): "format ANSI_COLOR_RESET"\r\n", __func__, __LINE__, ##__VA_ARGS__);\
    } while (0)

#define log_warn(format, ...) \
    do { \
         if(g_level>=LOG_LEVEL_WARN)\
             printf(ANSI_COLOR_YELLOW"[%s][warn](%d): "format ANSI_COLOR_RESET"\r\n", __func__, __LINE__, ##__VA_ARGS__);\
    } while (0)

#define log_error(format, ...) \
    do { \
         if(g_level>=LOG_LEVEL_ERROR)\
             printf(ANSI_COLOR_RED"[%s][error](%d): "format ANSI_COLOR_RESET"\r\n", __func__, __LINE__, ##__VA_ARGS__);\
    } while (0)
#else

#define log_printf(format, ...)
#define log_info(format, ...)
#define log_warn(format, ...)
#define log_error(format, ...)

#endif

#endif

