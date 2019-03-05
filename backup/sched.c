#include "final.h"
#include "type.h"

extern LoopLinkList* readyPcbQueue;
extern LinkListNode* curPcbNode;
extern Pcb* runPcb;
extern Tcb* runTcb;
extern Tss TSS;
extern Uint32 tssSel;

extern void eraseReadyPcb(Pcb* pcb);

extern void appendReadyPcb(Pcb* pcb);

extern void eraseReadyTcb(Pcb* pcb, Tcb* tcb);

extern void appendReadyTcb(Pcb* pcb, Tcb* tcb);

extern LinkListNode* loopLinkList_getHead(LoopLinkList* list);

extern void _asm_puts(char* str);

extern void putHex(Uint32 number);


int sched(Pcb** curPcb, Pcb** nextPcb, Tcb** curTcb, Tcb** nextTcb, Tss** tss , Uint32* ptssSel) {
	*ptssSel = tssSel;
	*tss = &TSS;

	Pcb* ppcb = (Pcb*)(curPcbNode->data);
	Tcb* ptcb = NULL;

	if (ppcb->curTcbNode->next != loopLinkList_getHead(ppcb->readyTcbQueue)) {
		//执行线程切换
		*curTcb = (Tcb*)(ppcb->curTcbNode->data);
		*nextTcb = (Tcb*)(ppcb->curTcbNode->next->data);
		(*curTcb)->state = TCB_STATE_READY;
		(*nextTcb)->state = TCB_STATE_RUN;
		runTcb = *nextTcb;

		ppcb->curTcbNode = ppcb->curTcbNode->next;

		//_asm_puts("\nthread\n");
		//_asm_puts((*curTcb)->name);
		//_asm_puts((*nextTcb)->name);

		return 1;
	}
	else {
		//执行进程切换
		*curPcb = ppcb;
		ppcb->state = PCB_STATE_READY;
		//ppcb->curTcbNode = loopLinkList_getHead(ppcb->readyTcbQueue);
		*curTcb = (Tcb*)(ppcb->curTcbNode->data);
		(*curTcb)->state = TCB_STATE_READY;

		ppcb = (Pcb*)(curPcbNode->next->data);
		*nextPcb = ppcb;
		ppcb->curTcbNode = loopLinkList_getHead(ppcb->readyTcbQueue);
		*nextTcb = (Tcb*)(ppcb->curTcbNode->data);
		runPcb = *nextPcb;
		runTcb = *nextTcb;

		curPcbNode = curPcbNode->next;

		//_asm_puts("\nprocess\n");
		//_asm_puts((*curTcb)->name);
		//_asm_puts((*nextTcb)->name);

		return 0;
	}
}