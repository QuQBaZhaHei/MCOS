#ifndef HEAP_H

#define HEAP_H

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


Heap* makeHeap(Uint32 size, _compareCallBack compare);

void heapify(Heap* heap, Uint32 index);

void heap_increaseKey(Heap* heap, Uint32 index, Drcb key);

void heap_insert(Heap* heap, Drcb key);

Drcb heap_getTop(Heap* heap);

Drcb heap_popTop(Heap* heap);



Heap* makeHeap(Uint32 size, _compareCallBack compare) {
	Heap* heap = mallocKernel(sizeof(Heap));
	heap->data = mallocKernel(size * sizeof(Drcb));
	heap->maxSize = size;
	heap->size = 0;
	heap->compare = compare;
	return heap;
}

void heapify(Heap* heap, Uint32 index) {
	if (index >= heap->size) {
		_asm_puts("\nheap index overbound\n");
		HLT;
	}
	else {
		Uint32 left = 2 * index;
		Uint32 right = 2 * index + 1;
		Uint32 largest = 0;
		if (left < heap->size  && heap->compare(heap->data[left], heap->data[index]) > 0) {
			largest = left;
		}
		else {
			largest = index;
		}
		if (right < heap->size  && heap->compare(heap->data[right], heap->data[largest]) > 0) {
			largest = right;
		}
		if (largest != index) {
			Drcb temp = heap->data[index];
			heap->data[index] = heap->data[largest];
			heap->data[largest] = temp;
			heapify(heap, largest);
		}
	}
}

void heap_increaseKey(Heap* heap, Uint32 index, Drcb key) {
	if (index >= heap->size) {
		_asm_puts("\nheap index overbound\n");
		HLT;
	}
	else {
		Drcb temp;
		heap->data[index] = key;
		while (index > 0 && heap->compare(heap->data[index / 2], heap->data[index]) < 0) {
			temp = heap->data[index];
			heap->data[index] = heap->data[index / 2];;
			heap->data[index / 2] = temp;
			index /= 2;
		}
		
	}
}

void heap_insert(Heap* heap, Drcb key) {
	(heap->size)++;
	heap_increaseKey(heap, heap->size - 1, key);
}

Drcb heap_getTop(Heap* heap) {
	if (heap->size == 0) {
		_asm_puts("\nheap no items\n");
		HLT;
	}
	else {
		return heap->data[0];
	}
}

Drcb heap_popTop(Heap* heap) {
	if (heap->size == 0) {
		_asm_puts("\nheap no items\n");
		HLT;
	}
	Drcb max = heap->data[0];
	heap->data[0] = heap->data[heap->size];
	heapify(heap, 0);
	(heap->size)--;
	return max;
}











#endif