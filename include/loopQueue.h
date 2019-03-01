#ifndef LOOP_QUEUE_H
#define LOOP_QUEUE_H


//====================================================================================
/*
	外部汇编过程声明
*/
//====================================================================================

//====================================================================================
/*
	外部C函数声明
*/
//====================================================================================

LoopQueue* makeLoopQueue(Uint32 size);

void loopQueue_push(LoopQueue* loopQueue, Byte value);

Byte loopQueue_front(LoopQueue* loopQueue);

Byte loopQueue_pop(LoopQueue* loopQueue);

Uint32 loopQueue_isEmpty(LoopQueue* loopQueue);

Uint32 loopQueue_isFull(LoopQueue* loopQueue);





LoopQueue* makeLoopQueue(Uint32 size) {
    LoopQueue* p = (LoopQueue*)mallocKernel(sizeof(LoopQueue));
    p->scanCode = (Byte*)mallocKernel(sizeof(Byte) * size);
    p->front = 0;
    p->back = 0;
    p->size = 0;
    p->maxSize = size;
    return p;
}

void loopQueue_push(LoopQueue* loopQueue, Byte value) {
    if (loopQueue_isFull(loopQueue) == 0) {
        loopQueue->scanCode[loopQueue->back] = value;
        loopQueue->back = (loopQueue->back + 1) % loopQueue->maxSize;
        (loopQueue->size)++;
    }
    else {
        _asm_puts("\nLoopQueue full\n");
        HLT;
    }
}

Byte loopQueue_front(LoopQueue* loopQueue) {
    if (loopQueue_isEmpty(loopQueue) == 0){
        return loopQueue->scanCode[loopQueue->front];
    }
    else {
        _asm_puts("\nLoopQueue empty\n");
        HLT;
        return '\0';
    }
}

Byte loopQueue_pop(LoopQueue* loopQueue) {
     if (loopQueue_isEmpty(loopQueue) == 0){
        Byte value = loopQueue->scanCode[loopQueue->front];
        loopQueue->front = (loopQueue->front + 1) % loopQueue->maxSize;
        (loopQueue->size)--;
        return value;
    }
    else {
        _asm_puts("\nLoopQueue empty\n");
        HLT;
        return '\0';
    }
}

Uint32 loopQueue_isEmpty(LoopQueue* loopQueue) {
    return (loopQueue->size == 0 ? 1 : 0);
}

Uint32 loopQueue_isFull(LoopQueue* loopQueue) {
    return (((loopQueue->back + 1) % loopQueue->maxSize) == loopQueue->front) ? 1 : 0;
}


#endif
