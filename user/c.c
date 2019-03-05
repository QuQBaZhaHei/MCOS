#include "final.h"
#include "type.h"

int main(void) {
	char buffer[512];
	char str[] = "test";
	struct FILE* fp;
	__asm__ volatile (
		"movl $8,%%eax;"
		"int $0x80;"
		:
		:"b"(&fp),"c"(str),"d"(0)
		:"%eax"
	);

	__asm__ volatile (
		"movl $2,%%eax;"
		"int $0x80;"
		:
		:"b"(fp)
		:"%eax","%edi"
	);

	buffer[0] = '\n';
	buffer[1] = '2';
	buffer[2] = '\n';

	__asm__ volatile (
		"movl $10,%%eax;"
		"movl $3,%%edi;"
		"int $0x80;"
		:
		:"b"(fp),"c"(buffer),"d"(0)
		:"%eax","%edi"
	);

	buffer[0] = 0;
	buffer[1] = 0;
	buffer[2] = 0;

	__asm__ volatile (
		"movl $9,%%eax;"
		"movl $1,%%edi;"
		"int $0x80;"
		:
		:"b"(fp),"c"(buffer),"d"(0)
		:"%eax","%edi"
	);

	buffer[3] = 0;

	__asm__ volatile (
		"movl $1,%%eax;"
		"int $0x80;"
		:
		:"b"(buffer)
		:"%eax","%edi"
	);
	while(1);
}