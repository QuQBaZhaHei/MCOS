#ifndef HEASH_TABLE_H
#define HEASH_TABLE_H

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


struct HashTable* makeHashTable(Uint32 size, _hashCallBack hash);

void hashTable_insertHashNode(HashNodeHead* hashNodeHead, HashNode* hashNode);

void hashTable_eraseHashNode(HashNodeHead* hashNodeHead, HashNode* hashNode);

void hashTable_insert(HashTable* hashTable, char* str, Uint32 value);

Uint32 hashTable_find(HashTable* hashTable, char* str);

void hashTable_erase(HashTable* hashTable, char* str);



struct HashTable* makeHashTable(Uint32 size, _hashCallBack hash) {
    HashTable* hashTable = (HashTable*)mallocKernel(sizeof(HashTable));
    hashTable->data = (HashNodeHead*)mallocKernel(sizeof(HashNodeHead) * size);
    hashTable->size = 0;
    hashTable->hash = hash;
    for (Uint32 i = 0; i < size; i++) {
        hashTable->data[i].head = NULL;
        hashTable->data[i].size = 0;
    }
    return hashTable;
}

void hashTable_insertHashNode(HashNodeHead* hashNodeHead, HashNode* hashNode) {
    HashNode* p =hashNodeHead->head;
    hashNodeHead->size += 1;
    hashNodeHead->head = hashNode;
    hashNode->next = p;
}

void hashTable_eraseHashNode(HashNodeHead* hashNodeHead, HashNode* hashNode) {
    if (hashNodeHead->head == hashNode) {
        hashNodeHead->head = hashNode->next;
    }
    else {
        HashNode* p =hashNodeHead->head;
        while (p->next != hashNode && p->next != NULL) {
            p = p->next;
        }
        if (p->next != NULL) {
            p->next = p->next->next;
        }
        else {
            _asm_puts("\nHashNode not exsit\n");
            HLT;
        }
    }
}

void hashTable_insert(HashTable* hashTable, char* str, Uint32 value) {
    HashNode* hashNode = (HashNode*)mallocKernel(sizeof(HashNode));
    strcpy(hashNode->fileName, str);
    hashNode->fcbIndex = value;
    HashNodeHead* hashNodeHead = &(hashTable->data[hashTable->hash(str)]);
    hashTable_insertHashNode(hashNodeHead, hashNode);
    (hashTable->size)++;
}

Uint32 hashTable_find(HashTable* hashTable, char* str) {
    Uint32 index = hashTable->hash(str);
    if (hashTable->data[index].size == 0) {
        return 0;
    }
    else {
        HashNode* p =hashTable->data[index].head;
        while(p != NULL && strcmp(p->fileName, str) != 0) {
            p = p->next;
        }
        if (p != NULL) {
            return p->fcbIndex;
        }
        else {
            return 0;
        }
    }
}

void hashTable_erase(HashTable* hashTable, char* str) {
    Uint32 index = hashTable->hash(str);
    if (hashTable->data[index].size == 0) {
        _asm_puts("\nHashNodeHead size is zero\n");
        HLT;
    }
    else {
        HashNode* p = hashTable->data[index].head;
        while (p != NULL && strcmp(p->fileName, str) != 0) {
            p = p->next;
        }
        if (p != NULL) {
            hashTable_eraseHashNode(&(hashTable->data[index]), p);
        }
        else {
            _asm_puts("\nHashNode not exist\n");
            HLT;
        }
    }
    (hashTable->size)--;
}


















#endif
