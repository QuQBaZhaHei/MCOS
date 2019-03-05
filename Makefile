kernel2 : kernel/_asm_kernelFun.asm kernel/_asm_sysCall.asm kernel/_asm_interrupt.asm kernel/kernel.c kernel/sched.c Makefile
	gcc -O2 -m32 -I include -c -Wno-strict-aliasing -fno-stack-protector -finput-charset=UTF-8 -o kernel/kernel.o kernel/kernel.c 
	gcc -O2 -m32 -I include -c -Wno-strict-aliasing -fno-stack-protector -finput-charset=UTF-8 -o kernel/sched.o kernel/sched.c
	nasm -f elf -o kernel/_asm_sysCall.o kernel/_asm_sysCall.asm
	nasm -f elf -o kernel/_asm_kernelFun.o kernel/_asm_kernelFun.asm
	nasm -f elf -o kernel/_asm_interrupt.o kernel/_asm_interrupt.asm
	ld -m elf_i386 -s -e main -Ttext 0x80010500 -o kernel/kernel.bin kernel/kernel.o kernel/_asm_sysCall.o kernel/_asm_kernelFun.o kernel/_asm_interrupt.o kernel/sched.o
	
kernel : kernel/_asm_kernelFun.asm kernel/_asm_sysCall.asm kernel/_asm_interrupt.asm kernel/kernel.c kernel/sched.c Makefile
	gcc -m32 -I include -c -Wno-strict-aliasing -fno-stack-protector -finput-charset=UTF-8 -o kernel/kernel.o kernel/kernel.c 
	gcc -m32 -I include -c -Wno-strict-aliasing -fno-stack-protector -finput-charset=UTF-8 -o kernel/sched.o kernel/sched.c
	nasm -f elf -o kernel/_asm_sysCall.o kernel/_asm_sysCall.asm
	nasm -f elf -o kernel/_asm_kernelFun.o kernel/_asm_kernelFun.asm
	nasm -f elf -o kernel/_asm_interrupt.o kernel/_asm_interrupt.asm
	ld -m elf_i386 -s -e main -Ttext 0x80010500 -o kernel/kernel.bin kernel/kernel.o kernel/_asm_sysCall.o kernel/_asm_kernelFun.o kernel/_asm_interrupt.o kernel/sched.o

kernel1 : kernel/_asm_kernelFun.asm kernel/_asm_sysCall.asm kernel/_asm_interrupt.asm kernel/kernel.c kernel/sched.c Makefile
	gcc -O1 -m32 -I include -c -Wno-strict-aliasing -fno-stack-protector -finput-charset=UTF-8 -o kernel/kernel.o kernel/kernel.c 
	gcc -O1 -m32 -I include -c -Wno-strict-aliasing -fno-stack-protector -finput-charset=UTF-8 -o kernel/sched.o kernel/sched.c
	nasm -f elf -o kernel/_asm_sysCall.o kernel/_asm_sysCall.asm
	nasm -f elf -o kernel/_asm_kernelFun.o kernel/_asm_kernelFun.asm
	nasm -f elf -o kernel/_asm_interrupt.o kernel/_asm_interrupt.asm
	ld -m elf_i386 -s -e main -Ttext 0x80010500 -o kernel/kernel.bin kernel/kernel.o kernel/_asm_sysCall.o kernel/_asm_kernelFun.o kernel/_asm_interrupt.o kernel/sched.o


kernel3 : kernel/_asm_kernelFun.asm kernel/_asm_sysCall.asm kernel/_asm_interrupt.asm kernel/kernel.c kernel/sched.c Makefile
	gcc -O3 -m32 -I include -c -Wno-strict-aliasing -fno-stack-protector -finput-charset=UTF-8 -o kernel/kernel.o kernel/kernel.c 
	gcc -O3 -m32 -I include -c -Wno-strict-aliasing -fno-stack-protector -finput-charset=UTF-8 -o kernel/sched.o kernel/sched.c
	nasm -f elf -o kernel/_asm_sysCall.o kernel/_asm_sysCall.asm
	nasm -f elf -o kernel/_asm_kernelFun.o kernel/_asm_kernelFun.asm
	nasm -f elf -o kernel/_asm_interrupt.o kernel/_asm_interrupt.asm
	ld -m elf_i386 -s -e main -Ttext 0x80010500 -o kernel/kernel.bin kernel/kernel.o kernel/_asm_sysCall.o kernel/_asm_kernelFun.o kernel/_asm_interrupt.o kernel/sched.o
	
mbr : boot/mbr.asm
	nasm -f bin -o boot/mbr.bin boot/mbr.asm
	
a : user/a.c Makefile
	gcc -m32 -I include -c -Wno-strict-aliasing -fno-stack-protector -finput-charset=UTF-8 -o user/a.o user/a.c 
	ld -m elf_i386 -s -e main -o user/a.bin user/a.o
	
b : user/b.c Makefile
	gcc -m32 -I include -c -Wno-strict-aliasing -fno-stack-protector -finput-charset=UTF-8 -o user/b.o user/b.c 
	ld -m elf_i386 -s -e main -o user/b.bin user/b.o