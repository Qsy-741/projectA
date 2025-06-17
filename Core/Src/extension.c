// 扩展一些常用代码


#define VECTOR_TABLE_SIZE (128) 
uint32_t vector_table_ram[VECTOR_TABLE_SIZE] __attribute__((aligned(512)));
// 向量表重映射到RAM
void Remap_Vector_Table_To_RAM(void) {
    // 1. 计算实际向量表的大小（以字节为单位）
    // STM32F446: 16 Core + 91 IRQs = 107 vectors
    // VECT_TAB_SIZE = 107 * 4 = 428 bytes
    const uint32_t vect_tab_size_bytes = (16 + 91) * 4; 

    // 2. 从Flash中拷贝向量表到SRAM
    // 原始向量表的地址存储在SCB->VTOR中
    memcpy(vector_table_ram, (void*)SCB->VTOR, vect_tab_size_bytes);

    // 3. 将SCB->VTOR指向SRAM中的新向量表
    // 确保在写VTOR之前所有内存访问都已完成
    __DSB(); // Data Synchronization Barrier
    SCB->VTOR = (uint32_t)vector_table_ram;
    __ISB(); // Instruction Synchronization Barrier
}
// 恢复向量表到Flash
void Restore_Vector_Table_To_Flash(void) {
    // 将VTOR恢复指向Flash的默认地址
    // 对于从Flash启动的应用，通常是0x08000000
    // FLASH_BASE是CubeMX/HAL库中定义的宏
    __DSB();
    SCB->VTOR = FLASH_BASE; 
    __ISB();
}

#define LOG_ENABLE

// 日志打印
#ifdef LOG_ENABLE
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define log_printf(format, ...) \
    do { \
         if(g_level>=LOG_LEVEL_Trans)\
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


