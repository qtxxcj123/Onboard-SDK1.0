#ifndef __MALLOC_H
#define __MALLOC_H
#include "stm32f4xx.h"
#ifndef NULL
#define NULL 0
#endif

#define MEM1_BLOCK_SIZE			32  	  						//�ڴ���СΪ32�ֽ�
#define MEM1_MAX_SIZE			80*1024  						//�������ڴ� 100K
#define MEM1_ALLOC_TABLE_SIZE	MEM1_MAX_SIZE/MEM1_BLOCK_SIZE 	//�ڴ���С


//�ڴ���������
struct _m_mallco_dev
{
	void (*init)();					//��ʼ��
	u8 (*perused)();		  	    	//�ڴ�ʹ����
	u8 	*membase;				//�ڴ�� ����SRAMBANK��������ڴ�
	u16 *memmap; 				//�ڴ����״̬��
	u8  memrdy; 				//�ڴ�����Ƿ����
};
extern struct _m_mallco_dev mallco_dev;	 //��mallco.c���涨��

void mymemset(void *s,u8 c,u32 count);	//�����ڴ�
void mymemcpy(void *des,void *src,u32 n);//�����ڴ�     
void my_mem_init(void);				//�ڴ�����ʼ������(��/�ڲ�����)
u32 my_mem_malloc(u32 size);	//�ڴ����(�ڲ�����)
u8 my_mem_free(u32 offset);		//�ڴ��ͷ�(�ڲ�����)
u8 my_mem_perused(void);				//����ڴ�ʹ����(��/�ڲ�����) 
////////////////////////////////////////////////////////////////////////////////
//�û����ú���
void myfree(void *ptr);  			//�ڴ��ͷ�(�ⲿ����)
void *mymalloc(u32 size);			//�ڴ����(�ⲿ����)
void *myrealloc(void *ptr,u32 size);//���·����ڴ�(�ⲿ����)

#endif













