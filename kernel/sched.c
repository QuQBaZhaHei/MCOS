#include "final.h"
#include "type.h"

extern Pcb* runPcb;
extern Pcb* readyPcbHead;
extern Pcb* readyPcbNext;
extern Tss TSS;
extern Uint32 tssSel;

extern void removeReadyPcb(Pcb* pcb);

extern void appendReadyPcb(Pcb* pcb);

extern void _asm_puts(char* str);

extern void putHex(Uint32 number);


int sched(Pcb** pcu, Pcb** pnext, Tcb** tcu, Tcb** tnext, Tss** tss , Uint32* ptssSel) {
	*ptssSel = tssSel;
	*tss = &TSS;
	if (runPcb->tcbCur->next == NULL) {
		runPcb->tcbCur == runPcb->tcbHead;

		if (runPcb->nextPCB == NULL) {
			readyPcbNext = readyPcbHead;
		}
		else {
			readyPcbNext = runPcb->nextPCB;
		}

		Pcb* curp = runPcb;
		*pnext = readyPcbNext;
		*pcu = runPcb;
		*tcu = runPcb->tcbCur;
		*tnext = (*pnext)->tcbHead;
		(*pnext)->tcbCur = (*pnext)->tcbHead;;
		runPcb->tcbCur = runPcb->tcbHead;
		runPcb = readyPcbNext;

		removeReadyPcb(curp);
		appendReadyPcb(curp);

		return 0;
	}
	else {
		Tcb* curt = runPcb->tcbCur;
		*tnext = curt->next;
		*tcu = curt;
		runPcb->tcbCur = curt->next;
		return 1;
	}
}