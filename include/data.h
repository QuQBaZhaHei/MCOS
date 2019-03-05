#ifndef DATA_H
#define DATA_H

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


extern Byte* _asm_pgdt;

extern Byte* _asm_pldt;

extern Byte* pageBitmap;

//目录块映射串
Byte directoryBitmap[FILESYS_FORMATE_BITMAP_SECTOR_COUNT * 512];

//FCB块映射串
Byte fcbBitmap[FILESYS_FORMATE_BITMAP_SECTOR_COUNT * 512];

//inode块映射串
Byte inodeBitmap[FILESYS_FORMATE_BITMAP_SECTOR_COUNT * 512];

//数据区映射串
Byte dataBitmap[FILESYS_FORMATE_BITMAP_SECTOR_COUNT * 512];

//超级块指针
SFcb* sfcb = NULL;

//文件名-->FCB映射表
HashTable* FcbIndexTable = NULL;

//键盘缓冲队列
LoopQueue* keyBoardBuffer = NULL;

//文件系统魔数
Byte fileSysMagicNumber[5] = {'M', 'C', 'F', 'S', '\0'};

//磁盘IO请求队列
Heap* DrcbQueue = NULL;

//内核缓冲区
Byte kernelBuffer[1024];

//文件系统缓冲区
Byte fileSysBuffer[1024];

//中断函数指针数组
void (*interrupt[256])();

//中断描述符表
Byte IDT[256 * 8];

//系统调用表
Sct SCT;

//任务状态段
Tss TSS;

//任务状态段选择子
Uint32 tssSel;

LoopLinkList* readyPcbQueue = NULL;

LoopLinkList* waitPcbQueue = NULL;

LinkListNode* curPcbNode = NULL;

Pcb* runPcb = NULL;

Tcb* runTcb = NULL;

Tcb* shellTcb = NULL;

Tcb* kernelTcb = NULL;

Pcb* kernelPcb = NULL;

Uint32 r3CodeSel;

Uint32 r3DataSel;

#endif