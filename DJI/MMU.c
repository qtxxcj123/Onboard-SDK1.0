#include "MMU.h"
#include "malloc.h"
MMU* mmu;
MMU mmuPtr = {
	interiMMUInit,
	setupMMU,
	allocMemory,
	freeMemory
};
void 
externMMUInit(void)
{
	mmu = (MMU*)mymalloc(sizeof(MMU));
	mmu->init = interiMMUInit;
	mmu->setupMMU = setupMMU;
	mmu->allocMemory = allocMemory;
	mmu->freeMemory  = freeMemory;
}

static void 
interiMMUInit(void)
{

}

void
setupMMU()
{
  uint32_t i;
  mmu->memoryTable[0].tabIndex  = 0;
  mmu->memoryTable[0].usageFlag = 1;
  mmu->memoryTable[0].pmem      = mmu->memory;
  mmu->memoryTable[0].memSize   = 0;
  for (i = 1; i < (MMU_TABLE_NUM - 1); i++)
  {
    mmu->memoryTable[i].tabIndex  = i;
    mmu->memoryTable[i].usageFlag = 0;
  }
  mmu->memoryTable[MMU_TABLE_NUM - 1].tabIndex  = MMU_TABLE_NUM - 1;
  mmu->memoryTable[MMU_TABLE_NUM - 1].usageFlag = 1;
  mmu->memoryTable[MMU_TABLE_NUM - 1].pmem      = mmu->memory + MEMORY_SIZE;
  mmu->memoryTable[MMU_TABLE_NUM - 1].memSize   = 0;
}

void
freeMemory(MMU_Tab* mmu_tab)
{
  if (mmu_tab == (MMU_Tab*)0)
  {
    return;
  }
  if (mmu_tab->tabIndex == 0 || mmu_tab->tabIndex == (MMU_TABLE_NUM - 1))
  {
    return;
  }
  mmu_tab->usageFlag = 0;
}

MMU_Tab*
allocMemory(uint16_t size)
{
  uint32_t mem_used = 0;
  uint8_t  i;
  uint8_t  j                = 0;
  uint8_t  mmu_tab_used_num = 0;
  uint8_t  mmu_tab_used_index[MMU_TABLE_NUM];

  uint32_t temp32;
  uint32_t temp_area[2] = { 0xFFFFFFFF, 0xFFFFFFFF };

  uint32_t record_temp32 = 0;
  uint8_t  magic_flag    = 0;

  if (size > PRO_PURE_DATA_MAX_SIZE || size > MEMORY_SIZE)
  {
    return (MMU_Tab *) 0;
  }

  for (i = 0; i < MMU_TABLE_NUM; i++)
  {
    if (mmu->memoryTable[i].usageFlag == 1)
    {
      mem_used += mmu->memoryTable[i].memSize;
      mmu_tab_used_index[mmu_tab_used_num++] = mmu->memoryTable[i].tabIndex;
    }
  }

  if (MEMORY_SIZE < (mem_used + size))
  {
    return (MMU_Tab *) 0;
  }

  if (mem_used == 0)
  {
    mmu->memoryTable[1].pmem      = mmu->memoryTable[0].pmem;
    mmu->memoryTable[1].memSize   = size;
    mmu->memoryTable[1].usageFlag = 1;
    return &mmu->memoryTable[1];
  }

  for (i = 0; i < (mmu_tab_used_num - 1); i++)
  {
    for (j = 0; j < (mmu_tab_used_num - i - 1); j++)
    {
      if (mmu->memoryTable[mmu_tab_used_index[j]].pmem >
          mmu->memoryTable[mmu_tab_used_index[j + 1]].pmem)
      {
        mmu_tab_used_index[j + 1] ^= mmu_tab_used_index[j];
        mmu_tab_used_index[j] ^= mmu_tab_used_index[j + 1];
        mmu_tab_used_index[j + 1] ^= mmu_tab_used_index[j];
      }
    }
  }
  for (i = 0; i < (mmu_tab_used_num - 1); i++)
  {
    temp32 = (uint32_t)(mmu->memoryTable[mmu_tab_used_index[i + 1]].pmem -
                                   mmu->memoryTable[mmu_tab_used_index[i]].pmem);

    if ((temp32 - mmu->memoryTable[mmu_tab_used_index[i]].memSize) >= size)
    {
      if (temp_area[1] > (temp32 - mmu->memoryTable[mmu_tab_used_index[i]].memSize))
      {
        temp_area[0] = mmu->memoryTable[mmu_tab_used_index[i]].tabIndex;
        temp_area[1] = temp32 - mmu->memoryTable[mmu_tab_used_index[i]].memSize;
      }
    }

    record_temp32 += temp32 - mmu->memoryTable[mmu_tab_used_index[i]].memSize;
    if (record_temp32 >= size && magic_flag == 0)
    {
      j          = i;
      magic_flag = 1;
    }
  }

  if (temp_area[0] == 0xFFFFFFFF && temp_area[1] == 0xFFFFFFFF)
  {
    for (i = 0; i < j; i++)
    {
      if (mmu->memoryTable[mmu_tab_used_index[i + 1]].pmem >
          (mmu->memoryTable[mmu_tab_used_index[i]].pmem +
           mmu->memoryTable[mmu_tab_used_index[i]].memSize))
      {
        memmove(mmu->memoryTable[mmu_tab_used_index[i]].pmem +
                  mmu->memoryTable[mmu_tab_used_index[i]].memSize,
                mmu->memoryTable[mmu_tab_used_index[i + 1]].pmem,
                mmu->memoryTable[mmu_tab_used_index[i + 1]].memSize);
        mmu->memoryTable[mmu_tab_used_index[i + 1]].pmem =
          mmu->memoryTable[mmu_tab_used_index[i]].pmem +
          mmu->memoryTable[mmu_tab_used_index[i]].memSize;
      }
    }

    for (i = 1; i < (MMU_TABLE_NUM - 1); i++)
    {
      if (mmu->memoryTable[i].usageFlag == 0)
      {
        mmu->memoryTable[i].pmem = mmu->memoryTable[mmu_tab_used_index[j]].pmem +
                              mmu->memoryTable[mmu_tab_used_index[j]].memSize;

        mmu->memoryTable[i].memSize   = size;
        mmu->memoryTable[i].usageFlag = 1;
        return &mmu->memoryTable[i];
      }
    }
    return (MMU_Tab*)0;
  }

  for (i = 1; i < (MMU_TABLE_NUM - 1); i++)
  {
    if (mmu->memoryTable[i].usageFlag == 0)
    {
      mmu->memoryTable[i].pmem =
        mmu->memoryTable[temp_area[0]].pmem + mmu->memoryTable[temp_area[0]].memSize;

      mmu->memoryTable[i].memSize   = size;
      mmu->memoryTable[i].usageFlag = 1;
      return &mmu->memoryTable[i];
    }
  }

  return (MMU_Tab*)0;
}
