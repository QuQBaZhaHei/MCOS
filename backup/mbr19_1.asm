

		;内核加载起始地址
		final_mem_kernel_adress        equ 0x10000
		
		;页目录起始地址
		final_mem_page_adress        equ 0x0040_0000
		
		;内核加载扇区
		final_disk_kernel_disk_sector  equ 0x1
		
		;4GB数据段GDT选择西
		final_mem_gdt_4gb            equ 0x0008
		
		;4GB代码段描述符
		final_code_gdt_4gb           equ 0x0010
		
		;显示缓冲区选择子
		final_mem_gdt_video          equ 0x0018
		
		;内核栈段选择子
		final_mem_gdt_stack          equ 0x0020
		
		;缓冲区
		final_mem_buffer			 equ 0x0030_0000
		
		
section mbr vstart=0x7c00

	jmp start
		
	pgdt dw 0
		 dd 0x7e00

	
	start:
		mov ax,cs
		mov ds,ax
		mov es,ax
		mov ss,ax
		
		mov ebx,[pgdt+0x02]
		
		;4GB数据段描述符
		mov dword [ebx+0x08],0000_0000_0000_0000_1111_1111_1111_1111b
		mov dword [ebx+0x0c],0000_0000_1100_1111_1001_0010_0000_0000b
		
		;4GB代码段描述符
		mov dword [ebx+0x10],0000_0000_0000_0000_1111_1111_1111_1111b
		mov dword [ebx+0x14],0000_0000_1100_1111_1001_1000_0000_0000b

		;显示缓冲区描述符
		mov dword [ebx+0x18],1000_0000_0000_0000_1111_1111_1111_1111b
		mov dword [ebx+0x1c],0000_0000_0100_0000_1001_0010_0000_1011b
		
		;内核栈段描述符
		mov dword [ebx+0x20],0111_1100_0000_0000_1111_1111_1111_1110b
		mov dword [ebx+0x24],0000_0000_1100_1111_1001_0110_0000_0000b
		
		;修改GDT界限
		mov word [pgdt],39
		
		lgdt [pgdt]
		
		;打开A20
		in al,0x92
		or al,0000_0010b
		out 0x92,al
		
		cli
		
		;进入保护模式
		mov eax,cr0
		or eax,1
		mov cr0,eax
		
		
		jmp dword final_code_gdt_4gb:flush
		
		
	[bits 32]
		
		
	flush:
		mov eax,final_mem_gdt_4gb
		mov ds,eax
		mov es,eax
		mov gs,eax
		mov fs,eax
		mov ss,eax
		mov esp,0x7c00
		
		mov ecx,1024
		mov esi,final_mem_page_adress
		mov edi,0x0040_1000
		xor eax,eax
		
		.clear_page:
			mov dword [esi+eax*4],0x0000_0000
			mov dword [edi+eax*4],0x0000_0000
			inc eax
			loop .clear_page
		
			
		.set_page:
			mov ebx,final_mem_page_adress
			mov edx,ebx
			or edx,0x0000_0003
			mov [ebx+4092],edx
			mov edx,0x0040_1003
			mov [ebx],edx
			mov [ebx+0x800],edx
			
			mov ebx,0x0040_1000
			xor esi,esi
			xor eax,eax
			
			.set_kernel_page:
				mov edx,eax
				or edx,0x3
				mov [ebx+esi*4],edx
				add eax,0x1000
				inc esi
				cmp esi,1024
				jl .set_kernel_page
				
		
		mov eax,final_disk_kernel_disk_sector
		mov esi,final_mem_kernel_adress
		
		mov ecx,1300
		.read_kernel:
			call read_hard_disk
			add esi,512
			inc eax
			loop .read_kernel
				
		
		
				
				
		;进入分页模式
		mov eax,final_mem_page_adress
		mov cr3,eax
		
		mov eax,cr0
		or eax,1000_0000_0000_0000_0000_0000_0000_0000b
		mov cr0,eax
		
		or esp,0x8000_0000
		
		jmp 0x80010500
			
			
		
		
		
		
		
		
	read_hard_disk:
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
		ret
		
		
		times 510-($-$$) db 0
                         db 0x55,0xaa
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		