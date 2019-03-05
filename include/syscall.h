#ifndef SYSCALL_H

#define SYSCALL_H


//====================================================================================
/*
	外部汇编过程声明
*/
//====================================================================================

//====================================================================================
/*
	外部C函数声明
*/

extern void touchFile(Byte* name);

//====================================================================================

struct HashTable* makeHashTable(Uint32 size, _hashCallBack hash);

void hashTable_insertHashNode(HashNodeHead* hashNodeHead, HashNode* hashNode);

void hashTable_eraseHashNode(HashNodeHead* hashNodeHead, HashNode* hashNode);

void hashTable_insert(HashTable* hashTable, char* str, Uint32 value);

Uint32 hashTable_find(HashTable* hashTable, char* str);

void hashTable_erase(HashTable* hashTable, char* str);

void addDiskRequest(Byte* buffer, Uint32 type, Uint32 sector);

void writeFile(Fcb* fcb, Byte* buffer, Uint32 logicStart, Uint32 byteSize);

void readFile(Fcb* fcb, Byte* buffer, Uint32 logicStart, Uint32 byteSize);

//======================================================================================


extern void _asm_readDisk(Byte* buffer,Uint32 diskSector);

extern void _asm_writeDisk(Byte* buffer, Uint32 diskSector);

extern void _asm_sreadDisk(Byte* buffer,Uint32 diskSector);

extern void _asm_swriteDisk(Byte* buffer, Uint32 diskSector);

extern void _asm_puts(char* str);

extern void _asm_putCharC(char c, char f);

extern void _asm_putChar(char c);

void putHex(Uint32 number);

int gets(char* str);

void syscall_createThread(Uint32 entry);

void syscall_fork(char* fileName);

void* syscall_malloc(Uint32 byteSize);

void syscall_free(void* p);

void syscall_open(File** fp, char* fileName, int flag);

void syscall_read(File* fp, Byte* buffer, Uint32 startByte ,Uint32 byteSize);

void syscall_write(File* fp, Byte* buffer, Uint32 startByte ,Uint32 byteSize);

void syscall_close(File* fp);



//=========================================================================

void putHex(Uint32 number) {
	_asm_puts("0x");
	Uint32 num[8];
	Uint32 k = 0;
	while(number !=0) {
		num[k++] = number % 16;
		number /= 16;
	}
	if (k == 0) {
		_asm_putChar('0');
	}
	else {
		for (Uint32 i = k - 1; i > 0; i--) {
			switch (num[i]) {
				case 0:
				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
				case 9:_asm_putChar(num[i] + '0');break;
				case 10:
				case 11:
				case 12:
				case 13:
				case 14:
				case 15:_asm_putChar(num[i] - 10 + 'A');break;
			}
		}
		switch (num[0]) {
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:_asm_putChar(num[0] + '0');break;
			case 10:
			case 11:
			case 12:
			case 13:
			case 14:
			case 15:_asm_putChar(num[0] - 10 + 'A');break;
		}
	}
}

int gets(char* str) {
	Uint32 k = 0;
	char c = getAscii();

	while (c != '\0' || c != '\n') {
		str[k++] = c;
		c = getAscii();
	}
	if ((c == '\0' && k == 0) || (c == '\n' && k == 0)) {
		return -1;
	}
	else {
		str[k] = '\0';
		return 0;
	}
}

void syscall_createThread(Uint32 entry) {
}

void* syscall_malloc(Uint32 byteSize) {
}

void syscall_free(void* p) {
	;
}

void syscall_open(File** fp, char* fileName, int flag) {
	static Byte* _buffer = NULL;
	if (_buffer == NULL) {
		_buffer = (Byte*)mallocKernel(512);
	}

	if (flag == 0) {
		touchFile(fileName);
	}

	Uint32 index = hashTable_find(FcbIndexTable, fileName);
	if (index == 0) {
		*fp = NULL;
	}
	else {
		addDiskRequest(_buffer, DISK_REQUSET_TYPE_READ, index);
		sleepThread(runPcb, runTcb);
		*fp = (File*)mallocKernel(sizeof(File));
		memcpy(*fp, _buffer, sizeof(File));
	}
	
}

void syscall_read(File* fp, Byte* buffer, Uint32 startByte ,Uint32 byteSize) {
	readFile(&(fp->fcb), buffer, startByte, byteSize);
}

void syscall_write(File* fp, Byte* buffer, Uint32 startByte ,Uint32 byteSize) {
	writeFile(&(fp->fcb), buffer, startByte, byteSize);
	_asm_puts(buffer);
}

void syscall_close(File* fp) {
	;
}

#endif