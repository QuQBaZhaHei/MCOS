#if !defined(LOOP_LINKE_LIST_H)
#define LOOP_LINKE_LIST_H

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


LoopLinkList* makeLoopLinkList();

//添加元素到链表尾部
int loopLinkList_pushBack(LoopLinkList* list, void* node);

//添加元素到链表头部
int loopLinkList_pushFront(LoopLinkList* list, void* node);

//删除末尾元素
int loopLinkList_popBack(LoopLinkList* list);

//删除头部元素
int loopLinkList_popFront(LoopLinkList* list);

//获得头节点
LinkListNode* loopLinkList_getHead(LoopLinkList* list);

//获得尾节点
LinkListNode* loopLinkList_getBack(LoopLinkList* list);

//删除节点
int loopLinkList_erase(LoopLinkList* list, void* node);

//插入节点
int loopLinkList_insert(LoopLinkList* list, void* node);

//查找节点
LinkListNode* loopLinkList_find(LoopLinkList* list, void* node);


LoopLinkList* makeLoopLinkList() {
	LoopLinkList* list = (LoopLinkList*)mallocKernel(sizeof(LoopLinkList));
	list->front = NULL;
	list->back = NULL;
	list->size = 0;
	return list;
}

int loopLinkList_pushBack(LoopLinkList* list, void* node) {
	LinkListNode* p = (LinkListNode*)mallocKernel(sizeof(LinkListNode));
	LinkListNode* q = NULL;

	p->data = node;

	if (node == NULL || list ==  NULL) {
		return 2;
	}

	if (list->size == 0) {
		list->front = p;
		list->back = p;
		p->prev = p;
		p->next = p;
	}
	else {
		list->back->next = p;
		p->prev = list->back;
		p->next = list->front;
		list->back = p;
	}

	++(list->size);
	return 0;
}

int loopLinkList_pushFront(LoopLinkList* list, void* node) {
	return loopLinkList_pushBack(list, node);
}

int loopLinkList_popBack(LoopLinkList* list) {
	if (list == NULL) {
		return 2;
	}

	if (list->size != 0) {
		if (list->size == 1) {
			//直接清空链表
			list->front = NULL;
			list->back = NULL;
		}
		else {
			//改变尾节点指针到前一个节点
			list->back = list->back->prev;
			//与链表头连接
			list->back->next = list->front;
			//链表头与尾节点连接
			list->front->prev = list->back;
		}
		--(list->size);
		return 0;
	}
	else {
		return 1;
	}
}

int loopLinkList_popFront(LoopLinkList* list) {
	if (list == NULL) {
		return 2;
	}

	if (list->size != 0) {
		if (list->size == 1) {
			//直接清空链表
			list->front = NULL;
			list->back = NULL;
		}
		else {
			//改变头节点为前一个节点
			list->front = list->front->next;
			//与链表尾连接
			list->front->prev = list->back;
			//链表尾与链表头相连
			list->back->next = list->front;
		}
		--(list->size);
		return 0;
	}
	else {
		return 1;
	}
}

LinkListNode* loopLinkList_getHead(LoopLinkList* list) {
	if (list == NULL) {
		return (LinkListNode*)2;
	}

	return list->front;
}

LinkListNode* loopLinkList_getBack(LoopLinkList* list) {
	if (list == NULL) {
		return (LinkListNode*)2;
	}

	return list->back;
}

int loopLinkList_erase(LoopLinkList* list, void* node) {
	LinkListNode* p = list->front;
	LinkListNode* q = NULL;
	LinkListNode* r = NULL;

	if (node == NULL || list ==  NULL) {
		return 2;
	}
	
	if (list->size == 0) {
		return 1;
	}

	if (list->size == 1) {
		//直接清空链表
		loopLinkList_popBack(list);
		return 0;
	}

	//寻找要被删除的节点
	while (p->data != node && p->next != NULL) {
		p = p->next;
	}
	//如果节点存在
	if (p->data == node) {
		if (list->front == p) {
			list->front = p->next;
			list->front->prev = list->back;
			list->back->next = list->front;
		}
		else if (list->back == p) {
			list->back = p->prev;
			list->back->next = list->front;
			list->front->prev = list->back;
		}
		else {
			//前一个节点与后一个节点连接
			p->prev->next = p->next;
			//后一个节点与前一个节点连接
			p->next->prev = p->prev;
		}
		--(list->size);
		return 0;
	}
	else {
		//要删除的节点不存在
		return 1;
	}
}

int loopLinkList_insert(LoopLinkList* list, void* node) {
	LinkListNode* p = list->front;
	LinkListNode* q = (LinkListNode*)mallocKernel(sizeof(LinkListNode));
	LinkListNode* r = NULL;


	if (node == NULL || list == NULL) {
		return 2;
	}

	if (list->size == 0) {
		return loopLinkList_pushFront(list, node);
	}

	//寻找要插入的位置的节点
	while (p->data != node && p->next != NULL) {
		p = p->next;
	}

	//如果节点存在
	if (p->data == node) {
		q->prev = p->prev;
		q->next = p;
		p->prev->next = q;
		p->prev = q;
		++(list->size);
		return 0;
	}
	else {
		return 1;
	}
}

LinkListNode* loopLinkList_find(LoopLinkList* list, void* node) {
	LinkListNode* p = list->front;

	if (node == NULL || list ==  NULL) {
		return (LinkListNode*)2;
	}

	while (p->data != node && p->next != NULL) {
		p = p->next;
	}

	return p;
}















#endif // LOOP_LINKE_LIST_H



