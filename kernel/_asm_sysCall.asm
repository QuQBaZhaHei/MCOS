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
				
		
;================================================================		

	global _asm_readDisk
	
	_asm_readDisk:
		;extern void _asm_readDisk(Byte* buffer,Uint32 diskSector);
		
		push ebp
		push edi
		push esi
		
		mov ebp,esp
		
		mov dx,0x1f2
		mov al,0x01
		out dx,al
		
		inc dx
		;取得扇区号
		mov eax,[ebp+20]
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
		
		
		.wait_read_disk:
			in al,dx
			and al,1000_1000b
			cmp al,0000_1000b
			jnz .wait_read_disk
			
		mov ecx,256
		mov dx,0x1f0
		mov esi,[ebp+16]
		
		.read_disk:
			in ax,dx
			mov [esi],ax
			add esi,2
			loop .read_disk
		
		pop esi
		pop edi
		pop ebp
		ret
		
;================================================================

	global _asm_writeDisk
	
	_asm_writeDisk:
		;extern void _asm_writeDisk(Byte* buffer,Uint32 diskSector);
		
		push ebp
		push edi
		push esi
		
		mov ebp,esp
		
		mov dx,0x1f2
		mov al,0x01
		out dx,al
		
		inc dx
		;取得扇区号
		mov eax,[ebp+20]
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
		mov al,0x30
		out dx,al
		
		
		.wait_wirte_disk:
			in al,dx
			and al,1000_1000b
			cmp al,0000_1000b
			jnz .wait_wirte_disk
			
		mov ecx,256
		mov dx,0x1f0
		mov esi,[ebp+16]
		
		.wirte_disk:
			mov ax,[esi]
			out dx,ax
			add esi,2
			loop .wirte_disk
		
		pop esi
		pop edi
		pop ebp
		ret


;================================================================

;================================================================		

	global _asm_sreadDisk
	
	_asm_sreadDisk:
		;extern void _asm_sreadDisk(Byte* buffer,Uint32 diskSector);
		
		push ebp
		push edi
		push esi
		
		mov ebp,esp
		
		mov dx,0x1f2
		mov al,0x01
		out dx,al
		
		inc dx
		;取得扇区号
		mov eax,[ebp+20]
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
		
		
		.wait_sread_disk:
			int 0x70
			in al,dx
			and al,1000_1000b
			cmp al,0000_1000b
			jnz .wait_sread_disk
			
		mov ecx,256
		mov dx,0x1f0
		mov esi,[ebp+16]
		
		.sread_disk:
			in ax,dx
			mov [esi],ax
			add esi,2
			loop .sread_disk
		
		pop esi
		pop edi
		pop ebp
		ret
		
;================================================================

	global _asm_swriteDisk
	
	_asm_swriteDisk:
		;extern void _asm_swriteDisk(Byte* buffer,Uint32 diskSector);
		
		push ebp
		push edi
		push esi
		
		mov ebp,esp
		
		mov dx,0x1f2
		mov al,0x01
		out dx,al
		
		inc dx
		;取得扇区号
		mov eax,[ebp+20]
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
		mov al,0x30
		out dx,al
		
		
		.wait_swirte_disk:
			int 0x70
			in al,dx
			and al,1000_1000b
			cmp al,0000_1000b
			jnz .wait_swirte_disk
			
		mov ecx,256
		mov dx,0x1f0
		mov esi,[ebp+16]
		
		.swirte_disk:
			mov ax,[esi]
			out dx,ax
			add esi,2
			loop .swirte_disk
		
		pop esi
		pop edi
		pop ebp
		ret


;================================================================
	
	
	global _asm_puts
	_asm_puts:
		;extern void _asm_putS(Byte* str);
		push esi
		push ebp
		
		mov ebp,esp
		
		xor ecx,ecx
		mov esi,[ebp+12]
		
		.scan_string:
			mov cl,[esi]
			or cl,cl
			jz .return
			push ecx
			call _asm_putChar
			add esp,4
			inc esi
			jmp .scan_string
			
			
		.return:
			pop ebp
			pop esi
			ret
			
			
;================================================================
	
	global _asm_putChar
	
	_asm_putChar:
		;extern void _asm_putChar(char c);
		
		push ebx
		push esi
		push edi
		
		mov ecx,[esp+16]
		
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
			jne .is_0x0a
			jmp .enter
			
			.is_0x0a:
				cmp cl,0x0a
				jne .put_other
			
			.enter:
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
		pop ebx
		ret		


;================================================================


global _asm_putCharC
	
	_asm_putCharC:
		;extern void _asm_putCharc(char c, char f);
		
		push ebx
		push esi
		push edi
		
		mov ecx,[esp+16]
		mov ch,[esp+20]
		
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
		
		.put_enterC:
			cmp cl,0x0d
			jne .is_0x0aC
			jmp .enterC
			
			.is_0x0aC:
				cmp cl,0x0a
				jne .put_otherC
			
			.enterC:
				mov eax,ebx
				xor edx,edx
				mov ebx,80
				div ebx
				inc eax
				xor edx,edx
				mul ebx
				mov ebx,eax
				jmp .roll_screenC
			
		.put_otherC:
			shl ebx,1
			mov [final_mem_video_adress+ebx],cx
			shr ebx,1
			inc ebx
			
		.roll_screenC:
			cmp ebx,2000
			jl .set_cursorC
			
			cld
			
			mov esi,final_mem_video_adress+0xa0
			mov edi,final_mem_video_adress
			mov ecx,1920
			rep movsd
			
		mov ebx,3840
		mov ecx,80
		
		.clean_last_lineC:
			mov word [final_mem_video_adress+ebx],0x0720
			add ebx,2
			loop .clean_last_lineC
			
			mov ebx,1920
		
		.set_cursorC:
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
		pop ebx
		ret		
		
	
		
		
		
		
		
		
		
		
		
		
		
		
		
		
			