#ifndef FINAL_H

#define FINAL_H

#define NULL 0
//内核加载起始物理地址
#define MEM_KERNEL_START_ADDRESS 0x10000

//内核加载起始扇区
#define DISK_KERNEL_START_SECTOR 0x1

//4GB ring0 代码段GDT选择子
#define MEM_GDT_4GB_SEL 0x0008

//4GB ring0 数据段GDT选择子
#define CODE_GDT_4GB_SEL 0x0010

//显存GDT选择子
#define MEM_GDT_VIDEO_SEL 0x0018
#define MEM_GDT_STACK_SEL 0x0020

//显存线性地址
#define MEM_VIDEO_ADRESS 0x800B8000

//ring0调用门描述符属性
#define ATTRI_RING0_CALL_GATE 0x8C00
#define ATTRI_RING3_CALL_GATE 0xEC00

//ring0 中断门描述符属性
#define ATTRI_RING0_INT_GATE 0x8E00
#define ATTRI_RING3_INT_GATE 0xEE00

//ring0 陷阱门描述符属性
#define ATTRI_RING0_TRAP_GATE 0x8F00
#define ATTRI_RING3_TRAP_GATE 0x8500

//ring3 GDT代码段描述符属性
#define ATTRI_GDT_RING3_CODE 0x00C0F800

//ring3 GDT数据段描述符属性
#define ATTRI_GDT_RING3_DATA 0x00C0F200
#define ATTRI_LDT_RING3_CODE 0x00C0F800
#define ATTRI_LDT_RING3_DATA 0x00C0F200

//TSS GDT描述符属性
#define ATTRI_TSS 0x0040e900

//文件系统超级块起始扇区
#define FILESYS_SFCB_SECTOR 1000

//文件系统格式化起始扇区
#define FILESYS_FORMATE_START_SECTOR 2000

//文件系统格式化时默认的位图所占扇区数
#define FILESYS_FORMATE_BITMAP_SECTOR_COUNT 100

//文件系统格式化时除数据区外其他区域所占扇区数
#define FILESYS_FORMATE_SECTOR_COUNT 5000

//进程状态
#define PCB_STATE_RUN 0
#define PCB_STATE_READY 1
#define PCB_STATE_WAIT 2

//线程状态
#define TCB_STATE_RUN 0
#define TCB_STATE_READY 1
#define TCB_STATE_WAIT 2

//文件状态
#define FCB_STATE_CLOSE 0
#define FCB_STATE_OPEN 1


#define FCB_TYPE_EXIST 0xf
#define FCB_TYPE_REMOVE 0xff

//索引块类型
#define IBLOCK_TYPE_NULL 0
#define IBLOCK_TYPE_IBLOCK 1
#define IBLOCK_TYPE_INDEX_FIRST 2
#define IBLOCK_TYPE_INDEX_SECOND 3
#define IBLOCK_TYPE_INDEX_THIRD 4

//索引块所占扇区数
#define IBLOCK_SECTOR_COUNT 8

//索引块所占扇区大小
#define IBLOCK_SECTOR_BYTE_SIZE 4096

//磁盘IO请求类型
#define DISK_REQUSET_TYPE_READ 0
#define DISK_REQUSET_TYPE_WRITE 1

//盘块所占扇区数
#define DISK_BLOCK_SECTOR_COUNT 4

//盘块大小
#define DISK_BOLOK_BYTE_SIZE 4096

#define IBLOCK_INCLUDE_BLOCK_COUNT 126 + 128 * (DISK_BLOCK_SECTOR_COUNT * 2 - 1)

#define IBLOCK_INDEX_ZERO_BYTE_SIZE IBLOCK_INCLUDE_BLOCK_COUNT * DISK_BOLOK_BYTE_SIZE

#define IBLOCK_INDEX_FIRST_BYTE_SIZE IBLOCK_INCLUDE_BLOCK_COUNT * IBLOCK_INDEX_ZERO_BYTE_SIZE

#define IBLOCK_INDEX_SECOND_BYTE_SIZE IBLOCK_INCLUDE_BLOCK_COUNT * IBLOCK_INDEX_FIRST_BYTE_SIZE

#define IBLOCK_INDEX_THIRD_BYTE_SIZE IBLOCK_INCLUDE_BLOCK_COUNT * IBLOCK_INDEX_SECOND_BYTE_SIZE

#define HASH_TABLE_SIZE 4111


//停机
#define HLT __asm__ volatile ("hlt;")

//关中断
#define CLI __asm__ volatile ("cli;")

//开中断
#define STI __asm__ volatile ("sti;")

//执行任务切换
#define SWITCH_TASK __asm__ volatile ("int $0x70;")

//切换CR3（内存映射表）（页目录 页表）
#define SWITCH_CR3(CR3) \
	__asm__ volatile (\
		"movl %0,%%cr3;"\
		:\
		:"r"(CR3)\
		:\
	)

//在栈中压入执行进程切换所需的寄存器等信息
#define PUSH_SREG(cs, ds, es, fs, gs, ss0, ss, esp0, esp, eip) \
	__asm__ volatile (\
		"movl %%esp,%%eax;"\
		"movl %%ss,%%ebx;"\
		"movl %7,%%esp;"\
		"movl %5,%%ecx;"\
		"movl %%ecx,%%ss;"\
		"pushl %6;"\
		"pushl %8;"\
		"pushl $0x200;"\
		"pushl %0;"\
		"pushl %9;"\
		"pushl %1;"\
		"pushl %2;"\
		"pushl %3;"\
		"pushl %4;"\
		"pushl %%eax;"\
		"pushl %%ebx;"\
		"pushl %%ecx;"\
		"pushl %%edx;"\
		"pushl %%ebp;"\
		"pushl %%esi;"\
		"pushl %%edi;"\
		"movl %%esp,%7;"\
		"movl %%ebx,%%ss;"\
		"movl %%eax,%%esp;"\
		:\
		:"m"(cs),"m"(ds),"g"(es),"g"(fs),"g"(gs),"g"(ss0),"g"(ss),"m"(esp0),"g"(esp),"g"(eip)\
		:"%eax","%ebx","%ecx","memory"\
	)






#define KEYBOARD_MAKECODE_ESC 0x01
#define KEYBOARD_MAKECODE_F1 0x3b
#define KEYBOARD_MAKECODE_F2 0x3c
#define KEYBOARD_MAKECODE_F3 0x3d
#define KEYBOARD_MAKECODE_F4 0x3e
#define KEYBOARD_MAKECODE_F5 0x3f
#define KEYBOARD_MAKECODE_F6 0x40
#define KEYBOARD_MAKECODE_F7 0x41
#define KEYBOARD_MAKECODE_F8 0x42
#define KEYBOARD_MAKECODE_F9 0x43
#define KEYBOARD_MAKECODE_F10 0x44
#define KEYBOARD_MAKECODE_F11 0x57
#define KEYBOARD_MAKECODE_F12 0x58
#define KEYBOARD_MAKECODE_U1 0x02
#define KEYBOARD_MAKECODE_U2 0x03
#define KEYBOARD_MAKECODE_U3 0x04
#define KEYBOARD_MAKECODE_U4 0x05
#define KEYBOARD_MAKECODE_U5 0x06
#define KEYBOARD_MAKECODE_U6 0x07
#define KEYBOARD_MAKECODE_U7 0x08
#define KEYBOARD_MAKECODE_U8 0x09
#define KEYBOARD_MAKECODE_U9 0x0a
#define KEYBOARD_MAKECODE_U0 0x0b
#define KEYBOARD_MAKECODE_U_MINUS 0x0c
#define KEYBOARD_MAKECODE_U_PLUS_EQUAL 0x0d
#define KEYBOARD_MAKECODE_BACKSPACE 0x0e
#define KEYBOARD_MAKECODE_TAB 0x0f
#define KEYBOARD_MAKECODE_Q 0x10
#define KEYBOARD_MAKECODE_W 0x11
#define KEYBOARD_MAKECODE_E 0x12
#define KEYBOARD_MAKECODE_R 0x13
#define KEYBOARD_MAKECODE_T 0x14
#define KEYBOARD_MAKECODE_Y 0x15
#define KEYBOARD_MAKECODE_U 0x16
#define KEYBOARD_MAKECODE_I 0x17
#define KEYBOARD_MAKECODE_O 0x18
#define KEYBOARD_MAKECODE_P 0x19
#define KEYBOARD_MAKECODE_LEFT_BRACKET 0x1a
#define KEYBOARD_MAKECODE_RIGHT_BRACKET 0x1b
#define KEYBOARD_MAKECODE_CAPSLOCK 0x3a
#define KEYBOARD_MAKECODE_A 0x1e
#define KEYBOARD_MAKECODE_S 0x1f
#define KEYBOARD_MAKECODE_D 0x20
#define KEYBOARD_MAKECODE_F 0x21
#define KEYBOARD_MAKECODE_G 0x22
#define KEYBOARD_MAKECODE_H 0x23
#define KEYBOARD_MAKECODE_J 0x24
#define KEYBOARD_MAKECODE_K 0x25
#define KEYBOARD_MAKECODE_L 0x26
#define KEYBOARD_MAKECODE_COLON 0x27
#define KEYBOARD_MAKECODE_QUOTATION 0x28
#define KEYBOARD_MAKECODE_ENTER 0x1c
#define KEYBOARD_MAKECODE_L_SHIFT 0x2a
#define KEYBOARD_MAKECODE_Z 0x2c
#define KEYBOARD_MAKECODE_X 0x2d
#define KEYBOARD_MAKECODE_C 0x2e
#define KEYBOARD_MAKECODE_V 0x2f
#define KEYBOARD_MAKECODE_B 0x30
#define KEYBOARD_MAKECODE_N 0x31
#define KEYBOARD_MAKECODE_M 0x32
#define KEYBOARD_MAKECODE_COMMA 0x33
#define KEYBOARD_MAKECODE_DOT 0x34
#define KEYBOARD_MAKECODE_BACKSLANT 0x35
#define KEYBOARD_MAKECODE_R_SHIFT 0x36
#define KEYBOARD_MAKECODE_L_CTRL 0x1d
#define KEYBOARD_MAKECODE_L_ALT 0x38
#define KEYBOARD_MAKECODE_SPACE 0x39
#define KEYBOARD_MAKECODE_R_ALT 0x38
#define KEYBOARD_MAKECODE_R_CTRL 0x1D
#define KEYBOARD_MAKECODE_SCROLL_LOCK 0x46
#define KEYBOARD_MAKECODE_N0 0x52
#define KEYBOARD_MAKECODE_N1 0x4f
#define KEYBOARD_MAKECODE_N2 0x50
#define KEYBOARD_MAKECODE_N3 0x51
#define KEYBOARD_MAKECODE_N4 0x4b
#define KEYBOARD_MAKECODE_N5 0x4c
#define KEYBOARD_MAKECODE_N6 0x4d
#define KEYBOARD_MAKECODE_N7 0x47
#define KEYBOARD_MAKECODE_N8 0x48
#define KEYBOARD_MAKECODE_N9 0x49
#define KEYBOARD_MAKECODE_PLUS 0x4e


#define FONT_COLOUR_GREEN 0x2
#define FONT_COLOUR_BLUE 0X1
#define FONT_COLOUR_CHING 0x3
#define FONT_COLOUR_RED 0x4
#define FONT_COLOUR_BROWN 0x6


#endif