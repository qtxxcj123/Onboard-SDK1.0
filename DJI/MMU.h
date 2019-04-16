#ifndef _MMU_H
#define _MMU_H
#define MMU_TABLE_NUM           32
#define MEMORY_SIZE             1024
#define PRO_PURE_DATA_MAX_SIZE  1007 // 2^10 - header size
#include "type.h"

//function point 
struct MMU;
typedef void(*MMUInitFunc)(void); // ��ʼ��
typedef void(*MMUsetUpFunc)(struct MMU*); // ����mmu
typedef MMU_Tab*(*MMUallocFunc)(struct MMU*,uint16_t); // �����ڴ�
typedef void(*MMUFreeFunc)(struct MMU*,MMU_Tab*);  // �ͷ��ڴ�

typedef struct MMU { // �� MMU
// function
	MMUInitFunc  init;
	MMUsetUpFunc setupMMU;
	MMUallocFunc allocMemory;
	MMUFreeFunc  freeMemory;
// variate 
	MMU_Tab memoryTable[MMU_TABLE_NUM];
	uint8_t memory[MEMORY_SIZE];
}MMU;

void externMMUInit(void);
static void interiMMUInit(void);
void setupMMU(MMU* mmu);
static void freeMemory(MMU* mmu,MMU_Tab* mmu_tab);
static MMU_Tab* allocMemory(MMU* mmu,uint16_t size);


#endif
