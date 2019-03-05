		;内核加载起始地址
		final_mem_kernel_adress                     equ 0x10000
		
		;内核加载扇区
		final_mem_kernel_disk_sector                equ 0x1
		
		;4GB数据段GDT选择西
		final_mem_gdt_4gb_sel                     equ 0x0008
		
		;4GB代码段描述符
		final_code_gdt_4gb_sel                    equ 0x0010
		
		;显示缓冲区选择子
		final_mem_gdt_video_sel                   equ 0x0018
		
		;内核栈段选择子
		final_mem_gdt_stack_sel                   equ 0x0020
		
		;显示缓冲区地址
		final_mem_video_adress                    equ 0x800b_8000
		
		;ring0调用门属性
		final_attribute_ring0_call_gate           equ 0000_0000_0000_0000_1000_1100_0000_0000b
		
		;ring3调用门属性
		final_attribute_ring3_call_gate           equ 0000_0000_0000_0000_1110_1100_0000_0000b
		
		;ring0中断门属性
		final_attribute_ring0_interrupt_gate      equ 0000_0000_0000_0000_1000_1110_0000_0000b
		
		;ring0 陷阱门属性
		final_attribute_ring0_trap_gate           equ 0000_0000_0000_0000_1000_1111_0000_0000b
		
		;ring0 任务门属性
		final_attribute_ring0_task_gate           equ 0000_0000_0000_0000_1000_0101_0000_0000b
		
		;ring3 LDT 4GB 代码段描述符属性
		final_attribute_ring3_LDT_code            equ 0x00c0f800
		
		;ring3 LDT 4GB 数据段描述符属性
		final_attribute_ring3_LDT_data            equ 0x00c0f200
		
		;ring0 安装在GDT中的LDT描述符
		final_attribute_ring0_LDT                 equ 0x00408200
		
		final_tcb_len                             equ 36
		
		final_tss_len                             equ 104
		
	extern _asm_puts	
	
	extern putHex
		
;================================================================

	global _asm_test
	
	_asm_test:
		int 0x28
		ret
		
;================================================================

		
		
		
		
		
;================================================================

	global _asm_allocOneKernelPage
	
	_enter  db 0x0a,0
	
	
	_asm_allocOneKernelPage:
		;extern void _asm_allocOneKernelPage(Uint32 linearAdress);
		
		push ebx
		push ebp
		push edi
		push esi
		
		mov ebp,esp
		
		;线性地址
		mov esi,[ebp+20]
		
		mov eax,esi
		shr eax,22
		shl eax,2
		or eax,0xffff_f000
		
		test dword [eax],0x0000_0001
		jnz .has_page_kernel_table
		
		push eax
		push ecx
		push edx
		call _asm_alloc4kbPage
		add esp,8
		pop ebx
		or eax,0x0000_0003
		mov [ebx],eax
		
		.has_page_kernel_table:
			mov eax,esi
			shr eax,22
			shl eax,12
			or eax,0xffc0_0000
			
			mov ebx,esi
			;保留中间10位
			and ebx,0000_0000_0011_1111_1111_0000_0000_0000b
			shr ebx,10
			or ebx,eax
			;测试该页表项是否存在
			test dword [ebx],0x0000_0001
			;如果已经存在
			jz .need_alloc_kernel_page
			int 0x29
			.need_alloc_kernel_page:
				push eax
				push ecx
				push edx
				call _asm_alloc4kbPage
				or eax,0x0000_0003
				mov [ebx],eax
				add esp,12
		
		pop esi
		pop edi
		pop ebp
		pop ebx
		ret
				
;================================================================

;================================================================

	global _asm_allocOneUserPage
	
	
	_asm_allocOneUserPage:
		;extern void _asm_allocOneUserPage(Uint32 linearAdress);
		
		push ebx
		push ebp
		push edi
		push esi
		
		mov ebp,esp
		
		;线性地址
		mov esi,[ebp+20]
		
		mov eax,esi
		shr eax,22
		shl eax,2
		or eax,0xffff_f000
		
		test dword [eax],0x0000_0001
		jnz .has_page_user_table
		
		push eax
		push ecx
		push edx
		call _asm_alloc4kbPage
		add esp,8
		pop ebx
		or eax,0x0000_0007
		mov [ebx],eax
		
		.has_page_user_table:
			mov eax,esi
			shr eax,22
			shl eax,12
			or eax,0xffc0_0000
			
			mov ebx,esi
			;保留中间10位
			and ebx,0000_0000_0011_1111_1111_0000_0000_0000b
			shr ebx,10
			or ebx,eax
			;测试该页表项是否存在
			test dword [ebx],0x0000_0001
			;如果已经存在
			jz .need_alloc_user_page
			int 0x29
			.need_alloc_user_page:
				push eax
				push ecx
				push edx
				call _asm_alloc4kbPage
				or eax,0x0000_0007
				mov [ebx],eax
				add esp,12
		
		pop esi
		pop edi
		pop ebp
		pop ebx
		ret
				
;================================================================
	
	global _asm_alloc4kbPage
	
	pageBitmap times (131072) db 0
		
	_asm_alloc4kbPage:
	
		push edi
		push esi
		
		mov ecx,0x100000
		xor eax,eax
		mov esi,pageBitmap
		
		.search_null_page:
			bts [esi],eax
			jnc .has_page
			inc eax
			loop .search_null_page
			
		int 0x28
		
			
		.has_page:
			shl eax,12
			
		pop esi
		pop edi
		ret

		
;================================================================
				
	global _asm_iniPageBitmap
		
	_asm_iniPageBitmap:
		;extern void _asm_iniPageBitmap();
		
		push ebx
		push ebp
		
		mov ebp,esp
		
		;初始化物理页映射串
		mov ecx,32
		mov edx,pageBitmap
		
		.ini_page_bits:
			mov eax,[edx]
			or eax,0xffff_ffff
			mov [edx],eax
			add edx,4
			loop .ini_page_bits
			
		mov eax,[edx]
		or eax,0000_0011b
		mov [edx],eax
			
		pop ebp
		pop ebx
		ret	
		
	

;================================================================		

	global _asm_getGateDescriptor
	
	_asm_getGateDescriptor:
		;extern void _asm_getCallGateDescriptor(Selector codeSelector, Uint32 entryAdress, Uint32 attribute, Byte* descriptor);
		
		push ebx
		push ebp
		push edi
		push esi
		
		mov ebp,esp

		;代码所在段选择子
		mov eax,[ebp+20]
		;代码入口
		mov ebx,[ebp+24]
		;描述符属性
		mov ecx,[ebp+28]
		
		movzx esi,bx
		shl eax,16
		or esi,eax
		
		and ebx,0xffff_0000
		mov edi,ebx
		mov di,cx
		
		mov eax,esi
		mov ebx,edi
		
		;要返回的选择子
		mov esi,[ebp+32]
		mov [esi],eax
		mov [esi+4],ebx
		
		pop esi
		pop edi
		pop ebp
		pop ebx
		ret
		
;================================================================

	global _asm_getLdtDescriptor
	
	_asm_getLdtDescriptor:
		;extern void _asm_getLdtDescriptor(Uint32 baseAdress, Uint32 limted, Uint32 attribute, Byte* descriptor);

		push ebx
		push ebp
		push edi
		push esi
		
		mov ebp,esp
		
		;线性基地址
		mov eax,[ebp+20]
		;段界限
		mov ebx,[ebp+24]
		;属性
		mov ecx,[ebp+28]
		
		movzx esi,bx
		mov edx,eax
		shl edx,16
		or esi,edx
		
		mov edi,eax
		and edi,0xffff_0000
		rol edi,8
		bswap edi
		
		xor bx,bx
		or edi,ebx
		
		or edi,ecx
		
		mov ebx,edi
		mov eax,esi
		
		;要返回的描述符
		mov esi,[ebp+32]
		mov [esi],eax
		mov [esi+4],ebx
		
		pop esi
		pop edi
		pop ebp
		pop ebx
		ret
		
;================================================================

	global _asm_getGdtDescriptor
	
	_asm_getGdtDescriptor:
		;extern void _asm_getGdtDescriptor(Uint32 baseAdress, Uint32 limted, Uint32 attribute, Byte* descriptor);
		
		push ebx
		push ebp
		push edi
		push esi
		
		mov ebp,esp
		
		;线性基地址
		mov eax,[ebp+20]
		;段界限
		mov ebx,[ebp+24]
		;属性
		mov ecx,[ebp+28]
		
		movzx esi,bx
		mov edx,eax
		shl edx,16
		or esi,edx
		
		mov edi,eax
		and edi,0xffff_0000
		rol edi,8
		bswap edi
		
		xor bx,bx
		or edi,ebx
		
		or edi,ecx
		
		mov ebx,edi
		mov eax,esi
		
		;要返回的描述符
		mov esi,[ebp+32]
		mov [esi],eax
		mov [esi+4],ebx
		
		pop esi
		pop edi
		pop ebp
		pop ebx
		ret
		
		
;================================================================

	global _asm_setupGdtDescriptor
	global _asm_pgdt
	
	_asm_pgdt
		dw 0
		dd 0
	
	_asm_setupGdtDescriptor:
		;extern void _asm_setupGdtDescriptor(Byte* descriptor, Selector* selector);
		
		push ebx
		push ebp
		push edi
		push esi
		
		mov ebp,esp
		
		;描述符
		mov esi,[ebp+20]
		mov eax,[esi]
		mov ebx,[esi+4]
		
		;pgdt
		mov esi,[_asm_pgdt]
		sgdt [esi]
		
		movzx edx,word [esi]
		inc dx
		mov edi,[esi+0x02]
		add edx,edi
		
		mov [edx],eax
		mov [edx+0x04],ebx
		
		mov dx,[esi]
		add dx,8
		mov [esi],dx
		
		lgdt [esi]
		
		
		mov cx,[esi]
		shr cx,3
		shl cx,3
		mov esi,[ebp+24]
		mov [esi],cx
		
		pop esi
		pop edi
		pop ebp
		pop ebx
		ret
		
;================================================================

	global _asm_setupLdtDescriptor
	
	_asm_setupLdtDescriptor:
		;extern void _asm_setupLdtDescriptor(Pldt* pldt, Byte* descriptor, Selector* selector);
		
		push ebx
		push ebp
		push edi
		push esi
		
		mov ebp,esp
		
		;描述符
		mov esi,[ebp+24]
		mov eax,[esi]
		mov ebx,[esi+4]
		
		;pldt
		mov esi,[ebp+20]
		
		movzx edx,word [esi]
		inc dx
		mov edi,[esi+0x02]
		add edx,edi
		
		mov [edx],eax
		mov [edx+0x04],ebx
		
		mov dx,[esi]
		add dx,8
		mov [esi],dx
		
		
		mov cx,[esi]
		shr cx,3
		shl cx,3
		and cx,1111_1111_1111_1000b
		mov esi,[ebp+28]
		mov [esi],cx
		
		pop esi
		pop edi
		pop ebp
		pop ebx
		ret
		
;================================================================

	global _asm_setupIntrruptGate
	
	_asm_setupIntrruptGate:
		;extern void _asm_setupIntrruptGate(Uint32 IDTbaseAdress, Uint32 index, Byte* descriptor);
		
		push ebx
		push ebp
		push edi
		push esi
		
		mov ebp,esp
		
		;IDT基址
		mov esi,[ebp+20]
		;索引
		mov ecx,[ebp+24]
		;中断门描述符
		mov edx,[ebp+28]
		mov eax,[edx]
		mov ebx,[edx+4]
		
		mov [esi+ecx*8],eax
		mov [esi+ecx*8+4],ebx
		
		pop esi
		pop edi
		pop ebp
		pop ebx
		ret
		
;================================================================

	global _asm_saveIDT
	global _asm_pidt
	
	_asm_pidt
		dw 0
		dd 0
	
	_asm_saveIDT:
		;extern void _asm_saveIDT(Uint32 IDTbaseAdress, Uint16 limted);
		
		push ebp
		push esi
		
		mov ebp,esp
		
		mov esi,[ebp+12]
		mov [_asm_pidt+2],esi
		mov si,[ebp+16]
		mov [_asm_pidt],si
		
		lidt[_asm_pidt]
		
		pop esi
		pop ebp
		ret
		
;================================================================

	global _asm_ini8259A
	
	_asm_ini8259A:
		;extern void _asm_ini8259A();
		
		;设置8259A中断控制器
         mov al,0x11
         out 0x20,al                        ;ICW1：边沿触发/级联方式
         mov al,0x20
         out 0x21,al                        ;ICW2:起始中断向量
         mov al,0x04
         out 0x21,al                        ;ICW3:从片级联到IR2
         mov al,0x01
         out 0x21,al                        ;ICW4:非总线缓冲，全嵌套，正常EOI

         mov al,0x11
         out 0xa0,al                        ;ICW1：边沿触发/级联方式
         mov al,0x70
         out 0xa1,al                        ;ICW2:起始中断向量
         mov al,0x04
         out 0xa1,al                        ;ICW3:从片级联到IR2
         mov al,0x01
         out 0xa1,al                        ;ICW4:非总线缓冲，全嵌套，正常EOI

         ;设置和时钟中断相关的硬件 
         mov al,0x0b                        ;RTC寄存器B
         or al,0x80                         ;阻断NMI
         out 0x70,al
         mov al,0x12                        ;设置寄存器B，禁止周期性中断，开放更
         out 0x71,al                        ;新结束后中断，BCD码，24小时制

         in al,0xa1                         ;读8259从片的IMR寄存器
         and al,0xfe                        ;清除bit 0(此位连接RTC)
         out 0xa1,al                        ;写回此寄存器

         mov al,0x0c
         out 0x70,al
         in al,0x71                         ;读RTC寄存器C，复位未决的中断状态
		 
		 ret

		
;================================================================

	global _asm_closeInterrupt
	
	_asm_closeInterrupt:
		cli
		ret
		
;================================================================
		
	global _asm_openInterrupt
	
	_asm_openInterrupt:
		sti
		ret	
	
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		