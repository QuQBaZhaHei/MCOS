

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
		
		;中断描述符表基地址
		final_mem_idt_adress                      equ IDT
		
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
		
	[bits 32]
		%define tss_link(x)    x+000
		%define tss_esp0(x)    x+004
		%define tss_ss0(x)     x+008
		%define tss_esp1(x)    x+012
		%define tss_ss1(x)     x+016
		%define tss_esp2(X)    x+020
		%define tss_ss2(x)     x+024
		%define tss_cr3(X)     x+028
		%define tss_eip(x)     x+032
		%define tss_eflags(x)  x+036
		%define tss_eax(x)     x+040
		%define tss_ecx(X)     x+044
		%define tss_edx(x)     x+048
		%define tss_ebx(x)     x+052
		%define tss_esp(x)     x+056
		%define tss_ebp(x)     x+060
		%define tss_esi(X)     x+064
		%define tss_edi(x)     x+068
		%define tss_es(X)      x+072
		%define tss_cs(x)      x+076
		%define tss_ss(x)      x+080
		%define tss_ds(x)      x+084
		%define tss_fs(x)      x+088
		%define tss_gs(x)      x+092
		%define tss_ldt_sel(x) x+096
		
		%define tcb_link(x)           x+000
		%define tcb_pid(x)            x+004
		%define tcb_pname(x)          x+008
		%define tcb_next_adress(x)    x+012
		%define tcb_addition(x)       x+016
		%define tcb_LDT_adress(x)     x+020
		%define tcb_LDT_sel(x)        x+024
		%define tcb_LDT_limit(x)      x+026
		%define tcb_TSS_adress(x)     x+028
		%define tcb_TSS_sel(x)        x+032
		%define tcb_Tss_limit(x)      x+034
		
		
		kernelBuffer 		equ _asm_kernelBuffer
		
		pageBitmap 		equ _asm_pageBitmap
		
		pgdt 			equ _asm_pgdt
		
		SCT 			equ _asm_SCT
		
		IDT 			equ _asm_IDT
		
		iniPageBitmap 	equ _asm_iniPageBitmap
		
		iniSysCall		equ _asm_iniSysCall
		
		iniIDT			equ _asm_iniIDT
		
		
		global _asm_IDT
		
		global _asm_SCT
		
		global _asm_kernelBuffer
		
		global _asm_pageBitmap
		
		global _asm_pgdt
		
		
		
		
		
		
		
	

section kernel vstart=0x80010000
	
	;内核长度
	kernel_length         	dd kernel_end
	
	;内核入口
	kernel_entry          	dd start
	
	;内核空间下次地址
	kernel_next_adress        dd 0x80412000
	
	;TCB头节点
	tcb_head           	 	dd 0
	
	;TCB尾节点
	tcb_end           	  	dd 0
	
	;物理页映射串
	pageBitmap           	times 0x8000 dd 0x0000_0000
	
	final_pageBitmap_len	dd $-pageBitmap
	
	;GDT
	pgdt                	dw 0
							dd 0
			   
			   
	;IDT
	pidt                	dw 0
							dd final_mem_idt_adress
			   
	;系统调用表		   
	SCT:
							put_string dd _put_string
									   dw 0
							read_disk  dd _read_hard_disk
									   dw 0
							put_char   dd _put_char
				                       dw 0				   
							times (97*6) db 0
								   
								   
	final_SCT_len       	equ $-SCT
	final_SCT_counts    	equ final_SCT_len/6
	
	;中断描述符表
	IDT                 	times (256*8) db 0
	
	;内核缓冲区
	kernelBuffer                times 1024 db 0

	msg                 	db 0x0d,'Hello World',0x00
	
	no_page_msg             db 0x0d,'*******************no page*******************',0x0d,0
	
	excep_msg 				db 0x0d,'*******************exception*******************',0x0d,0
	
	int_0x000_msg 			db 0x0d,'*******************int 0x000 div by zero*******************',0x0d,0
	
	int_0x004_msg 			db 0x0d,'*******************int 0x004 overflow*******************',0x0d,0
	
	int_0x00a_msg 			db 0x0d,'*******************int 0x00a no effective TSS*******************',0x0d,0
	
	int_0x00b_msg 			db 0x0d,'*******************int 0x00b seg not exist*******************',0x0d,0
	
	int_0x00c_msg 			db 0x0d,'*******************int 0x00c stack seg fault*******************',0x0d,0
	
	int_0x00d_msg 			db 0x0d,'*******************int 0x00d routine protect*******************',0x0d,0
	
	int_0x00e_msg 			db 0x0d,'*******************int 0x00e page fault*******************',0x0d,0
	
	start:
	
		call iniPageBitmap
		call iniSysCall
		call iniIDT
		
		;mov esi,0x4444_4000
		;call alloc_inst_page
		
		mov ecx,20
		mov esi,0x8003f000
		mov eax,500
		.n1:
			call far [read_disk]
			add esi,512
			inc eax
			loop .n1
		jmp 0x8003f094
		
		
		
		jmp $
		


;================================================================		
		
	load_user_program:
		;输入 push 逻辑扇区号
		;     push TCB基地址
		pushad
		
		mov ebp,esp
		
		xor esi,esi
		mov ecx,512
		
		.clear_user_pdt:
			mov dword [esi*4+0xffff_f000],0x0000_0000
			inc esi
			loop .clear_user_pdt
		
		mov eax,[ebp+40]
		mov esi,kernelBuffer
		call far [read_disk]
		
		mov eax,[kernelBuffer]
		xor edx,edx
		mov ebx,512
		div ebx
		or edx,edx
		;如果没有整除
		jnz .has_remainder
		dec eax
		
		.has_remainder:
			cmp eax,0
			jz .fill_TCB
		
		mov ecx,eax
		;TCB基地址
		push dword [ebp+36]
		mov ebx,512
		xor edx,edx
		mul ebx
		;要分配得内存大小
		push eax
		call alloc_user_mem
		mov esi,eax
		mov eax,[ebp+40]
		
		.read_user_program:
			call far [read_disk]
			inc eax
			add esi,512
			loop .read_user_program
		
		.fill_TCB:
			
			ret
		
		
;================================================================		
		
	create_user_PDT:
		;输出 esi 新页目录得物理地址
		push eax
		push ebx
		push ecx
		push edx
		
		call alloc_a_4kb_page
		mov esi,eax
		mov ebx,eax
		or ebx,0x0000_0007
		mov [0xffff_fff8],ebx
		
		invlpg [0xffff_fff8]
		
		mov ecx,1024
		xor eax,eax
		xor ebx,ebx
		
		.copy_cur_pdt:
			mov eax,[ebx*4+0xffff_f000]
			mov [ebx*4+0xffff_e000],eax
			inc ebx
			loop .copy_cur_pdt
		
		pop edx
		pop ecx
		pop ebx
		pop eax
		ret
			

;================================================================			

		alloc_user_mem:
		;输入 push TCB基地址
		;     push 要分配得内存大小
		;输出 eax 内存起始地址
		pushad
		push esp
		
		mov ebp,esp
		
		;取出TCB基地址
		mov esi,[ebp+40]
		mov ecx,[tcb_next_adress(esi)]
		;取出需要的内存大小
		mov eax,[ebp+36]
		add eax,ecx
		;更新下次分配内存时的首地址
		mov [tcb_next_adress(esi)],eax
		
		;判断是否需要分配新的页面
		;ecx为更新前的下次分配内存的首地址
		mov edx,ecx
		;4kb对齐
		and edx,0xffff_f000
		;eax更新后的下次分配内存的首地址
		mov ebx,eax
		sub ebx,edx
		cmp ebx,4096
		jle .alloc_user_mem_ret
		
		.need_alloc_page:
			;ecx为更新前的下次分配内存的首地址
			mov edx,ecx
			and edx,0xffff_f000
			add edx,4096
		
		
		.alloc_user_mem_ret:
			pop esp
			popad
			ret 8
	

;================================================================

	alloc_kernel_mem:
		;输入 push 要分配得内存大小
		;输出 eax 内存起始地址

		ret
			
		
;================================================================
		
	alloc_inst_page:
		;输入 esi 线性地址
		push eax
		push edi
		
		mov edi,esi
		shr edi,22
		shl edi,2
		or edi,0xffff_f000
		test dword [edi],0x0000_0001
		jnz .b1
		
		call alloc_a_4kb_page
		or eax,0x0000_0007
		mov [edi],eax
		
		.b1:
			mov eax,esi
			shr eax,10
			and eax,0000_0000_0011_1111_1111_0000_0000_0000b
			or eax,1111_1111_1100_0000_0000_0000_0000_0000b
			
			mov edi,esi
			shl edi,10
			shr edi,22
			shl edi,2
			or edi,eax
			
			call alloc_a_4kb_page
			or eax,0x0000_0007
			mov [edi],eax
			
		pop edi
		pop eax
		ret
			
		

;================================================================		

	alloc_a_4kb_page:
		;输出 eax 页的物理地址
		push ecx
		push esi
		
		mov ecx,final_pageBitmap_len*8
		xor eax,eax
		
		.find_page:
			bts [pageBitmap],eax
			jnc .has_page
			inc eax
			loop .find_page
			
		;没有找到空闲页
		mov esi,no_page_msg
		call far [put_string]
		hlt
			
		.has_page:
			shl eax,12
		
		pop esi
		pop ecx
		ret

		
;================================================================	

	tcb_append:
		;输入 esi TCB基地址
		push eax
		push ebx
		push esi
		
		mov eax,[tcb_head]
		mov ebx,[tcb_end]
		
		or eax,ebx
		jz .no_tcb
		mov eax,[tcb_end]
		mov [tcb_link(eax)],esi
		mov [tcb_end],esi
		jmp .tcb_append_ret
		
		.no_tcb:
			mov [tcb_head],esi
			mov [tcb_end],esi
		
		.tcb_append_ret:
			pop esi
			pop ebx
			pop eax
			ret
			
			
			
			
			
			
;================================================================			
	
	make_gdt_or_ldt_descriptor:
		;输入 eax      线性基地址
		;	  ebx      段界限
		;     ecx      属性
		;输出 ebx:eax  64位GDT描述符
		push edx
		push edi
		push esi
		
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
		
		pop esi
		pop edi
		pop edx
		ret
;================================================================	
		
	make_gate_desciptor:
		;输入 eax       代码所在段选择子
		;     ebx       段内偏移量
		;     ecx       属性
		;输出 ebx:eax   64位调用门描述符
		push edx
		push edi
		push esi
		
		movzx esi,bx
		shl eax,16
		or esi,eax
		
		and ebx,0xffff_0000
		mov edi,ebx
		mov di,cx
		
		mov eax,esi
		mov ebx,edi
		
		pop esi
		pop edi
		pop edx
		ret
		
		
		
		
;================================================================		
		
	setup_gdt_or_ldt_descriptor:
		;输入 ebx:eax 64位GDT OR LDT描述符
		;     cx TI:RPL
		;输出 cx 16位选择子
		push edx
		push edi
		push esi
		
		
		sgdt [pgdt]
		
		movzx edx,word [pgdt]
		inc dx
		mov edi,[pgdt+0x02]
		add edx,edi
		
		mov [edx],eax
		mov [edx+0x04],ebx
		
		mov dx,[pgdt]
		add dx,8
		mov [pgdt],dx
		
		lgdt [pgdt]
		
		
		mov si,[pgdt]
		shr si,3
		shl si,3
		or cx,si
		
		pop esi
		pop edi
		pop edx
		
		ret


;================================================================		
		
	fill_descriptor_in_ldt:
		;输入 esi TCB 基地址
		;     ebx:eax LDT描述符
		;输出 cx 选择子
		
		push  edx
		
		
		xor ecx,ecx
		mov edx,[tcb_LDT_adress(esi)]
		mov cx,[tcb_LDT_limit(esi)]
		inc cx
		
		add edx,ecx
		mov [edx],eax
		mov [edx+0x04],ebx
		
		mov cx,[tcb_LDT_limit(esi)]
		add cx,8
		mov [tcb_LDT_limit(esi)],cx
		
		shr cx,3
		shl cx,3
		or cx,0000_0000_0000_0100B
		
		pop edx
		ret
		
		
	
;================================================================	
	
	_read_hard_disk:
		;输入 ds  缓冲区段 
		;     esi 缓冲区偏移
		;     eax 扇区号
		
		push eax
		push ebx
		push ecx
		push esi
		
		push eax
		
		mov dx,0x1f2
		mov al,0x01
		out dx,al
		
		inc dx
		pop eax
		out dx,al
		
		inc dx
		shr eax,8
		out dx,al
		
		inc dx
		shr eax,8
		out dx,al
		
		inc dx
		shr eax,8
		or al,0xe0
		out dx,al
		
		inc dx
		mov al,0x20
		out dx,al
		
		.wait_disk:
			in al,dx
			and al,1000_1000b
			cmp al,0000_1000b
			jnz .wait_disk
			
		mov ecx,256
		mov dx,0x1f0
		
		.read_disk:
			in ax,dx
			mov [esi],ax
			add esi,2
			loop .read_disk
			
		pop esi
		pop ecx
		pop ebx
		pop eax
		retf
		
		
;================================================================		
		
	_put_char:
		;输入 cl ASCII码
		
		push eax
		push ebx
		push edx
		push esi
		push edi
		
		
		mov dx,0x3d4
		mov al,0x0e
		out dx,al
		inc dx
		;取出光标高八位
		in al,dx
		mov ah,al
		
		dec dx
		mov al,0x0f
		out dx,al
		inc dx
		;取出光标低八位
		in al,dx
		
		mov bx,ax
		and ebx,0x0000_ffff
		
		.put_enter:
			cmp cl,0x0d
			jne .put_other
			mov eax,ebx
			xor edx,edx
			mov ebx,80
			div ebx
			inc eax
			xor edx,edx
			mul ebx
			mov ebx,eax
			jmp .roll_screen
			
		.put_other:
			shl ebx,1
			mov [final_mem_video_adress+ebx],cl
			shr ebx,1
			inc ebx
			
		.roll_screen:
			cmp ebx,2000
			jl .set_cursor
			
			cld
			
			mov esi,final_mem_video_adress+0xa0
			mov edi,final_mem_video_adress
			mov ecx,1920
			rep movsd
			
		mov ebx,3840
		mov ecx,80
		
		.clean_last_line:
			mov word [final_mem_video_adress+ebx],0x0720
			add ebx,2
			loop .clean_last_line
			
			mov ebx,1920
		
		.set_cursor:
			mov dx,0x3d4
			mov al,0x0e
			out dx,al
			inc dx
			mov al,bh
			out dx,al
			
			dec dx
			mov al,0x0f
			out dx,al
			inc dx
			mov al,bl
			out dx,al
			
		pop edi
		pop esi
		pop edx
		pop ebx
		pop eax
		retf
	

;================================================================	
	
	iniPageBitmap:
	
		push eax
		push ecx
		push edx
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
		or eax,0x0000_0011b
		mov [edx],eax
			
		pop edx
		pop ecx
		pop eax
		ret
	

	
;================================================================	
	
	iniSysCall:
		push eax
		push ebx
		push ecx
		push esi
		
		mov eax,final_code_gdt_4gb_sel
		mov ecx,final_SCT_counts
		mov esi,SCT
		
		;初始化系统调用
		.make_sys_call:
			mov ebx,[esi]
			push ecx
			;特权级为0，参数个数为0
			mov ecx,final_attribute_ring3_call_gate
			mov eax,final_code_gdt_4gb_sel
			call make_gate_desciptor
			xor ecx,ecx
			call setup_gdt_or_ldt_descriptor
			mov [esi+0x04],cx
			add esi,0x06
			pop ecx
			loop .make_sys_call
			
		pop esi
		pop ecx
		pop ebx
		pop eax
		ret
		
		

;================================================================
		
	iniIDT:
		pushad
			
		mov eax,final_code_gdt_4gb_sel
		mov ebx,general_interrupt_handler
		mov ecx,final_attribute_ring0_interrupt_gate
		call make_gate_desciptor
		
		mov ecx,20
		xor esi,esi
		
		.iniIDT_0_20:
			mov [esi*8+final_mem_idt_adress],eax
			mov [esi*8+final_mem_idt_adress+0x04],ebx
			inc esi
			loop .iniIDT_0_20
		
		mov eax,final_code_gdt_4gb_sel
		mov ebx,general_exception_handler
		mov ecx,final_attribute_ring0_interrupt_gate
		call make_gate_desciptor
		
		mov ecx,235
		
		.iniIDT_other:
			mov [esi*8+final_mem_idt_adress],eax
			mov [esi*8+final_mem_idt_adress+0x04],ebx
			inc esi
			loop .iniIDT_other
		
		mov eax,final_code_gdt_4gb_sel
		mov ebx,int_0x000_handler
		mov ecx,final_attribute_ring0_interrupt_gate
		call make_gate_desciptor
		mov esi,final_mem_idt_adress
		mov [esi],eax
		mov [esi+0x04],ebx
		
		mov eax,final_code_gdt_4gb_sel
		mov ebx,int_0x004_handler
		mov ecx,final_attribute_ring0_interrupt_gate
		call make_gate_desciptor
		mov esi,final_mem_idt_adress
		mov [esi+8*4],eax
		mov [esi+8*4+0x04],ebx
		
		mov eax,final_code_gdt_4gb_sel
		mov ebx,int_0x00a_handler
		mov ecx,final_attribute_ring0_interrupt_gate
		call make_gate_desciptor
		mov esi,final_mem_idt_adress
		mov [esi+8*0x0a],eax
		mov [esi+8*0x0a+0x04],ebx
		
		mov eax,final_code_gdt_4gb_sel
		mov ebx,int_0x00b_handler
		mov ecx,final_attribute_ring0_interrupt_gate
		call make_gate_desciptor
		mov esi,final_mem_idt_adress
		mov [esi+8*0x0b],eax
		mov [esi+8*0x0b+0x04],ebx
		
		mov eax,final_code_gdt_4gb_sel
		mov ebx,int_0x00c_handler
		mov ecx,final_attribute_ring0_interrupt_gate
		call make_gate_desciptor
		mov esi,final_mem_idt_adress
		mov [esi+8*0x0c],eax
		mov [esi+8*0x0c+0x04],ebx
		
		mov eax,final_code_gdt_4gb_sel
		mov ebx,int_0x00d_handler
		mov ecx,final_attribute_ring0_interrupt_gate
		call make_gate_desciptor
		mov esi,final_mem_idt_adress
		mov [esi+8*0x0d],eax
		mov [esi+8*0x0d+0x04],ebx
		
		mov eax,final_code_gdt_4gb_sel
		mov ebx,int_0x00e_handler
		mov ecx,final_attribute_ring0_interrupt_gate
		call make_gate_desciptor
		mov esi,final_mem_idt_adress
		mov [esi+8*0x0e],eax
		mov [esi+8*0x0e+0x04],ebx
		
		mov word [pidt],256*8-1
		lidt [pidt]
		
		popad
		ret
		

;================================================================		
		
	_put_string:
		;输入 ds  字符串所在段
		;     esi 字符串段内偏移
		push esi
			
		.scan_string:
			mov cl,[esi]
			or cl,cl
			jz .return
			call far [put_char]
			inc esi
			jmp .scan_string
			
			
		.return:
			pop esi
			retf
			

;================================================================
			
	general_interrupt_handler:                  
		;通用的中断处理过程
		push eax
		
		;中断结束命令EOI 
		mov al,0x20
		;向从片发送 
		out 0xa0,al
		;向主片发送
		out 0x20,al
			 
		pop eax
		iretd



;================================================================		
	
	general_exception_handler:                  
		;通用的异常处理过程
		mov ebx,excep_msg
		call far [put_string]
			 
		hlt
			


;================================================================			
	
	int_0x000_handler:
		;通用的中断处理过程
		push eax
		push esi
			
		mov esi,int_0x000_msg
		call far [put_string]
			
		;中断结束命令EOI 
		mov al,0x20
		;向从片发送 
		out 0xa0,al
		;向主片发送
		out 0x20,al
		 
		pop esi 
		pop eax
			
		iretd



;================================================================
		
	int_0x004_handler:
		;通用的中断处理过程
		push eax
		push esi
		
		mov esi,int_0x004_msg
		call far [put_string]
			
		;中断结束命令EOI 
		mov al,0x20
		;向从片发送 
		out 0xa0,al
		;向主片发送
		out 0x20,al
		 
		pop esi 
		pop eax
			
		iretd
			



;================================================================
			
	int_0x00a_handler:                  
		;通用的中断处理过程
		push eax
		push esi
			
			
		mov esi,int_0x00a_msg
		call far [put_string]
			
		;中断结束命令EOI 
		mov al,0x20
		;向从片发送 
		out 0xa0,al
		;向主片发送
		out 0x20,al
			 
		pop esi
		pop eax
		iretd
			


;================================================================			
			
	int_0x00b_handler:                  
		;通用的中断处理过程
		push eax
		push esi
			
			
		mov esi,int_0x00b_msg
		call far [put_string]
			
		;中断结束命令EOI 
		mov al,0x20
		;向从片发送 
		out 0xa0,al
		;向主片发送
		out 0x20,al
			 
		pop esi
		pop eax
		iretd



;================================================================		
			
	int_0x00c_handler:                  
		;通用的中断处理过程
		push eax
		push esi
			
			
		mov esi,int_0x00c_msg
		call far [put_string]
			
		;中断结束命令EOI 
		mov al,0x20
		;向从片发送 
		out 0xa0,al
		;向主片发送
		out 0x20,al
			 
		pop esi
		pop eax
		iretd
			
			

;================================================================
			
	int_0x00d_handler:                  
		;通用的中断处理过程
		push eax
		push esi
			
			
		mov esi,int_0x00d_msg
		call far [put_string]
			
		;中断结束命令EOI 
		mov al,0x20
		;向从片发送 
		out 0xa0,al
		;向主片发送
		out 0x20,al
			 
		pop esi
		pop eax
		add esp,4
		iret
			
		
		

;================================================================

	int_0x00e_handler:                  
		;通用的中断处理过程
		push eax
		push esi
			
			
		mov esi,int_0x00e_msg
		call far [put_string]
			
		;中断结束命令EOI 
		mov al,0x20
		;向从片发送 
		out 0xa0,al
		;向主片发送
		out 0x20,al
			 
		pop esi
		pop eax
		add esp,4
		iretd
	
	
	
	
	
	
section _end
	kernel_end: