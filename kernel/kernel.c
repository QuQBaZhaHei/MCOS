#include "final.h"
#include "type.h"
#include "data.h"
#include "kernelfun.h"
#include "interrupt.h"
#include "syscall.h"
#include "heap.h"
#include "hashTable.h"
#include "fileSys.h"
#include "loopQueue.h"

extern void _asm_test();

//初始化系统调用表
void iniSysCall();

//初始化中断描述符表
void iniIDT();

//初始化内核TCB
void iniKernelTcb();

//初始化shellTCB
void iniShellTcb();

//初始化任务状态段
void iniTss();

//shell线程
void shell();

//初始化环3代码段和数据段
void iniRing3GdtDescriptor();

void putcommand();

void loop() {
	while (1);
}

Pcb* test() {
	Pcb* pcb = (Pcb*)mallocKernel(sizeof(Pcb));
	pcb->cr3Address = runPcb->cr3Address;
	pcb->nextPCB = NULL;
	createTcb(pcb, (Uint32)loop);
	return pcb;
}

int main(void) {
	_asm_iniPageBitmap();
	iniSysCall();
	iniIDT();
	iniFileSys();
	iniRing3GdtDescriptor();
	iniTss();
	iniKernelTcb();
	iniShellTcb();
	//建立键盘缓冲区
	keyBoardBuffer = makeLoopQueue(512);
	runPcb = kernelPcb;
	appendReadyPcb(kernelPcb);
	//appendReadyPcb(test);
	appendReadyPcb(loadUserProgram(50000));
	appendReadyPcb(loadUserProgram(60000));
	char c;
	while(1) {
		handleDiskRequest();
	}
	return 0;
}

void iniSysCall() {
	SCT.len = 0;
	SCT.entryAdress[SCT.len++] = (Uint32)_asm_putChar;
	SCT.entryAdress[SCT.len++] = (Uint32)_asm_puts;
	SCT.entryAdress[SCT.len++] = (Uint32)putHex;
	SCT.entryAdress[SCT.len++] = (Uint32)gets;
	SCT.entryAdress[SCT.len++] = (Uint32)syscall_malloc;
	SCT.entryAdress[SCT.len++] = (Uint32)syscall_createThread;
	SCT.entryAdress[SCT.len++] = (Uint32)syscall_close;
	SCT.entryAdress[SCT.len++] = (Uint32)syscall_free;//7
	//=======================================================================

	//=======================================================================
	SCT.entryAdress[SCT.len++] = (Uint32)syscall_open;
	//=======================================================================
	SCT.entryAdress[SCT.len++] = (Uint32)syscall_read;
	SCT.entryAdress[SCT.len++] = (Uint32)syscall_write;
	

}

void iniIDT() {
	Uint32 k = 0;
	Byte descriptor[8];
	Selector selector;
	interrupt[k++] = _asm_int0x000;
	interrupt[k++] = _asm_int0x001;
	interrupt[k++] = _asm_int0x002;
	interrupt[k++] = _asm_int0x003;
	interrupt[k++] = _asm_int0x004;
	interrupt[k++] = _asm_int0x005;
	interrupt[k++] = _asm_int0x006;
	interrupt[k++] = _asm_int0x007;
	interrupt[k++] = _asm_int0x008;
	interrupt[k++] = _asm_int0x009;
	interrupt[k++] = _asm_int0x00a;
	interrupt[k++] = _asm_int0x00b;
	interrupt[k++] = _asm_int0x00c;
	interrupt[k++] = _asm_int0x00d;
	interrupt[k++] = _asm_int0x00e;
	interrupt[k++] = _asm_int0x00f;
	interrupt[k++] = _asm_int0x010;
	interrupt[k++] = _asm_int0x011;
	interrupt[k++] = _asm_int0x012;
	interrupt[k++] = _asm_int0x013;
	
	//安装0x0 ~ 0x13号中断
	for (int i = 0; i < k; i++) {
		_asm_getGateDescriptor(CODE_GDT_4GB_SEL, (Uint32)interrupt[i], ATTRI_RING3_INT_GATE, descriptor);
		for (int j = 0; j < 8; j++) {
			IDT[i * 8 + j] = descriptor[j];
		}
	}
	
	//安装通用中断处理例程
	_asm_getGateDescriptor(CODE_GDT_4GB_SEL, (Uint32)_asm_interruptGeneral, ATTRI_RING3_INT_GATE, descriptor);
	
	for (int i = k; i < 256; i++) {
		for (int j = 0; j < 8; j++) {
			IDT[i * 8 + j] = descriptor[j];
		}
	}

	//安装键盘中断
	_asm_getGateDescriptor(CODE_GDT_4GB_SEL, (Uint32)_asm_int0x021, ATTRI_RING3_INT_GATE, descriptor);
	
	for (int i = 0; i < 8; i++) {
		IDT[33 * 8 + i] = descriptor[i];
	}

	//安装内存不足中断
	_asm_getGateDescriptor(CODE_GDT_4GB_SEL, (Uint32)_asm_int0x028, ATTRI_RING3_INT_GATE, descriptor);
	
	for (int i = 0; i < 8; i++) {
		IDT[40 * 8 + i] = descriptor[i];
	}
	
	//安装实时时钟中断
	_asm_getGateDescriptor(CODE_GDT_4GB_SEL, (Uint32)_asm_int0x070, ATTRI_RING3_INT_GATE, descriptor);
	
	for (int i = 0; i < 8; i++) {
		IDT[112 * 8 + i] = descriptor[i];
	}
	
	//安装系统调用中断
	_asm_getGateDescriptor(CODE_GDT_4GB_SEL, (Uint32)_asm_int0x080, ATTRI_RING3_INT_GATE, descriptor);
	
	for (int i = 0; i < 8; i++) {
		IDT[128 * 8 + i] = descriptor[i];
	}
	
	//使IDT生效
	_asm_saveIDT((Uint32)IDT, 256 * 8 - 1);
	//初始化8259A芯片
	_asm_ini8259A();
}

void iniKernelTcb() {
	Byte descriptor[8];
	kernelPcb = (Pcb*)mallocKernel(sizeof(Pcb));
	kernelPcb->state = TCB_STATE_READY;
	kernelTcb->state = TCB_STATE_READY;
	kernelPcb->ID = 0;
	kernelPcb->nextPCB = NULL;
	
	__asm__ volatile (
		"movl %%cr3,%0;"
		:"=r"(kernelPcb->cr3Address)
		:
		:
	);

	kernelTcb = (Tcb*)mallocKernel(sizeof(Tcb));
	kernelTcb->next = NULL;
	kernelPcb->tcbCur = kernelTcb;
	kernelPcb->tcbHead = kernelTcb;
}

void iniShellTcb() {
	shellTcb = (Tcb*)mallocKernel(sizeof(Tcb));
	shellTcb->ID = 1;
	shellTcb->next = NULL;
	shellTcb->state = TCB_STATE_READY;
	shellTcb->eip = (Uint32)shell;
	shellTcb->esp0 = (Uint32)mallocKernel(4096) + 4096;

	shellTcb->cs = 0x10;
	shellTcb->ds = 0x08;
	shellTcb->es = 0x08;
	shellTcb->fs = 0x08;
	shellTcb->gs = 0x08;
	shellTcb->ss0 = 0x08;
	shellTcb->ss = 0x08;

	//提前在内核栈压入执行任务切换所需要的寄存器
	PUSH_SREG(shellTcb->cs, shellTcb->ds, shellTcb->es, shellTcb->fs, shellTcb->gs, 
			shellTcb->ss0, shellTcb->ss, shellTcb->esp0, 0, shellTcb->eip);

	kernelPcb->tcbHead->next = shellTcb;

}

void iniTss() {
	Byte descriptor[8];
	//将TSS安装到GDT中
	_asm_getGdtDescriptor((Uint32)(&TSS), 103, ATTRI_TSS, descriptor);
	_asm_setupGdtDescriptor(descriptor,(Selector*)(&tssSel));

	//前指针置空
	TSS.prev = NULL;
	//目前不使用LDT
	TSS.LDTSel = 0;
	//没有IO位串
	TSS.IO = 0;
	unsigned short* io = (unsigned short*)(&(TSS.IO)) + 1;
	*io = 103;

	//安装TSS
	__asm__ volatile (
		"ltrw %0;"
		:
		:"m"(tssSel)
		:
	);
}

void iniRing3GdtDescriptor() {
	Byte descriptor[8];

	//创建并安装ring3 4gb代码段
	_asm_getGdtDescriptor(0, 0xfffff, ATTRI_GDT_RING3_CODE, descriptor);
	_asm_setupGdtDescriptor(descriptor, (Selector*)(&r3CodeSel));

	//创建并安装ring3 4gb数据段
	_asm_getGdtDescriptor(0, 0xfffff, ATTRI_GDT_RING3_DATA, descriptor);
	_asm_setupGdtDescriptor(descriptor, (Selector*)(&r3DataSel));

	//设置选择子的RPL字段为3
	__asm__ volatile (
		"movl %0,%%eax;"
		"orl $0x3,%%eax;"
		"movl %%eax,%0;"
		"movl %1,%%eax;"
		"orl $0x3,%%eax;"
		"movl %%eax,%1;"
		:
		:"m"(r3CodeSel),"m"(r3DataSel)
		:"%eax","memory"
	);
}

Pcb* loadUserProgram(Uint32 diskSector) {
	static Uint32 ID = 1;
	Uint32 oldCr3 = kernelPcb->cr3Address;
	Pcb* pcb = (Pcb*)mallocKernel(sizeof(Pcb));
	Tcb* tcb = (Tcb*)mallocKernel(sizeof(Tcb));
	tcb->next = NULL;
	pcb->tcbHead = tcb;
	pcb->tcbCur = tcb;
	tcb->ID = ID++;
	tcb->esp0 = (Uint32)mallocKernel(4096) + 4096;
	pcb->nextAddress = 4096;
	
	//分配一个4kb页作为页目录并拷贝内核页目录
	__asm__ volatile (
		"call _asm_alloc4kbPage;"//分配一个物理页
		"movl %%eax,%0;"//物理页地址在eax中
		"movl $0xfffffff8,%%ebx;"//页目录的倒数第二项
		"movl %%eax,%%edx;"
		"orl $0x00000007,%%edx;"//设置页目录的属性
		"movl %%edx,(%%ebx);"//写入页目录
		"invlpg (%%ebx);"//刷新高速缓存
		"movl $0xfffffeffc,%%ebx;"
		"movl %%edx,(%%ebx);"
		"movl $512,%%ecx;"//清空低一半的页目录
		"xorl %%edx,%%edx;"
		"pushl %%eax;"
		"movl $0xffffe000,%%eax;"
		"_clear:;"
		"movl $0x00000000,(%%eax,%%edx,0x4);"
		"incl %%edx;"
		"loop _clear;"
		"pushl %%esi;"
		"pushl %%edi;"
		"movl $512,%%ecx;"//拷贝内核页目录
		"movl $0xfffff800,%%esi;"
		"movl $0xffffe800,%%edi;"
		"rep movsd;"
		"popl %%edi;"
		"popl %%esi;"
		"popl %%eax;"
		"movl $0xffffeffc,%%edx;"
		"movl %%eax,%%ecx;"
		"orl $0x00000007,%%ecx;"
		"movl %%ecx,(%%edx);"
		"movl $0xfffffff8,%%edx;"
		"movl $0x00000000,(%%edx);"
		"invlpg (%%edx);"
		"movl %%eax,%%cr3;"
		"movl $0xfffffff8,%%edx;"
		"movl $0x00000000,(%%edx);"
		"invlpg (%%edx);"
		:"=m"(pcb->cr3Address)
		:
		:"%eax","%ebx","ecx","%edx","memory"
	);
	
	
	_asm_readDisk(kernelBuffer, diskSector);
	
	ELF32Header* header = (ELF32Header*)kernelBuffer;
	ELF32ProgramHeader* phHeader = (ELF32ProgramHeader*)(kernelBuffer + header->e_phoff);
	_asm_allocOneUserPage(0);

	ELF32ProgramHeader* tempPheader = phHeader;

	while ((phHeader + 1) != NULL && (phHeader + 1)->p_type == 1) {
		phHeader++;
	}
	malloc(pcb, phHeader->p_vaddr + phHeader->p_memsz / 4096 * 4096 + 4096);
	
	/*
	for (Uint32 i = 0; i < header->e_phnum; i++) {
		if (phHeader->p_type == 1) {
			malloc(pcb, (phHeader->p_memsz) / 4096 * 4096 + 4096);
		}
		phHeader++;
	}
	*/
	
	phHeader = tempPheader;
	
	Uint32 k = 0;

	//_asm_puts("\nzero\n");
	
	for (Uint32 i = 0; i < header->e_phnum; i++) {
		if (phHeader->p_type == 1) {
			k = 0;
			Byte* userMem = (Byte*)(phHeader->p_vaddr);
			Uint32 startSector = diskSector + (phHeader->p_offset) / 512;
			Uint32 times = (phHeader->p_memsz) % 512 == 0 ? (phHeader->p_memsz) / 512 : (phHeader->p_memsz) / 512 + 1;
			for (Uint32 j = 0; j < times; j++) {
				_asm_readDisk(kernelBuffer + 512, startSector++);
				for (Uint32 m = 0; m < 512; m++) {
					userMem[k++] = kernelBuffer[m + 512];
				}
			}
		}
		phHeader++;
	}
	
	
	tcb->eip = header->e_entry;
	tcb->cs = r3CodeSel;
	tcb->ds = r3DataSel;
	tcb->es = r3DataSel;
	tcb->fs = r3DataSel;
	tcb->gs = r3DataSel;
	tcb->ss0 = 0x08;
	tcb->ss = r3DataSel;
	tcb->esp = 4096;
	tcb->esp00 = tcb->esp0;

	PUSH_SREG(tcb->cs, tcb->ds, tcb->es, tcb->fs, tcb->gs, tcb->ss0, tcb->ss, tcb->esp0, tcb->esp, tcb->eip);
	SWITCH_CR3(oldCr3);
	_asm_puts("\nSucess!\n");
	
	return pcb;
}

void shell() {
	_asm_putChar('\n');
	char str[256];
	Uint32 k = 0;
	char c;
	
	while(1) {
		putcommand();
		c = getAscii();
		while (c != '\n') {
			if (c != '\0') {
				str[k++] = c;
				_asm_putChar(c);
			}
			c = getAscii();
		}
		str[k] = 0;
		_asm_putChar('\n');
	}
}

void putcommand() {
	_asm_putCharC('M', FONT_COLOUR_CHING);
	_asm_putCharC('C', FONT_COLOUR_CHING);
	_asm_putCharC('O', FONT_COLOUR_CHING);
	_asm_putCharC('S', FONT_COLOUR_CHING);
	_asm_putChar('@');
	_asm_putCharC('U', FONT_COLOUR_GREEN);
	_asm_putCharC('s', FONT_COLOUR_GREEN);
	_asm_putCharC('e', FONT_COLOUR_GREEN);
	_asm_putCharC('r', FONT_COLOUR_GREEN);

	_asm_putChar(':');
	_asm_putCharC('r', FONT_COLOUR_BLUE);
	_asm_putCharC('o', FONT_COLOUR_BLUE);
	_asm_putCharC('o', FONT_COLOUR_BLUE);
	_asm_putCharC('t', FONT_COLOUR_BLUE);
	_asm_putChar('>');
}













