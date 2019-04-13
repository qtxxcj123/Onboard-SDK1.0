#ifndef _MMU_H
#define _MMU_H
#define MMU_TABLE_NUM           32
#define MEMORY_SIZE             1024
#define PRO_PURE_DATA_MAX_SIZE  1007 // 2^10 - header size
#include "type.h"

//function point 

typedef void(*MMUInitFunc)(void); // ��ʼ��
typedef void(*MMUsetUpFunc)(void); // ����mmu
typedef MMU_Tab*(*MMUallocFunc)(uint16_t); // �����ڴ�
typedef void(*MMUFreeFunc)(MMU_Tab*);  // �ͷ��ڴ�

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
static void setupMMU(void);
static void freeMemory(MMU_Tab* mmu_tab);
static MMU_Tab* allocMemory(uint16_t size);


#endif
