// 扩展一些常用代码
#include "extension.h"

void my_memcpy(void *dst, void *src, uint16_t size)
{
    // 参数检查
    if (dst == NULL || src == NULL || size == 0) {
        return;
    }

    // 检查源和目标是否重叠
    if (((uint8_t *)dst <= (uint8_t *)src && (uint8_t *)dst + size > (uint8_t *)src) ||
        ((uint8_t *)src <= (uint8_t *)dst && (uint8_t *)src + size > (uint8_t *)dst)) {
        // 如果内存区域重叠，从后向前拷贝
        uint8_t *d = (uint8_t *)dst + size - 1;
        uint8_t *s = (uint8_t *)src + size - 1;
        while (size--) {
            *d-- = *s--;
        }
    } else {
        // 不重叠的情况，从前向后拷贝
        uint8_t *d = (uint8_t *)dst;
        uint8_t *s = (uint8_t *)src;
        
        // 首先尝试按 32 位对齐拷贝
        while (size >= 4 && (((uintptr_t)d & 3) == 0) && (((uintptr_t)s & 3) == 0)) {
            *(uint32_t *)d = *(uint32_t *)s;
            d += 4;
            s += 4;
            size -= 4;
        }
        
        // 剩余的字节按字节拷贝
        while (size--) {
            *d++ = *s++;
        }
    }
}

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
    my_memcpy(vector_table_ram, (void*)SCB->VTOR, vect_tab_size_bytes);

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




