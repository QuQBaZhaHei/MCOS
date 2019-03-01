#ifndef TYPE_H

#define TYPE_h


typedef unsigned short Selector;
typedef unsigned int Adress;
typedef char Byte;
typedef unsigned int Uint32;
typedef unsigned short Uint16;


typedef struct PGDT {
	Uint16 limted;
	Adress baseAdress;
} Pgdt;

typedef struct PLDT {
	Uint16 limted;
	Adress baseAdress;
} Pldt;

/*
typedef struct SysCallItem {
	Uint32 entryAdress;
	Uint16 gateSel;
} SysCallItem;

typedef struct Sct {
	SysCallItem items[100];
	Uint32 len;
} Sct;

*/
typedef struct SysCallItem {
	Uint32 entryAdress;
} SysCallItem;

typedef struct Sct {
	Uint32 entryAdress[100];
	Uint32 len;
} Sct;

typedef struct Pidt {
	Uint16 limted;
	Adress baseAdress;
} Pidt;


//进程控制块
typedef struct PCB {
	char name[20];
	Uint32 ID;
	Uint32 nextAddress;
	Uint32 cr3Address;
	Uint32 state;
	struct TCB* tcbHead;
	struct TCB* tcbCur;
	struct PCB* nextPCB;
} Pcb;

//线程控制块
typedef struct TCB {
	char name[20];
	Uint32 ID;

	Uint32 cs;
	Uint32 ds;
	Uint32 es;
	Uint32 fs;
	Uint32 gs;
	Uint32 ss0;
	Uint32 ss;

	Uint32 esp0;
	Uint32 esp;
	Uint32 eip;
	Uint32 esp00;
	Uint32 state;
	struct TCB* next;

} Tcb;


//任务状态段
typedef struct TSS {
	struct TSS* prev;
	Uint32 esp0;
	Uint32 ss0;
	Uint32 esp1;
	Uint32 ss1;
	Uint32 esp2;
	Uint32 ss2;
	Uint32 cr3;
	Uint32 eip;
	Uint32 eflags;
	Uint32 eax;
	Uint32 ecx;
	Uint32 edx;
	Uint32 ebx;
	Uint32 esp;
	Uint32 ebp;
	Uint32 esi;
	Uint32 edi;
	Uint32 es;
	Uint32 cs;
	Uint32 ss;
	Uint32 ds;
	Uint32 fs;
	Uint32 gs;
	Uint32 LDTSel;
	Uint32 IO;
} Tss;

//ELF文件头
typedef struct ELF32Header {
	Byte e_ident[16];
	Uint16 e_type;
	Uint16 e_machine;
	Uint32 e_version;
	Uint32 e_entry;
	Uint32 e_phoff;
	Uint32 e_shoff;
	Uint32 e_flags;
	Uint16 e_ehsizes;
	Uint16 e_phentsize;
	Uint16 e_phnum;
	Uint16 e_shentsize;
	Uint16 e_shnum;
	Uint16 e_shstmdx;
} ELF32Header;

//ELF文件程序头
typedef struct ELF32ProgramHeader {
	Uint32 p_type;
	Uint32 p_offset;
	Uint32 p_vaddr;
	Uint32 p_paddr;
	Uint32 p_filesz;
	Uint32 p_memsz;
	Uint32 p_flags;
	Uint32 p_align;
} ELF32ProgramHeader;


//文件系统超级块
typedef struct SFCB {
	Byte magicNumber[8];

	Uint32 directoryBitmapSector;
	Uint32 directoryBitmapLength;
	Uint32 directorySector;
	Uint32 directoryCount;

	Uint32 fcbBitmapSector;
	Uint32 fcbBitmapLength;
	Uint32 fcbSector;
	Uint32 fcbCount;

	Uint32 inodeBitmapSector;
	Uint32 inodeBitmapLength;
	Uint32 inodeSector;
	Uint32 inodeCount;

	Uint32 dataBitmapSector;
	Uint32 dataBitmapLength;
	Uint32 dataSector;
} SFcb;

//文件控制块
typedef struct FCB {
	Uint32 type;
	Uint32 state;
	Uint32 count;
	Byte name[256];
	Uint32 byteSize;
	Uint32 allocByteSize;
	Uint32 inodeIndex;
} Fcb;

//索引块
typedef struct Inode {
	Uint32 fileType;
	Uint32 touchTimes;
	Uint32 iblockIndex;
} Inode;

typedef struct InodeTable {
	Uint32 type;
	Inode inode[1024];
} InodeTable;

//盘块
typedef struct Iblock {
	Uint32 type;
	Uint32 size;
	Uint32 sector[126 + 128 * (DISK_BLOCK_SECTOR_COUNT * 2 - 1)];
} Iblock;

//磁盘IO请求控制块
typedef struct DRCB {
	Pcb* pcb;
	Byte* buffer;
	Uint32 type;
	Uint32 sector;
} Drcb;

typedef int (*_compareCallBack)(Drcb x, Drcb y);

typedef struct Heap {
	Drcb* data;
	Uint32 size;
	Uint32 maxSize;
	_compareCallBack compare;
} Heap;


typedef Uint32 (*_hashCallBack)(char* str);

typedef struct HashNodeHead {
	Uint32 size;
	struct HashNode* head;
} HashNodeHead;

typedef struct HashNode {
	char fileName[256];
	Uint32 fcbIndex;
	struct HashNode* next;
} HashNode;

typedef struct HashTable {
	HashNodeHead* data;
	Uint32 size;
	Uint32 maxSize;
	_hashCallBack hash;
} HashTable;

typedef struct LoopQueue {
	Byte* scanCode;
	Uint32 front;
	Uint32 back;
	Uint32 size;
	Uint32 maxSize;
} LoopQueue;

//文件
typedef struct FILE {
	Fcb fcb;
} File;

#endif