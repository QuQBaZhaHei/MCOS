#ifndef INTeRRUPT_H

#define INTeRRUPT_H

//====================================================================================
/*
	外部汇编过程声明
*/
//====================================================================================

//====================================================================================
/*
	外部C函数声明
*/
//====================================================================================

void loopQueue_push(LoopQueue* loopQueue, Byte value);

Byte loopQueue_front(LoopQueue* loopQueue);

Byte loopQueue_pop(LoopQueue* loopQueue);

Uint32 loopQueue_isEmpty(LoopQueue* loopQueue);

Uint32 loopQueue_isFull(LoopQueue* loopQueue);

Pcb* loadUserProgram(Uint32 diskSector);




extern void _asm_interuptException();

extern void _asm_interruptGeneral();

extern void _asm_int0x000();

extern void _asm_int0x001();

extern void _asm_int0x002();

extern void _asm_int0x003();

extern void _asm_int0x004();

extern void _asm_int0x005();

extern void _asm_int0x006();

extern void _asm_int0x007();

extern void _asm_int0x008();

extern void _asm_int0x009();

extern void _asm_int0x00a();

extern void _asm_int0x00b();

extern void _asm_int0x00c();

extern void _asm_int0x00d();

extern void _asm_int0x00e();

extern void _asm_int0x00f();

extern void _asm_int0x010();

extern void _asm_int0x011();

extern void _asm_int0x012();

extern void _asm_int0x013();

//键盘中断
extern void _asm_int0x021();

//缺页中断(物理页用完）
extern void _asm_int0x028();

//页已使用中断(物理页存在，但是重分配)
extern void _asm_int0x029();

extern void _asm_int0x070();

extern void _asm_int0x080();

void keyBoardBuffer_push(Uint32 scanCode) {
    loopQueue_push(keyBoardBuffer, (Byte)scanCode);
}


#endif