
char str[] = "\nHello\n";

int main(void) {
	while (1) {
		__asm__ volatile (
			"movl $1,%%eax;"
			"movl %0,%%ebx;"
			"int $0x80;"
			:
			:"r"(str)
			:"%eax","%ebx"
		);
		for (unsigned i = 0; i < 9999999; i++);
	}
}