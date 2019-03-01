
	exception_msg 			db 0x0d,'*******************Exception*******************',0x0d,0
	
	int_0x000_msg 			db 0x0d,'*******************int 0x000 div by zero*******************',0x0d,0
	
	int_0x001_msg 			db 0x0d,'*******************int 0x001*******************',0x0d,0
	
	int_0x002_msg 			db 0x0d,'*******************int 0x002 NMI*******************',0x0d,0
	
	int_0x003_msg 			db 0x0d,'*******************int 0x003 breakPoint*******************',0x0d,0
	
	int_0x004_msg 			db 0x0d,'*******************int 0x004 overflow*******************',0x0d,0
	
	int_0x005_msg 			db 0x0d,'*******************int 0x005 out of bound*******************',0x0d,0
	
	int_0x006_msg 			db 0x0d,'*******************int 0x006 undefine operand number*******************',0x0d,0
	
	int_0x007_msg 			db 0x0d,'*******************int 0x007 device can not use*******************',0x0d,0
	
	int_0x008_msg 			db 0x0d,'*******************int 0x008 double fault*******************',0x0d,0
	
	int_0x009_msg 			db 0x0d,'*******************int 0x009 float number fault*******************',0x0d,0
	
	int_0x00a_msg 			db 0x0d,'*******************int 0x00a no effective TSS*******************',0x0d,0
	
	int_0x00b_msg 			db 0x0d,'*******************int 0x00b seg not exist*******************',0x0d,0
	
	int_0x00c_msg 			db 0x0d,'*******************int 0x00c stack seg fault*******************',0x0d,0
	
	int_0x00d_msg 			db 0x0d,'*******************int 0x00d routine protect*******************',0x0d,0
	
	int_0x00e_msg 			db 0x0d,'*******************int 0x00e page fault*******************',0x0d,0
	
	int_0x00f_msg 			db 0x0d,'*******************int 0x00f*******************',0x0d,0
	
	int_0x010_msg 			db 0x0d,'*******************int 0x010 FPU fault*******************',0x0d,0
	
	int_0x011_msg 			db 0x0d,'*******************int 0x011 align fault*******************',0x0d,0
	
	int_0x012_msg 			db 0x0d,'*******************int 0x012 machine check*******************',0x0d,0
	
	int_0x013_msg 			db 0x0d,'*******************int 0x013 SIMD fault*******************',0x0d,0
	
	int_0x028_msg			db 0x0d,'*******************int 0x028 no physical page*******************',0x0d,0
	
	int_0x029_msg			db 0x0d,'*******************int 0x029 realloc same page*******************',0x0d,0
	
	extern _asm_puts
	extern _asm_putChar

;================================================================
	
;================================================================

	global _asm_interuptException
	
	_asm_interuptException:
		
		mov eax,exception_msg
		push eax
		call _asm_puts
		add esp,4
			 
		hlt
		
;================================================================

	global _asm_interruptGeneral
	
	_asm_interruptGeneral:
		
		push eax
          
        mov al,0x20                        ;中断结束命令EOI 
        out 0xa0,al                        ;向从片发送 
        out 0x20,al                        ;向主片发送
         
        pop eax
          
        iret


;================================================================
	
	global _asm_int0x000
		
	_asm_int0x000:
		
		push eax
		push ecx
		push edx
		mov eax,int_0x000_msg
		push eax
		call _asm_puts
		add esp,16
		hlt
		
		iret
		
;================================================================		
	global _asm_int0x001
	
	_asm_int0x001:
		
		push eax
		push ecx
		push edx
		mov eax,int_0x001_msg
		push eax
		call _asm_puts
		add esp,16
		hlt
		
		iret
		
;================================================================
		
	global _asm_int0x002
	
	_asm_int0x002:
		
		push eax
		push ecx
		push edx
		mov eax,int_0x002_msg
		push eax
		call _asm_puts
		add esp,16
		hlt
		
		iret
		
		
		
		
;================================================================
		
	global _asm_int0x003
	
	_asm_int0x003:
		
		push eax
		push ecx
		push edx
		mov eax,int_0x003_msg
		push eax
		call _asm_puts
		add esp,16
		hlt
		
		iret	
		
		
		
;================================================================
		
	global _asm_int0x004
	
	_asm_int0x004:
		
		push eax
		push ecx
		push edx
		mov eax,int_0x004_msg
		push eax
		call _asm_puts
		add esp,16
		hlt
		
		iret	
		
;================================================================
		
	global _asm_int0x005
	
	_asm_int0x005:
		
		push eax
		push ecx
		push edx
		mov eax,int_0x005_msg
		push eax
		call _asm_puts
		add esp,16
		hlt
		
		iret	
		
		
;================================================================
		
	global _asm_int0x006
	
	_asm_int0x006:
		
		push eax
		push ecx
		push edx
		mov eax,int_0x006_msg
		push eax
		call _asm_puts
		add esp,16
		hlt
		
		iret	
		
;================================================================
		
	global _asm_int0x007
	
	_asm_int0x007:
		
		push eax
		push ecx
		push edx
		mov eax,int_0x007_msg
		push eax
		call _asm_puts
		add esp,16
		hlt
		
		iret	
		
		
;================================================================
		
	global _asm_int0x008
	
	_asm_int0x008:
		
		push eax
		push ecx
		push edx
		mov eax,int_0x008_msg
		push eax
		call _asm_puts
		add esp,20
		hlt
		
		iret	
		
;================================================================
	
	global _asm_int0x009
	
	_asm_int0x009:
		
		push eax
		push ecx
		push edx
		mov eax,int_0x009_msg
		push eax
		call _asm_puts
		add esp,16
		hlt
		
		iret	
	
	
	
;================================================================
	
	global _asm_int0x00a
	
	_asm_int0x00a:
		
		push eax
		push ecx
		push edx
		mov eax,int_0x00a_msg
		push eax
		call _asm_puts
		add esp,20
		hlt
		
		iret	
		
		
;================================================================
		
	global _asm_int0x00b
	
	_asm_int0x00b:
		
		push eax
		push ecx
		push edx
		mov eax,int_0x00b_msg
		push eax
		call _asm_puts
		add esp,20
		hlt
		
		iret	
		
	
;================================================================
	
	global _asm_int0x00c
	
	_asm_int0x00c:
		
		push eax
		push ecx
		push edx
		mov eax,int_0x00c_msg
		push eax
		call _asm_puts
		add esp,20
		hlt
		
		iret	
	
;================================================================
	
	global _asm_int0x00d
	
	_asm_int0x00d:
		
		push eax
		push ecx
		push edx
		mov eax,int_0x00d_msg
		push eax
		call _asm_puts
		add esp,20
		hlt
		
		iret

;================================================================

	global _asm_int0x00e
	
	extern putHex
	
	_msg1 db 0x0a,'wrong number : ',0x0
	
	_msg2 db 0x0a,'linearAddress : ',0x0
	
	_asm_int0x00e:
		
		push eax
		push ecx
		push edx
		push dword [esp+20]
		call putHex
		add esp,4
		mov eax,_msg2
		push eax
		call _asm_puts
		add esp,4
		mov eax,cr2
		push eax
		call putHex
		add esp,4
		mov eax,int_0x00e_msg
		push eax
		call _asm_puts
		add esp,20
		hlt
		
		iret

;================================================================

	global _asm_int0x00f
	
	_asm_int0x00f:
		
		push eax
		push ecx
		push edx
		mov eax,int_0x00f_msg
		push eax
		call _asm_puts
		add esp,16
		hlt
		
		iret


;================================================================

	global _asm_int0x010
	
	_asm_int0x010:
		
		push eax
		push ecx
		push edx
		mov eax,int_0x010_msg
		push eax
		call _asm_puts
		add esp,16
		hlt
		
		iret


;================================================================

	global _asm_int0x011
	
	_asm_int0x011:
		
		push eax
		push ecx
		push edx
		mov eax,int_0x011_msg
		push eax
		call _asm_puts
		add esp,20
		hlt
		
		iret


;================================================================		
		
	global _asm_int0x012
	
	_asm_int0x012:
		
		push eax
		push ecx
		push edx
		mov eax,int_0x012_msg
		push eax
		call _asm_puts
		add esp,16
		hlt
		
		iret

		
;================================================================

	global _asm_int0x013
	
	_asm_int0x013:
		
		
		push eax
		push ecx
		push edx
		mov eax,int_0x013_msg
		push eax
		call _asm_puts
		add esp,16
		hlt

		iret

;================================================================

	global _asm_int0x021

	extern keyBoardBuffer_push
	
	_asm_int0x021:
		
		push eax
          
        mov al,0x20                        ;中断结束命令EOI 
        out 0xa0,al                        ;向从片发送 
        out 0x20,al                        ;向主片发送

		xor eax,eax

		;8042芯片输出缓冲区
		in al,0x60

		push ecx
		push edx

		push eax
		;加入缓冲队列
		call keyBoardBuffer_push

		add esp,4
		pop edx
		pop ecx
         
        pop eax
          
        iret
		
;================================================================

	global _asm_int0x028
	
	_asm_int0x028:
		
		push eax
		push ecx
		push edx
		mov eax,int_0x028_msg
		push eax
		call _asm_puts
		add esp,16
		hlt
		
		iret
		
		
;================================================================

	global _asm_int0x029
	
	_asm_int0x029:
	
		push eax
		push ecx
		push edx
		mov eax,int_0x029_msg
		push eax
		call _asm_puts
		add esp,20
		hlt
		

	
;================================================================
	global _asm_int0x070
	
	%define pcbCr3Address(x)  x+28

	%define tcbss0(x) x+44

	%define tcbss(x) x+48

	%define tcbesp0(x) x+52

	%define tcbesp(x) x+56

	%define tcbeip(x) x+60

	%define tcbesp00(x) x+64

	%define tssSs0(x) x+8

	%define tssSs(x) x+80

	%define tssesp0(x) x+4

	%define tssesp(x) x+56

	
	extern sched
	
	_schmsg 	db 0x0a,'sheduling',0x0a,0
	
	curPcb 		dd 0
	
	nextPcb 	dd 0

	curTcb 		dd 0
	
	nextTcb 	dd 0

	tssSel		dd 0

	tss 		dd 0
	
	_asm_int0x070:
		
		push eax
	
		;中断结束命令EOI
		mov al,0x20
		;向8259A从片发送
		out 0xa0,al
        ;向8259A主片发送
		out 0x20,al

		;寄存器C的索引。且开放NMI
        mov al,0x0c
        out 0x70,al
		;读一下RTC的寄存器C，否则只发生一次中断
        in al,0x71


		push ecx
		push edx
		push dword tssSel
		push dword tss
		push dword nextTcb
		push dword curTcb
		push dword nextPcb
		push dword curPcb

		call sched

		add esp,24
		pop edx
		pop ecx
		
		cmp eax,0
		je .switch_process

		.switch_thread:

			pop eax

			push ds
			push es
			push fs
			push gs
			push eax
			push ebx
			push ecx
			push edx
			push ebp
			push esi
			push edi

			mov edi,[curTcb]
			mov esi,[nextTcb]

			;保存内核栈段
			xor eax,eax
			mov eax,ss
			mov [tcbss0(edi)],eax
			;保存内核栈指针
			mov [tcbesp0(edi)],esp
			;切换内核栈段
			mov eax,[tcbss0(esi)]
			mov ss,eax
			;切换内核栈指针
			mov esp,[tcbesp0(esi)]

			;ltr word [tssSel]

			pop edi
			pop esi
			pop ebp
			pop edx
			pop ecx
			pop ebx
			pop eax
			pop gs
			pop fs
			pop es
			pop ds

			iret

		.switch_process:
			pop eax

			push ds
			push es
			push fs
			push gs
			push eax
			push ebx
			push ecx
			push edx
			push ebp
			push esi
			push edi


			mov edi,[curTcb]
			mov esi,[nextTcb]
			mov ecx,[curPcb]
			mov edx,[nextPcb]

			;保存映射表
			mov eax,cr3
			mov [pcbCr3Address(ecx)],eax
			;切换映射表
			mov eax,[pcbCr3Address(edx)]
			mov cr3,eax

			;保存内核栈段
			xor eax,eax
			mov eax,ss
			mov [tcbss0(edi)],eax
			;保存内核栈指针
			mov [tcbesp0(edi)],esp
			;切换内核栈段
			mov eax,[tcbss0(esi)]
			mov ebx,[tss]
			mov [tssSs0(ebx)],eax
			mov ss,eax
			;切换内核栈指针
			mov esp,[tcbesp0(esi)]
			;修改TSS内的内核栈段和内核栈指针
			mov eax,[tcbesp00(esi)]
			mov [tssesp0(ebx)],eax

			;ltr word [tssSel]

			pop edi
			pop esi
			pop ebp
			pop edx
			pop ecx
			pop ebx
			pop eax
			pop gs
			pop fs
			pop es
			pop ds



		iret
		
;================================================================

	global _asm_int0x080

	extern SysCall
	
	_asm_int0x080:
		
		push eax
		push ebx
		push ecx
		push edx
		push edi
		push esi

		push edi
		push edx
		push ecx
		push ebx
		push eax

		call SysCall

		add esp,20
		

		pop esi
		pop edi
		pop edx
		pop ecx
		pop ebx
		pop eax
		

		iret		
	
		
		
		
		