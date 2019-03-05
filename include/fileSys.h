#ifndef FILESYS_H

#define FILESYS_H

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


//磁盘请求优先级比较
int compare(Drcb x, Drcb y);

//文件名-->FCB索引的哈希函数
Uint32 hash(char* str);

//为某一段内存填充零
void fileNull(Byte* buffer, Uint32 byteSize) __attribute__((noinline)) __attribute__((optimize("O0")));

//从磁盘读取映射串
void readBitMap(Byte* buffer, Uint32 sector, Uint32 length);

//向磁盘写入映射串
void writeBitMap(Byte* buffer, Uint32 sector, Uint32 length);

//初始化文件系统
void iniFileSys();

//得到磁盘大小
Uint32 getDiskSize();

//得到空闲的目录块索引
Uint32 getFreeDirectorySector() __attribute__((noinline));

//回收目录块
void freeDirectorySector();

//得到空闲的FCB块索引
Uint32 getFreeFcbSector() __attribute__((noinline));

//回收FCB块
void freeFcbSector();

//得到空闲的inode块索引
Uint32 getFreeInodeSector() __attribute__((noinline));

//回收inode块
void freeInodeSector();

//得到空闲的数据区的磁盘块索引
Uint32 getFreeDataBlock() __attribute__((noinline));

//回收数据区磁盘块
void freeDataBlock();

//添加磁盘IO请求
void addDiskRequest(Byte* buffer, Uint32 type, Uint32 sector);

//得到下一给磁盘IO请求
Drcb getNextDrcb();

//处理磁盘IO请求
void handleDiskRequest();

//加载索引块到内存
void loadIblock(Byte* buffer, Uint32 index);

//保存索引块到磁盘
void saveIblock(Byte* buffer, Uint32 index);

//读取索引块内的磁盘块
void readIblock(Byte* buffer, Iblock* block, Uint32 pos, Uint32 size);

//写入索引块内的磁盘块
void writeIblock(Byte* buffer, Iblock* block, Uint32 start, Uint32 size);

//读取第一级间接索引
void readIblockFirst(Byte* buffer, Iblock* iblockFirst, Uint32 blockIndex, Uint32 offset, Uint32 size);

//写入第一级间接索引
void writeIblockFirst(Byte* buffer, Iblock* iblockFirst, Uint32 blockIndex, Uint32 offset, Uint32 size);

//读取第二级间接索引
void readIblockSecond(Byte* buffer, Iblock* iblockSecond, Uint32 blockIndex, Uint32 offset, Uint32 size);

//写入第二级间接索引
void writeIblockSecond(Byte* buffer, Iblock* iblockSecond, Uint32 blockIndex, Uint32 offset, Uint32 size);

//读取第三级间接索引
void readIblockThird(Byte* buffer, Iblock* iblockThird, Uint32 blockIndex, Uint32 offset, Uint32 size);

//写入第三级间接索引
void writeIblockThird(Byte* buffer, Iblock* iblockThird, Uint32 blockIndex, Uint32 offset, Uint32 size);

//创建文件
void touchFile(Byte* name);

//删除文件
void eraseFile(Fcb* fcb);

//写文件
void writeFile(Fcb* fcb, Byte* buffer, Uint32 logicStart, Uint32 byteSize);

//读文件
void readFile(Fcb* fcb, Byte* buffer, Uint32 logicStart, Uint32 byteSize);

//扩充文件空间
void extendFile(Fcb* fcb, Uint32 newByteSize);

//扩充直接索引
Uint32 extendIblock(Iblock* iblock, Uint32 saveIndex,Uint32 times);

//扩充第一级间接索引
Uint32 extendIblockFirst(Iblock* iblockFirst, Uint32 saveIndex,Uint32 times);

//扩充第二级间接索引
Uint32 extendIblockSecond(Iblock* iblockSecond, Uint32 saveIndex,Uint32 times);

//扩充第三级间接索引
Uint32 extendIblockThird(Iblock* iblockThird, Uint32 saveIndex,Uint32 times);

//===================================================================================

int compare(Drcb x, Drcb y) {
	return (int)(x.sector) - (int)(y.sector);
}

Uint32 hash(char* str) {
	Uint32 h = 0;
	while (*str != '\0') {
		h = ((h << 5) + *str++) % HASH_TABLE_SIZE;
		str++;
	}
	return h % HASH_TABLE_SIZE;
}

void fileNull(Byte* buffer, Uint32 byteSize) {
	//for (Uint32 i = 0; i < byteSize; i++) {
	//	buffer[i] = 0;
	//}
	__asm__ volatile (
		"movl %1,%%ecx;"
		"xorl %%eax,%%eax;"
		"xorl %%ebx,%%ebx;"
		"movl %0,%%esi;"
		"_fill:;"
		"movl (%%esi,%%eax,4),%%ebx;"
		:
		:"m"(buffer),"m"(byteSize)
		:"%ecx","%eax","%ebx"
	);
}

void readBitMap(Byte* buffer, Uint32 sector, Uint32 length) {
	static Byte* _buffer = NULL;
	if (_buffer == NULL) {
		_buffer = (Byte*)mallocKernel(1024);
	}

	//计算映射串所占扇区数
	Uint32 times = length % (512 * 8) == 0 ? length / (512 * 8) : length / (512 * 8) + 1;
	Uint32 k = 0;
	for (Uint32 i = 0; i < times; i++) {
		_asm_readDisk(_buffer, sector++);
		for (Uint32 j = 0; j < 512; j++) {
			buffer[k++] = _buffer[i];
		}
	}
}

void writeBitMap(Byte* buffer, Uint32 sector, Uint32 length) {
	//计算映射串所占扇区数
	Uint32 times = length % (512 * 8) == 0 ? length / (512 * 8) : length / (512 * 8) + 1;
	for (Uint32 i = 0; i < times; i++) {
		_asm_writeDisk(buffer + i * 512, sector++);
	}
}

void iniFileSys() {
	Uint32 startSector = FILESYS_FORMATE_START_SECTOR;
	//创建磁盘IO队列
	Heap* heap = makeHeap(1000, compare);
	DrcbQueue = heap;

	//创建超级块的内存
	sfcb = (SFcb*)mallocKernel(sizeof(SFcb));

	//尝试读取超级块
	_asm_readDisk(fileSysBuffer, FILESYS_SFCB_SECTOR);
	strcpy(sfcb->magicNumber, fileSysBuffer);

	//初始化文件名-->FCB映射表
	FcbIndexTable = makeHashTable(HASH_TABLE_SIZE, hash);

	//是否需要初始化
	if (strcmp(sfcb->magicNumber, fileSysMagicNumber) != 0) {
		
		_asm_puts("\nformating disk\n");

		//设置超级快的魔数
		strcpy(sfcb->magicNumber, fileSysMagicNumber);

		//设置目录块映射串
		sfcb->directoryBitmapSector = startSector;
		sfcb->directoryBitmapLength = FILESYS_FORMATE_BITMAP_SECTOR_COUNT * 512 * 8;
		sfcb->directoryCount = 0;

		//设置FCB块映射串
		startSector += FILESYS_FORMATE_BITMAP_SECTOR_COUNT;
		sfcb->fcbBitmapSector = startSector;
		sfcb->fcbBitmapLength = FILESYS_FORMATE_BITMAP_SECTOR_COUNT * 512 * 8;
		sfcb->fcbCount = 0;

		//设置inode块映射串
		startSector += FILESYS_FORMATE_BITMAP_SECTOR_COUNT;
		sfcb->inodeBitmapSector = startSector;
		sfcb->inodeBitmapLength = FILESYS_FORMATE_BITMAP_SECTOR_COUNT * 512 * 8;
		sfcb->inodeCount = 0;

		//设置数据区映射串
		startSector += FILESYS_FORMATE_BITMAP_SECTOR_COUNT;
		sfcb->dataBitmapSector = startSector;
		sfcb->dataBitmapLength = FILESYS_FORMATE_BITMAP_SECTOR_COUNT * 512 * 8;

		//设置目录块起始扇区
		startSector += FILESYS_FORMATE_BITMAP_SECTOR_COUNT;
		sfcb->directorySector = startSector;

		//设置FCB块起始扇区
		startSector += FILESYS_FORMATE_SECTOR_COUNT;
		sfcb->fcbSector = startSector;

		//设置inode块起始扇区
		startSector += FILESYS_FORMATE_SECTOR_COUNT;
		sfcb->inodeSector = startSector;

		//设置数据区起始扇区
		startSector += FILESYS_FORMATE_SECTOR_COUNT;
		sfcb->dataSector = startSector;

		//拷贝超级块到缓冲区准备写入磁盘
		for (Uint32 i = 0; i < sizeof(SFcb); i++) {
			fileSysBuffer[i] = ((Byte*)(sfcb))[i];
		}
		for (Uint32 i = sizeof(SFcb); i < 512; i++) {
			fileSysBuffer[i] = 0;
		}

		//写入超级块和映射串到磁盘
		_asm_writeDisk(fileSysBuffer, FILESYS_SFCB_SECTOR);
		writeBitMap(directoryBitmap, sfcb->directoryBitmapSector, FILESYS_FORMATE_BITMAP_SECTOR_COUNT * 512 * 8);
		writeBitMap(directoryBitmap, sfcb->fcbBitmapSector, FILESYS_FORMATE_BITMAP_SECTOR_COUNT * 512 * 8);
		writeBitMap(directoryBitmap, sfcb->inodeBitmapSector, FILESYS_FORMATE_BITMAP_SECTOR_COUNT * 512 * 8);
		writeBitMap(directoryBitmap, sfcb->dataBitmapSector, FILESYS_FORMATE_BITMAP_SECTOR_COUNT * 512 * 8);

	}
	else {
		_asm_puts("\nloading file system\n");

		//重新读取超级块
		_asm_readDisk(fileSysBuffer, FILESYS_SFCB_SECTOR);
		for (Uint32 i = 0; i < sizeof(SFcb); i++) {
			((Byte*)(sfcb))[i] = fileSysBuffer[i];
		}

		//加载映射串到内存
		readBitMap(directoryBitmap, sfcb->directoryBitmapSector, sfcb->directoryBitmapLength);
		readBitMap(fcbBitmap, sfcb->fcbBitmapSector, sfcb->fcbBitmapLength);
		readBitMap(inodeBitmap, sfcb->inodeBitmapSector, sfcb->inodeBitmapLength);
		readBitMap(dataBitmap, sfcb->dataBitmapSector, sfcb->dataBitmapLength);

		Fcb* fcb = NULL;
		startSector = sfcb->fcbSector;

		//将已存在的文件加入到映射表
		for(Uint32 i = 0; i < sfcb->fcbCount; i++) {
			_asm_readDisk(fileSysBuffer, startSector++);
			if (fcb->type == FCB_TYPE_EXIST) {
				hashTable_insert(FcbIndexTable, fcb->name, fcb->inodeIndex);
			}
		}
	}
}

void saveFileSys() {
	//将映射串写回磁盘
	writeBitMap(directoryBitmap, sfcb->directoryBitmapSector, sfcb->directoryBitmapLength);
	writeBitMap(fcbBitmap, sfcb->fcbBitmapSector, sfcb->fcbBitmapLength);
	writeBitMap(inodeBitmap, sfcb->inodeBitmapSector, sfcb->inodeBitmapLength);
	writeBitMap(dataBitmap, sfcb->dataBitmapSector, sfcb->dataBitmapLength);
	
	fileNull(fileSysBuffer, 512);
	for (Uint32 i = 0; i < sizeof(SFcb); i++) {
		fileSysBuffer[i] = ((Byte*)(sfcb))[i];
	}

	//将超级块写回磁盘
	_asm_writeDisk(fileSysBuffer, FILESYS_SFCB_SECTOR);
}

Uint32 getFreeDirectorySector() {
	Uint32 sector = 0;
	__asm__ volatile (
		"xorl %%eax,%%eax;"
		"movl %2,%%ecx;"
		"_searchDiSector:;"
		"btsl %%eax,(%1);"
		"jnc _hasDiSector;"
		"incl %%eax;"
		"loop _searchDiSector;"
		"_hasDiSector:;"
		"movl %%eax,%0;"
		:"=m"(sector)
		:"r"(dataBitmap),"m"(sfcb->directoryBitmapLength)
		:"memory","%eax","%ecx"
	);
	return sector + (sfcb->directorySector);
}

Uint32 getFreeFcbSector() {
	Uint32 sector = 0;
	__asm__ volatile (
		"xorl %%eax,%%eax;"
		"movl %2,%%ecx;"
		"_searchFSector:;"
		"btsl %%eax,(%1);"
		"jnc _hasFSector;"
		"incl %%eax;"
		"loop _searchFSector;"
		"_hasFSector:;"
		"movl %%eax,%0;"
		:"=m"(sector)
		:"r"(inodeBitmap),"m"(sfcb->fcbBitmapLength)
		:"memory","%eax","%ecx"
	);
	return sector + (sfcb->fcbSector);
}

Uint32 getFreeInodeSector() {
	Uint32 sector = 0;
	__asm__ volatile (
		"xorl %%eax,%%eax;"
		"movl %2,%%ecx;"
		"_searchInSector:;"
		"btsl %%eax,(%1);"
		"jnc _hasInSector;"
		"incl %%eax;"
		"loop _searchInSector;"
		"_hasInSector:;"
		"movl %%eax,%0;"
		:"=m"(sector)
		:"r"(inodeBitmap),"m"(sfcb->inodeBitmapLength)
		:"memory","%eax","%ecx"
	);
	return sector + (sfcb->inodeSector);
}

Uint32 getFreeDataBlock() {
	Uint32 sector = 0;
	__asm__ volatile (
		"xorl %%eax,%%eax;"
		"movl %2,%%ecx;"
		"_searchDaBlock:;"
		"btsl %%eax,(%1);"
		"jnc _hasDaBlock;"
		"incl %%eax;"
		"loop _searchDaBlock;"
		"_hasDaBlock:;"
		"movl %%eax,%0;"
		:"=m"(sector)
		:"r"(dataBitmap),"m"(sfcb->dataBitmapLength)
		:"memory","%eax","%ecx"
	);
	return sector * DISK_BLOCK_SECTOR_COUNT + (sfcb->dataSector);
}

void addDiskRequest(Byte* buffer, Uint32 type, Uint32 sector) {
	static Uint32 signal = 0;
	
	while(signal == 1);
	CLI;
	signal = 1;
	STI;
	
	Drcb drcb;
	//设置IO请求进程
	drcb.pcb = (Pcb*)(curPcbNode->data);
	drcb.buffer = buffer;
	drcb.type = type;
	drcb.sector = sector;
	heap_insert(DrcbQueue, drcb);
	
	CLI;
	signal = 0;
	STI;
}

Drcb getNextDrcb() {
	return heap_popTop(DrcbQueue);
}

void handleDiskRequest() {
	//备份内存映射表
	Uint32 oldCr3 = ((Pcb*)(curPcbNode->data))->cr3Address;
	for (Uint32 i = 0; i < DrcbQueue->size; i++) {
		//原子操作
		CLI;
		Drcb drcb = getNextDrcb();
		STI;
		
		//切换映射表
		SWITCH_CR3(drcb.pcb->cr3Address);
		
		//启动磁盘读写
		if (drcb.type == DISK_REQUSET_TYPE_READ) {
			_asm_sreadDisk(drcb.buffer, drcb.sector);
		}
		else if (drcb.type == DISK_REQUSET_TYPE_WRITE) {
			_asm_swriteDisk(drcb.buffer, drcb.sector);
		}

		//唤醒任务
		wakeUpThread(drcb.pcb, (Tcb*)(drcb.pcb->curTcbNode->data));
	}
	
	//恢复内存映射表
	SWITCH_CR3(oldCr3);
}

void loadIblock(Byte* buffer, Uint32 index) {
	addDiskRequest(buffer, DISK_REQUSET_TYPE_READ, index++);
	sleepThread(runPcb, runTcb);

	addDiskRequest(buffer + 512, DISK_REQUSET_TYPE_READ, index++);
	sleepThread(runPcb, runTcb);

	addDiskRequest(buffer + 1024, DISK_REQUSET_TYPE_READ, index++);
	sleepThread(runPcb, runTcb);

	addDiskRequest(buffer + 1024 + 512, DISK_REQUSET_TYPE_READ, index++);
	sleepThread(runPcb, runTcb);

	addDiskRequest(buffer + 2048, DISK_REQUSET_TYPE_READ, index++);
	sleepThread(runPcb, runTcb);

	addDiskRequest(buffer + 2048 + 512, DISK_REQUSET_TYPE_READ, index++);
	sleepThread(runPcb, runTcb);

	addDiskRequest(buffer + 3072, DISK_REQUSET_TYPE_READ, index++);
	sleepThread(runPcb, runTcb);

	addDiskRequest(buffer + 3072 + 512, DISK_REQUSET_TYPE_READ, index++);
	sleepThread(runPcb, runTcb);
}

void saveIblock(Byte* buffer, Uint32 index) {
	addDiskRequest(buffer, DISK_REQUSET_TYPE_WRITE, index++);
	sleepThread(runPcb, runTcb);

	addDiskRequest(buffer + 512, DISK_REQUSET_TYPE_WRITE, index++);
	sleepThread(runPcb, runTcb);

	addDiskRequest(buffer + 1024, DISK_REQUSET_TYPE_WRITE, index++);
	sleepThread(runPcb, runTcb);

	addDiskRequest(buffer + 1024 + 512, DISK_REQUSET_TYPE_WRITE, index++);
	sleepThread(runPcb, runTcb);

	addDiskRequest(buffer + 2048, DISK_REQUSET_TYPE_WRITE, index++);
	sleepThread(runPcb, runTcb);

	addDiskRequest(buffer + 2048 + 512, DISK_REQUSET_TYPE_WRITE, index++);
	sleepThread(runPcb, runTcb);

	addDiskRequest(buffer + 3072, DISK_REQUSET_TYPE_WRITE, index++);
	sleepThread(runPcb, runTcb);

	addDiskRequest(buffer + 3072 + 512, DISK_REQUSET_TYPE_WRITE, index++);
	sleepThread(runPcb, runTcb);
}

void readIblock(Byte* buffer, Iblock* iblock, Uint32 start, Uint32 size) {
	static Byte* _buffer = NULL;
	if (_buffer == NULL) {
		_buffer = (Byte*)mallocKernel(1024);
	}

	//计算索引号
	Uint32 blockIndex = start / 512;
	Uint32 sector = iblock->sector[blockIndex++];
	//计算起始偏移
	Uint32 offset = start % 512;
	if (512 - start > size) {
		_asm_puts("\nreadIblock overbound\n");
		HLT;
	}
	else {
		Uint32 k = 0;
		addDiskRequest(_buffer, DISK_REQUSET_TYPE_READ, sector);
		sleepThread(runPcb, runTcb);
		for (Uint32 i = 0; (i < 512 - offset) && (size > 0) ; i++,size--) {
			buffer[k++] = _buffer[offset + i];
		}
		if (size != 0) {
			for (Uint32 i = 1; i < iblock->size; i++) {
				sector = iblock->sector[blockIndex++];
				addDiskRequest(_buffer, DISK_REQUSET_TYPE_READ, sector);
				sleepThread(runPcb, runTcb);
				for (Uint32 j = 0; (j < 512) && (size > 0) ; j++,size--) {
					buffer[k++] = _buffer[j];
				}
			}
		}
	}
}

void writeIblock(Byte* buffer, Iblock* iblock, Uint32 start, Uint32 size) {
	static Byte* _buffer = NULL;
	if (_buffer == NULL) {
		_buffer = (Byte*)mallocKernel(1024);
	}

	//计算索引号
	Uint32 blockIndex = start / 512;
	Uint32 sector = iblock->sector[blockIndex++];
	//计算起始偏移
	Uint32 offset = start % 512;
	addDiskRequest(_buffer, DISK_REQUSET_TYPE_READ, sector);
	sleepThread(runPcb, runTcb);
	Uint32 k = 0;
	for (Uint32 i = 0; (i < 512 - offset) && (size > 0); i++,size--) {
		_buffer[offset + i] = buffer[k++];
	}
	addDiskRequest(_buffer, DISK_REQUSET_TYPE_WRITE, sector);
	sleepThread(runPcb, runTcb);
	if (size != 0) {
		for (Uint32 i = 1; i < iblock->size; i++) {
			sector = iblock->sector[blockIndex++];
			addDiskRequest(_buffer, DISK_REQUSET_TYPE_READ, sector);
			sleepThread(runPcb, runTcb);
			for (Uint32 j = 0; (j < 512) && (size > 0) ; j++,size--) {
				_buffer[i] = buffer[k++];
			}
			addDiskRequest(_buffer, DISK_REQUSET_TYPE_WRITE, sector);
			sleepThread(runPcb, runTcb);
		}
	}
}

void readIblockFirst(Byte* buffer, Iblock* iblockFirst, Uint32 blockIndex, Uint32 offset, Uint32 size) {
	static Byte* _buffer = NULL;
	if (_buffer == NULL) {
		_buffer = (Byte*)mallocKernel(4096);
	}

	//加载直接
	loadIblock(_buffer, iblockFirst->sector[blockIndex]);
	Iblock iblock = *((Iblock*)(_buffer));
	readIblock(buffer, &iblock, offset, DISK_BOLOK_BYTE_SIZE - offset);
	//这里可能需要移动buffer指针

	Uint32 j = 1;

	for (Uint32 i = blockIndex + 1; i < iblock.size; i++, j++) {
		//加载直接索引
		loadIblock(_buffer, iblockFirst->sector[i]);
		iblock = *((Iblock*)(_buffer));
		if (size >= 4096) {
			readIblock(buffer + j * DISK_BOLOK_BYTE_SIZE, &iblock, 0, DISK_BOLOK_BYTE_SIZE);
			size -= 4096;
		}
		else if (size < 4096 && size !=0) {
			readIblock(buffer + j * DISK_BOLOK_BYTE_SIZE, &iblock, 0, size);
			size = 0;
		}
	}
}

void writeIblockFirst(Byte* buffer, Iblock* iblockFirst, Uint32 blockIndex, Uint32 offset, Uint32 size) {
	static Byte* _buffer = NULL;
	if (_buffer == NULL) {
		_buffer = (Byte*)mallocKernel(4096);
	}

	//加载直接索引
	loadIblock(_buffer, iblockFirst->sector[blockIndex]);
	Iblock iblock = *((Iblock*)(_buffer));
	writeIblock(buffer, &iblock, offset, DISK_BOLOK_BYTE_SIZE - offset);
	//这里可能需要移动buffer指针

	Uint32 j = 1;

	for (Uint32 i = blockIndex + 1; i < iblock.size; i++, j++) {
		//加载直接索引
		loadIblock(_buffer, iblockFirst->sector[i]);
		iblock = *((Iblock*)(_buffer));
		if (size >= 4096) {
			writeIblock(buffer + j * DISK_BOLOK_BYTE_SIZE, &iblock, 0, DISK_BOLOK_BYTE_SIZE);
			size -= 4096;
		}
		else if (size < 4096 && size !=0) {
			writeIblock(buffer + j * DISK_BOLOK_BYTE_SIZE, &iblock, 0, size);
			size = 0;
		}
	}
}

void readIblockSecond(Byte* buffer, Iblock* iblockSecond, Uint32 blockIndex, Uint32 offset, Uint32 size) {
	static Byte* _buffer = NULL;
	if (_buffer == NULL) {
		_buffer = (Byte*)mallocKernel(4096);
	}

	//加载第一级索引
	loadIblock(_buffer, iblockSecond->sector[blockIndex]);
	Iblock iblock = *((Iblock*)(_buffer));
	readIblockFirst(buffer, &iblock, blockIndex, offset, IBLOCK_INDEX_FIRST_BYTE_SIZE - offset);
	//这里可能需要移动buffer指针

	for (Uint32 i = blockIndex + 1; i < iblockSecond->size; i++) {
		//加载第一级索引
		loadIblock(_buffer, iblockSecond->sector[i]);
		Iblock iblock = *((Iblock*)(_buffer));

		if (size >= IBLOCK_INDEX_FIRST_BYTE_SIZE) {
			readIblockFirst(buffer + i * IBLOCK_INDEX_FIRST_BYTE_SIZE, &iblock, 0, 0, IBLOCK_INDEX_FIRST_BYTE_SIZE);
			size -= IBLOCK_INDEX_FIRST_BYTE_SIZE;
		}
		else if (size < IBLOCK_INDEX_FIRST_BYTE_SIZE && size != 0) {
			readIblockFirst(buffer + i * IBLOCK_INDEX_FIRST_BYTE_SIZE, &iblock, 0, 0, size);
			size = 0;
		}
	}
}

void writeIblockSecond(Byte* buffer, Iblock* iblockSecond, Uint32 blockIndex, Uint32 offset, Uint32 size) {
	static Byte* _buffer = NULL;
	if (_buffer == NULL) {
		_buffer = (Byte*)mallocKernel(4096);
	}

	//加载第一级索引
	loadIblock(_buffer, iblockSecond->sector[blockIndex]);
	Iblock iblock = *((Iblock*)(_buffer));
	writeIblockFirst(buffer, &iblock, blockIndex, offset, IBLOCK_INDEX_FIRST_BYTE_SIZE - offset);
	//这里可能需要移动buffer指针

	for (Uint32 i = blockIndex + 1; i < iblockSecond->size; i++) {
		//加载第一级索引
		loadIblock(_buffer, iblockSecond->sector[i]);
		Iblock iblock = *((Iblock*)(_buffer));

		if (size >= IBLOCK_INDEX_FIRST_BYTE_SIZE) {
			writeIblockFirst(buffer + i * IBLOCK_INDEX_FIRST_BYTE_SIZE, &iblock, 0, 0, IBLOCK_INDEX_FIRST_BYTE_SIZE);
			size -= IBLOCK_INDEX_FIRST_BYTE_SIZE;
		}
		else if (size < IBLOCK_INDEX_FIRST_BYTE_SIZE && size != 0) {
			writeIblockFirst(buffer + i * IBLOCK_INDEX_FIRST_BYTE_SIZE, &iblock, 0, 0, size);
			size = 0;
		}
	}
}

void readIblockThird(Byte* buffer, Iblock* iblockThird, Uint32 blockIndex, Uint32 offset, Uint32 size) {
	static Byte* _buffer = NULL;
	if (_buffer == NULL) {
		_buffer = (Byte*)mallocKernel(4096);
	}

	//加载第二级索引
	loadIblock(_buffer, iblockThird->sector[blockIndex]);
	Iblock iblock = *((Iblock*)(_buffer));
	readIblockSecond(buffer, &iblock, blockIndex, offset, IBLOCK_INDEX_SECOND_BYTE_SIZE - offset);
	//这里可能需要移动buffer指针

	for (Uint32 i = blockIndex + 1; i < iblockThird->size; i++) {
		//加载第二级索引
		loadIblock(_buffer, iblockThird->sector[i]);
		Iblock iblock = *((Iblock*)(_buffer));

		if (size >= IBLOCK_INDEX_SECOND_BYTE_SIZE) {
			readIblockSecond(buffer + i * IBLOCK_INDEX_SECOND_BYTE_SIZE, &iblock, 0, 0, IBLOCK_INDEX_SECOND_BYTE_SIZE);
			size -= IBLOCK_INDEX_SECOND_BYTE_SIZE;
		}
		else if (size < IBLOCK_INDEX_SECOND_BYTE_SIZE && size != 0) {
			readIblockSecond(buffer + i * IBLOCK_INDEX_SECOND_BYTE_SIZE, &iblock, 0, 0, size);
			size = 0;
		}
	}
}

void writeIblockThird(Byte* buffer, Iblock* iblockThird, Uint32 blockIndex, Uint32 offset, Uint32 size) {
	static Byte* _buffer = NULL;
	if (_buffer == NULL) {
		_buffer = (Byte*)mallocKernel(4096);
	}


	//加载第二级索引
	loadIblock(_buffer, iblockThird->sector[blockIndex]);
	Iblock iblock = *((Iblock*)(_buffer));
	writeIblockSecond(buffer, &iblock, blockIndex, offset, IBLOCK_INDEX_SECOND_BYTE_SIZE - offset);
	//这里可能需要移动buffer指针

	for (Uint32 i = blockIndex + 1; i < iblockThird->size; i++) {
		//加载第二级索引
		loadIblock(_buffer, iblockThird->sector[0]);
		Iblock iblock = *((Iblock*)(_buffer));

		if (size >= IBLOCK_INDEX_SECOND_BYTE_SIZE) {
			writeIblockSecond(buffer + i * DISK_BOLOK_BYTE_SIZE, &iblock, 0, 0, IBLOCK_INDEX_SECOND_BYTE_SIZE);
			size -= IBLOCK_INDEX_SECOND_BYTE_SIZE;
		}
		else if (size < IBLOCK_INDEX_SECOND_BYTE_SIZE && size != 0) {
			writeIblockSecond(buffer + i * DISK_BOLOK_BYTE_SIZE, &iblock, 0, 0, size);
			size = 0;
		}
	}
}

void touchFile(Byte* name) {
	static Byte* _buffer = NULL;
	if (_buffer == NULL) {
		_buffer = (Byte*)mallocKernel(10000);
	}


	Fcb* fcb = (Fcb*)mallocKernel(sizeof(Fcb));
	//寻找空闲的FCB块
	Uint32 fcbSector = getFreeFcbSector();
	strcpy(fcb->name, name);
	fcb->byteSize = 0;
	fcb->state = FCB_STATE_CLOSE;
	fcb->count = 0;
	fcb->inodeIndex = getFreeInodeSector();
	fcb->type = FCB_TYPE_EXIST;
	fcb->allocByteSize = 4096;
	fileNull(_buffer, 512);
	Inode inode = (*(Inode*)_buffer);
	inode.iblockIndex = getFreeDataBlock();
	Iblock iblock;
	iblock.size = 1;
	iblock.sector[iblock.size - 1] = getFreeDataBlock();

	//写FCB到磁盘
	memcpy(_buffer, fcb, 512);
	_asm_swriteDisk(_buffer, fcbSector);

	//写inode到磁盘
	fileNull(_buffer, 512);
	memcpy(_buffer, &inode, 512);
	_asm_swriteDisk(_buffer, fcb->inodeIndex);

	//写索引块到磁盘
	saveIblock((Byte*)(&iblock), inode.iblockIndex);


	//添加到映射表
	hashTable_insert(FcbIndexTable, name, fcbSector);

	//更新超级块信息
	sfcb->fcbCount += 1;
}

void eraseFile(Fcb* fcb) {
	fcb->state = FCB_TYPE_REMOVE;
	hashTable_erase(FcbIndexTable, fcb->name);
}

void writeFile(Fcb* fcb, Byte* buffer, Uint32 logicStart, Uint32 byteSize) {
	static Byte* _buffer = NULL;
	if (_buffer == NULL) {
		_buffer = (Byte*)mallocKernel(4096);
	}

	//计算逻辑起始盘块号
	Uint32 blockIndex = logicStart / DISK_BOLOK_BYTE_SIZE;
	//计算起始偏移
	Uint32 offset = logicStart % DISK_BOLOK_BYTE_SIZE;

	//加载inode
	fileNull(_buffer, 512);
	addDiskRequest(_buffer, DISK_REQUSET_TYPE_READ, fcb->inodeIndex);
	sleepThread(runPcb, runTcb);
	Inode inode = *((Inode*)(_buffer));

	//加载索引
	fileNull(_buffer, 4096);
	addDiskRequest(_buffer, DISK_REQUSET_TYPE_READ, inode.iblockIndex);
	sleepThread(runPcb, runTcb);
	Iblock iblock = *((Iblock*)(_buffer));

	if (iblock.type == IBLOCK_TYPE_IBLOCK) {
		writeIblock(buffer, &iblock, offset, DISK_BOLOK_BYTE_SIZE - offset);
	}
	else if (iblock.type == IBLOCK_TYPE_INDEX_FIRST) {
		writeIblockFirst(buffer, &iblock, blockIndex % (DISK_BLOCK_SECTOR_COUNT), offset, byteSize);
	}
	else if (iblock.type == IBLOCK_TYPE_INDEX_SECOND) {
		writeIblockSecond(buffer, &iblock, blockIndex % (DISK_BLOCK_SECTOR_COUNT * DISK_BLOCK_SECTOR_COUNT), offset, byteSize);

	}
	else if (iblock.type == IBLOCK_TYPE_INDEX_THIRD) {
		writeIblockThird(buffer, &iblock, 
						blockIndex % (DISK_BLOCK_SECTOR_COUNT * DISK_BLOCK_SECTOR_COUNT * DISK_BLOCK_SECTOR_COUNT), 
						offset, byteSize);
	}

}

void readFile(Fcb* fcb, Byte* buffer, Uint32 logicStart, Uint32 byteSize) {
	static Byte* _buffer = NULL;
	if (_buffer == NULL) {
		_buffer = (Byte*)mallocKernel(4096);
	}

	//计算逻辑起始盘块号
	Uint32 blockIndex = logicStart / DISK_BOLOK_BYTE_SIZE;
	//计算起始偏移
	Uint32 offset = logicStart % DISK_BOLOK_BYTE_SIZE;

	//加载inode
	fileNull(_buffer, 512);
	addDiskRequest(_buffer, DISK_REQUSET_TYPE_READ, fcb->inodeIndex);
	sleepThread(runPcb, runTcb);
	Inode inode = *((Inode*)(_buffer));

	//加载索引
	fileNull(_buffer, 4096);
	loadIblock(_buffer, inode.iblockIndex);
	Iblock iblock = *((Iblock*)(_buffer));

	if (iblock.type == IBLOCK_TYPE_IBLOCK) {
		readIblock(buffer, &iblock, offset, DISK_BOLOK_BYTE_SIZE - offset);
	}
	else if (iblock.type == IBLOCK_TYPE_INDEX_FIRST) {
		readIblockFirst(buffer, &iblock, blockIndex % DISK_BLOCK_SECTOR_COUNT, offset, byteSize);
	}
	else if (iblock.type == IBLOCK_TYPE_INDEX_SECOND) {
		readIblockSecond(buffer, &iblock, blockIndex % (DISK_BLOCK_SECTOR_COUNT * DISK_BLOCK_SECTOR_COUNT), offset, byteSize);

	}
	else if (iblock.type == IBLOCK_TYPE_INDEX_THIRD) {
		readIblockThird(buffer, &iblock, 
						blockIndex % (DISK_BLOCK_SECTOR_COUNT * DISK_BLOCK_SECTOR_COUNT * DISK_BLOCK_SECTOR_COUNT), 
						offset, byteSize);
	}

}

void extendFile(Fcb* fcb, Uint32 byteSize) {
	static Byte* _buffer = NULL;
	if (_buffer == NULL) {
		_buffer = (Byte*)mallocKernel(4096);
	}

	//要扩充的空间对齐
	byteSize = byteSize / DISK_BOLOK_BYTE_SIZE * DISK_BOLOK_BYTE_SIZE + DISK_BOLOK_BYTE_SIZE;
	fcb->byteSize += byteSize;

	Uint32 allocTimes = byteSize / DISK_BOLOK_BYTE_SIZE;

	//加载inode
	fileNull(_buffer, 4096);
	addDiskRequest(_buffer, DISK_REQUSET_TYPE_READ, fcb->inodeIndex);
	sleepThread(runPcb, runTcb);

	Inode inode = *((Inode*)(_buffer));

	//加载索引块
	loadIblock(_buffer, inode.iblockIndex);

	Iblock iblock = *((Iblock*)(_buffer));

	Uint32 oldIndex = NULL;

	//如果是直接索引
	if (iblock.type == IBLOCK_TYPE_IBLOCK) {
		//扩充直接索引
		allocTimes = extendIblock(&iblock, inode.iblockIndex, allocTimes);
		
		//如果需要继续扩充
		if (allocTimes != 0) {
			oldIndex = inode.iblockIndex;
			inode.iblockIndex = getFreeDataBlock();

			iblock.type = IBLOCK_TYPE_INDEX_FIRST;
			iblock.sector[0] = oldIndex;
			iblock.size = 1;
			
			//扩充一级索引
			allocTimes = extendIblockFirst(&iblock, inode.iblockIndex, allocTimes);
		}

		//如果需要继续扩充
		if (allocTimes != 0) {
			oldIndex = inode.iblockIndex;
			inode.iblockIndex = getFreeDataBlock();

			iblock.type = IBLOCK_TYPE_INDEX_SECOND;
			iblock.sector[0] = oldIndex;
			iblock.size = 1;

			//扩充二级索引
			allocTimes = extendIblockSecond(&iblock, inode.iblockIndex, allocTimes);
		}

		//如果需要继续扩充
		if (allocTimes != 0) {
			oldIndex = inode.iblockIndex;
			inode.iblockIndex = getFreeDataBlock();

			iblock.type = IBLOCK_TYPE_INDEX_THIRD;
			iblock.sector[0] = oldIndex;
			iblock.size = 1;

			//扩充三级索引
			allocTimes = extendIblockThird(&iblock, inode.iblockIndex, allocTimes);
		}
	}
	else if (iblock.type == IBLOCK_TYPE_INDEX_FIRST) {
		allocTimes = extendIblockFirst(&iblock, inode.iblockIndex, allocTimes);
		if (allocTimes != 0) {
			oldIndex = inode.iblockIndex;
			inode.iblockIndex = getFreeDataBlock();

			iblock.type = IBLOCK_TYPE_INDEX_SECOND;
			iblock.sector[0] = oldIndex;
			iblock.size = 1;

			allocTimes = extendIblockSecond(&iblock, inode.iblockIndex, allocTimes);
		}
		if (allocTimes != 0) {
			oldIndex = inode.iblockIndex;
			inode.iblockIndex = getFreeDataBlock();

			iblock.type = IBLOCK_TYPE_INDEX_THIRD;
			iblock.sector[0] = oldIndex;
			iblock.size = 1;

			allocTimes = extendIblockThird(&iblock, inode.iblockIndex, allocTimes);
		}
	}
	else if (iblock.type = IBLOCK_TYPE_INDEX_SECOND) {
		allocTimes = extendIblockSecond(&iblock, inode.iblockIndex, allocTimes);
		if (allocTimes != 0) {
			oldIndex = inode.iblockIndex;
			inode.iblockIndex = getFreeDataBlock();

			iblock.type = IBLOCK_TYPE_INDEX_THIRD;
			iblock.sector[0] = oldIndex;
			iblock.size = 1;

			allocTimes = extendIblockThird(&iblock, inode.iblockIndex, allocTimes);
		}
	}
	else if (iblock.type == IBLOCK_TYPE_INDEX_THIRD) {
		allocTimes = extendIblockThird(&iblock, inode.iblockIndex, allocTimes);
		if (allocTimes != 0) {
			_asm_puts("\nFile overflow\n");
			HLT;
		}
	}


}

Uint32 extendIblock(Iblock* iblock, Uint32 saveIndex,Uint32 times) {
	for (; iblock->size < IBLOCK_INCLUDE_BLOCK_COUNT && times > 0; (iblock->size)++) {
		iblock->sector[iblock->size] = getFreeDataBlock();
		times--;
	}
	saveIblock((Byte*)iblock, saveIndex);
	return times;
}

Uint32 extendIblockFirst(Iblock* iblockFirst, Uint32 saveIndex,Uint32 times) {
	static Byte* _buffer = NULL;
	if (_buffer == NULL) {
		_buffer = (Byte*)mallocKernel(IBLOCK_SECTOR_BYTE_SIZE);
	}

	Iblock iblock;

	loadIblock((Byte*)_buffer, iblockFirst->sector[iblockFirst->size - 1]);
	iblock = *((Iblock*)(_buffer));
	if (iblock.size < IBLOCK_INCLUDE_BLOCK_COUNT) {
		times = extendIblock(&iblock, iblockFirst->sector[iblockFirst->size - 1], times);
	}

	fileNull(_buffer, IBLOCK_SECTOR_BYTE_SIZE);
	iblock = *((Iblock*)(_buffer));

	for (; iblockFirst->size < IBLOCK_INCLUDE_BLOCK_COUNT && times > 0; (iblockFirst->size)++) {
		iblockFirst->sector[iblockFirst->size] = getFreeDataBlock();
		iblock.size = 0;
		iblock.type = IBLOCK_TYPE_IBLOCK;
		times = extendIblock(&iblock, iblockFirst->sector[iblockFirst->size], times);
	}
	saveIblock((Byte*)iblockFirst, saveIndex);
	return times;
}

Uint32 extendIblockSecond(Iblock* iblockSecond, Uint32 saveIndex,Uint32 times) {
	static Byte* _buffer = NULL;
	if (_buffer == NULL) {
		_buffer = (Byte*)mallocKernel(IBLOCK_SECTOR_BYTE_SIZE);
	}

	Iblock iblockFirst;
	
	loadIblock((Byte*)_buffer, iblockSecond->sector[iblockSecond->size - 1]);
	iblockFirst = *((Iblock*)(_buffer));
	if (iblockFirst.size < IBLOCK_INCLUDE_BLOCK_COUNT) {
		times = extendIblock(&iblockFirst, iblockSecond->sector[iblockSecond->size - 1], times);
	}

	fileNull(_buffer, IBLOCK_SECTOR_BYTE_SIZE);
	iblockFirst = *((Iblock*)(_buffer));

	for (; iblockSecond->size < IBLOCK_INCLUDE_BLOCK_COUNT && times > 0; (iblockSecond->size)++) {
		iblockSecond->sector[iblockSecond->size] = getFreeDataBlock();
		iblockFirst.size = 0;
		iblockFirst.type = IBLOCK_TYPE_INDEX_FIRST;
		times = extendIblock(&iblockFirst, iblockSecond->sector[iblockSecond->size], times);
	}
	saveIblock((Byte*)iblockSecond, saveIndex);
	return times;
}

Uint32 extendIblockThird(Iblock* iblockThird, Uint32 saveIndex,Uint32 times) {
	static Byte* _buffer = NULL;
	if (_buffer == NULL) {
		_buffer = (Byte*)mallocKernel(IBLOCK_SECTOR_BYTE_SIZE);
	}

	Iblock iblockSecond;
	
	loadIblock((Byte*)_buffer, iblockThird->sector[iblockThird->size - 1]);
	iblockSecond = *((Iblock*)(_buffer));
	if (iblockSecond.size < IBLOCK_INCLUDE_BLOCK_COUNT) {
		times = extendIblock(&iblockSecond, iblockThird->sector[iblockThird->size - 1], times);
	}

	fileNull(_buffer, IBLOCK_SECTOR_BYTE_SIZE);
	iblockSecond = *((Iblock*)(_buffer));

	for (; iblockThird->size < IBLOCK_INCLUDE_BLOCK_COUNT && times > 0; (iblockThird->size)++) {
		iblockThird->sector[iblockThird->size] = getFreeDataBlock();
		iblockSecond.size = 0;
		iblockSecond.type = IBLOCK_TYPE_INDEX_SECOND;
		times = extendIblock(&iblockSecond, iblockThird->sector[iblockThird->size], times);
	}
	saveIblock((Byte*)iblockThird, saveIndex);
	return times;
}












#endif