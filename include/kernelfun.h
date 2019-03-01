#ifndef KERNELFUN_H

#define KERNELFUN_H

//====================================================================================
/*
	外部汇编过程声明
*/

//制作GDT选择子
extern void _asm_getGdtDescriptor(Uint32 baseAdress, Uint32 limted, Uint32 attribute, Byte* descriptor);

//制作LDT选择子
extern void _asm_getLdtDescriptor(Uint32 baseAdress, Uint32 limted, Uint32 attribute, Byte* descriptor);

//制作门选择子
extern void _asm_getGateDescriptor(Selector codeSelector, Uint32 entryAdress, Uint32 attribute, Byte* descriptor);

//安装GDT选择子
extern void _asm_setupGdtDescriptor(Byte* descriptor, Selector* selector);

//安装LDT选择子
extern void _asm_setupLdtDescriptor(Byte* descriptor, Selector* selector);

//安装中断门
extern void _asm_setupIntrruptGate(Uint32 IDTbaseAdress, Uint32 index, Byte* descriptor);

//初始化物理页映射串
extern void _asm_iniPageBitmap();

//关中断
extern void _asm_closeInterrupt();

//开中断
extern void _asm_openInterrupt();

//保存中断描述符表
extern void _asm_saveIDT(Uint32 IDTbaseAdress, Uint16 limted);

//初始化8259A芯片
extern void _asm_ini8259A();

//分配一个内核页
extern void _asm_allocOneKernelPage(Uint32 linearAddress);

////分配一个内核页
extern void _asm_allocOneUserPage(Uint32 linearAddress);

//寻找一个空闲的物理页
extern int _asm_alloc4kbPage();

//====================================================================================

//====================================================================================
/*
	外部C函数声明
*/

extern Byte loopQueue_front(LoopQueue* loopQueue);

extern Byte loopQueue_pop(LoopQueue* loopQueue);

extern Uint32 loopQueue_isEmpty(LoopQueue* loopQueue);

extern Uint32 loopQueue_isFull(LoopQueue* loopQueue);

extern void loopQueue_push(LoopQueue* loopQueue, Byte value);

//====================================================================================





//extern Uint32  _asm_alloc4kbPage();

//在进程中创建一个线程
void createTcb(Pcb* pcb, Uint32 entry); 

Pcb* loadUserProgram(Uint32 diskSector);

//分配内核内存
void* mallocKernel(Uint32 byteSize);

//分配用户内存
void* malloc(Pcb* pcb, Uint32 byteSize);

//将进程从就绪队列移除
void removeReadyPcb(Pcb* pcb);

//将进程添加到就绪队列
void appendReadyPcb(Pcb* pcb);

//将进程从阻塞队列移除
void removeWaitPcb(Pcb* pcb);

//将进程添加到阻塞队列
void appendWaitPcb(Pcb* pcb);

//在进程中创建一个线程
void appendTcb(Pcb* pcb, Tcb* tcb);

//在进程中移除一个线程
void removeTcb(Pcb* pcb, Tcb* tcb);

//阻塞一个进程
void sleepProcess(Pcb* pcb);

//阻塞一个线程
void sleepThread(Tcb* tcb);

//唤醒一个进程
void wakeUpProcess(Pcb* pcb);

//唤醒一个线程
void wakeUpThread(Tcb* tcb);

//字符串拷贝
void strcpy(Byte* str1, Byte* str2);

//计算字符串长度
Uint32 strlen(Byte* str);

//字符串比较
int strcmp(Byte* str1, Byte* str2);

//内存拷贝
void memcpy(void* p, void* q, Uint32 byteSize);

char getAscii();

void SysCall(Uint32 sysCallNumber, Uint32 args1, Uint32 args2, Uint32 args3, Uint32 args4);

//====================================================================================

void createTcb(Pcb* pcb, Uint32 entry) {
	CLI;
	Tcb* tcb = (Tcb*)mallocKernel(sizeof(Tcb));

	//分配内核栈
	tcb->esp0 = (Uint32)mallocKernel(4096) + 4096;
	//记录初始内核栈指针
	tcb->esp00 = tcb->esp0;
	//分配用户栈
	tcb->esp = 4096;
	//指定入口
	tcb->eip = entry;
	tcb->cs = r3CodeSel;
	tcb->ds = r3DataSel;
	tcb->es = r3DataSel;
	tcb->fs = r3DataSel;
	tcb->gs = r3DataSel;
	tcb->ss0 = 0x08;
	tcb->ss = r3DataSel;
	pcb->tcbHead = tcb;
	pcb->tcbCur = tcb;

	PUSH_SREG(tcb->cs, tcb->ds, tcb->es, tcb->fs, tcb->gs, tcb->ss0, tcb->ss, tcb->esp0, tcb->esp, tcb->eip);

	STI;
}

void* mallocKernel(Uint32 byteSize) {
	static Uint32 kernelNextAddress = 0x80400000;
	void* p = 0;
	if (kernelNextAddress == (kernelNextAddress / 4096 * 4096)) {
		_asm_allocOneKernelPage(kernelNextAddress);
	}
	if ((kernelNextAddress + byteSize) < (kernelNextAddress / 4096 * 4096 + 4096)) {
		//记录被分配内存的首地址
		p = (void*)(kernelNextAddress);
	}
	else {
		//计算需要分配的内存大小
		Uint32 size = byteSize - (kernelNextAddress / 4096 * 4096 + 4096) + kernelNextAddress;
		//计算需要分配4kb物理页的数量
		Uint32 allocTimes = size % 4096 == 0 ? size / 4096 : size / 4096 + 1;
		//计算起始线性地址
		Uint32 startAddress = kernelNextAddress / 4096 * 4096 + 4096;
		for (int i = 0; i < allocTimes; i++) {
			//分配一个页
			_asm_allocOneKernelPage(startAddress);
			startAddress += 4096;
		}
		//记录被分配内存的首地址
		p = (void*)(kernelNextAddress);
	}

	//更新下次分配的起始地址
	kernelNextAddress += byteSize;
	return p;
}

void* malloc(Pcb* pcb, Uint32 byteSize) {
	Uint32 nextAddress = pcb->nextAddress;
	void* p = NULL;
	if (nextAddress == (nextAddress / 4096 * 4096)) {
		_asm_allocOneUserPage(nextAddress);
	}
	if (nextAddress + byteSize < nextAddress / 4096 * 4096 + 4096) {
		//记录被分配内存的首地址
		p = (void*)nextAddress;
	}
	else {
		//计算需要分配的内存大小
		Uint32 size = byteSize - (nextAddress / 4096 * 4096 + 4096) + nextAddress;
		//计算需要分配4kb物理页的数量
		Uint32 allocTimes = size % 4096 == 0 ? size / 4096 : size / 4096 + 1;
		//计算起始线性地址
		Uint32 startAddress = nextAddress / 4096 * 4096 + 4096;
		
		for (Uint32 i = 0; i < allocTimes; i++) {
			//分配一个页
			_asm_allocOneUserPage(startAddress);
			startAddress += 4096;
		}
		//记录被分配内存的首地址
		p = (void*)(pcb->nextAddress);
		//更新下次分配的起始地址
		pcb->nextAddress += byteSize;
	}

	//更新下次分配的起始地址
	pcb->nextAddress += byteSize;
	return p;
}

void removeReadyPcb(Pcb* pcb) {
	static Uint32 signal = 0;
	while (signal == 1);
	CLI;
	signal = 1;
	STI;

	Pcb* p = readyPcbHead;

	//如果只有一个PCB
	if (readyPcbHead == pcb) {
		readyPcbHead = NULL;
		readyPcbEnd = NULL;
	}
	else {
		//基本的删除链表节点的操作
		if (p != NULL && pcb != NULL) {
			while (p->nextPCB != pcb && p->nextPCB != NULL) {
				p = p->nextPCB;
			}
			if (p->nextPCB != NULL) {
				p->nextPCB = pcb ->nextPCB;
			}
			if (p->nextPCB == NULL) {
				readyPcbEnd = p;
			}
		}
	}

	CLI;
	signal = 0;
	STI;
}

void appendReadyPcb(Pcb* pcb) {
	CLI;

	//如果目前进程阻塞
	if (pcb->state == PCB_STATE_WAIT) {
		//从阻塞队列中移除
		removeWaitPcb(pcb);
		//切换为就绪态
		pcb->state = PCB_STATE_READY;
	}
	//如果目前没有PCB
	if (readyPcbHead == NULL) {
		readyPcbHead = pcb;
		readyPcbEnd = pcb;
	}
	else {
		//基本的链表添加操作
		readyPcbEnd->nextPCB = pcb;
		readyPcbEnd = pcb;
		readyPcbEnd->nextPCB = NULL;
	}
	STI;
}

void removeWaitPcb(Pcb* pcb) {
	CLI;

	Pcb* p = waitPcbHead;
	//如果只有一个PCB
	if (waitPcbHead == pcb) {
		waitPcbHead = NULL;
		waitPcbEnd = NULL;
	}
	else {
		//基本的删除链表节点的操作
		if (p != NULL && pcb != NULL) {
			while (p->nextPCB != pcb && p->nextPCB != NULL) {
				p = p->nextPCB;
			}
			if (p->nextPCB != NULL) {
				p->nextPCB = pcb ->nextPCB;
			}
			if (p->nextPCB == NULL) {
				waitPcbEnd = p;
			}
		}
	}

	STI;
}

void appendWaitPcb(Pcb* pcb) {
	static Uint32 signal = 0;
	while (signal == 1);
	CLI;
	signal = 1;
	STI;

	//如果之前进程是就绪态
	if (pcb->state == PCB_STATE_READY) {
		//从就绪队列中移除
		removeReadyPcb(pcb);
		//切换为阻塞态
		pcb->state = PCB_STATE_WAIT;
	}
	//如果没有PCB
	if (waitPcbHead == NULL) {
		waitPcbHead = pcb;
		waitPcbEnd = pcb;
	}
	else {
		//基本的链表追加操作
		waitPcbEnd->nextPCB = pcb;
		waitPcbEnd = pcb;
		waitPcbEnd->nextPCB = NULL;
	}

	CLI;
	signal = 0;
	STI;
}

void appendTcb(Pcb* pcb, Tcb* tcb) {
	static Uint32 signal = 0;
	while (signal == 1);
	CLI;
	signal = 1;
	STI;

	//如果该进程下没有线程
	if (pcb->tcbHead == NULL) {
		pcb->tcbCur = tcb;
		pcb->tcbHead = tcb;
		tcb->next = NULL;
	}
	else {
		//基本的链表追加操作
		Tcb* tmp = pcb->tcbHead;
		pcb->tcbHead = tcb;
		tcb->next = tmp;
	}

	CLI;
	signal = 0;
	STI;

}

void sleepProcess(Pcb* pcb) {
	if (pcb->state == PCB_STATE_RUN) {
		//从就绪队列中移除
		removeReadyPcb(pcb);
		//添加到阻塞队列
		appendWaitPcb(pcb);
		//启动调度
		SWITCH_TASK;
	}
}

void sleepThread(Tcb* tcb) {
	if (tcb->state == TCB_STATE_RUN) {
		//设置为阻塞
		tcb->state = TCB_STATE_WAIT;
		//启动调度
		SWITCH_TASK;
	}
}

void wakeUpProcess(Pcb* pcb) {
	if (pcb->state == PCB_STATE_WAIT) {
		//从阻塞队列中移除
		removeWaitPcb(pcb);
		//添加到就绪队列
		appendReadyPcb(pcb);
	}
}

void wakeUpThread(Tcb* tcb) {
	if (tcb->state == TCB_STATE_WAIT) {
		//设置为就绪态
		tcb->state = TCB_STATE_READY;
	}
}

void strcpy(Byte* str1, Byte* str2) {
	Byte* p = str1;
	Byte* q = str2;
	while (*q != '\0') {
		*p = *q;
		p++,q++;
	}
	*p = '\0';
}

Uint32 strlen(Byte* str) {
	Byte* p = str;
	Uint32 len = 0;
	while (*p != '\0') {
		len++,p++;
	}
	return len;
}

int strcmp(Byte* str1, Byte* str2) {
	Byte* p = str1;
	Byte* q = str2;
	while (*p != '\0' && *q != '\0' && *p == *q) {
		p++,q++;
	}
	if (*p == '\0' && *q != '\0') {
		return -1;
	}
	else if(*q == '\0' && *p != '\0') {
		return 1;
	}
	else {
		return *p - *q;
	}
}

void memcpy(void* p, void* q, Uint32 byteSize) {
	for (Uint32 i = 0; i < byteSize; i++) {
		*((Byte*)p) = *((Byte*)q);
	}
}

char getAscii() {
	static int shiftState = 1;
	static int capsLockState = 1;
	if (loopQueue_isEmpty(keyBoardBuffer) == 0) {
		char c = loopQueue_pop(keyBoardBuffer);
		char x = '\0';
		switch (c) {
			case KEYBOARD_MAKECODE_Q : x = 'q'; break;
			case KEYBOARD_MAKECODE_W : x = 'w'; break;
			case KEYBOARD_MAKECODE_E : x = 'e'; break;
			case KEYBOARD_MAKECODE_R : x = 'r'; break;
			case KEYBOARD_MAKECODE_T : x = 't'; break;
			case KEYBOARD_MAKECODE_Y : x = 'y'; break;
			case KEYBOARD_MAKECODE_U : x = 'u'; break;
			case KEYBOARD_MAKECODE_I : x = 'i'; break;
			case KEYBOARD_MAKECODE_O : x = 'o'; break;
			case KEYBOARD_MAKECODE_P : x = 'p'; break;
			case KEYBOARD_MAKECODE_A : x = 'a'; break;
			case KEYBOARD_MAKECODE_S : x = 's'; break;
			case KEYBOARD_MAKECODE_D : x = 'd'; break;
			case KEYBOARD_MAKECODE_F : x = 'f'; break;
			case KEYBOARD_MAKECODE_G : x = 'g'; break;
			case KEYBOARD_MAKECODE_H : x = 'h'; break;
			case KEYBOARD_MAKECODE_J : x = 'j'; break;
			case KEYBOARD_MAKECODE_K : x = 'k'; break;
			case KEYBOARD_MAKECODE_L : x = 'l'; break;
			case KEYBOARD_MAKECODE_Z : x = 'z'; break;
			case KEYBOARD_MAKECODE_X : x = 'x'; break;
			case KEYBOARD_MAKECODE_C : x = 'c'; break;
			case KEYBOARD_MAKECODE_V : x = 'v'; break;
			case KEYBOARD_MAKECODE_B : x = 'b'; break;
			case KEYBOARD_MAKECODE_N : x = 'n'; break;
			case KEYBOARD_MAKECODE_M : x = 'm'; break;
			case KEYBOARD_MAKECODE_N0 : x = '0'; break;
			case KEYBOARD_MAKECODE_N1 : x = '1'; break;
			case KEYBOARD_MAKECODE_N2 : x = '2'; break;
			case KEYBOARD_MAKECODE_N3 : x = '3'; break;
			case KEYBOARD_MAKECODE_N4 : x = '4'; break;
			case KEYBOARD_MAKECODE_N5 : x = '5'; break;
			case KEYBOARD_MAKECODE_N6 : x = '6'; break;
			case KEYBOARD_MAKECODE_N7 : x = '7'; break;
			case KEYBOARD_MAKECODE_N8 : x = '8'; break;
			case KEYBOARD_MAKECODE_N9 : x = '9'; break;
			case KEYBOARD_MAKECODE_ENTER : x = '\n'; break;
			case KEYBOARD_MAKECODE_SPACE : x = ' '; break;
			case KEYBOARD_MAKECODE_CAPSLOCK : capsLockState *= -1; break;
			case KEYBOARD_MAKECODE_L_SHIFT : shiftState *= -1; break;
		}
		if (capsLockState == -1 && x >= 'a' && x <= 'z') {
			return x - 32;
		}
		else {
			return x;
		}
	}
	else {
		return '\0';
	}
}

void SysCall(Uint32 sysCallNumber, Uint32 args1, Uint32 args2, Uint32 args3, Uint32 args4) {
	switch (sysCallNumber) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
			__asm__ volatile (
				"pushl %0;"
				"call %1;"
				"addl $4,%%esp;"
				:
				:"r"(args1),"r"(SCT.entryAdress[sysCallNumber])
				:"%eax","%ecx","%edx","memory"
			); break;

		case 8:
			__asm__ volatile (
				"pushl %2;"
				"pushl %1;"
				"pushl %0;"
				"call %3;"
				"addl $12,%%esp;"
				:
				:"g"(args1),"g"(args2),"g"(args3),"g"(SCT.entryAdress[sysCallNumber])
				:"%eax","%ecx","%edx","memory"
			); break;

		case 9:
		case 10:
			__asm__ volatile (
				"pushl %3;"
				"pushl %2;"
				"pushl %1;"
				"pushl %0;"
				"call %4;"
				"addl $16,%%esp;"
				:
				:"g"(args1),"g"(args2),"g"(args3),"g"(args4),"g"(SCT.entryAdress[sysCallNumber])
				:"%eax","%ecx","%edx","memory"
			); break;
	}

}

#endif