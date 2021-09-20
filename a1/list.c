#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "list.h"

typedef struct ListNode_s ListNode;
struct ListNode_s {
    List* head;
    List* tail;
    int size;
};

static ListNode listNodes;
static List freeNodes;
static Node nodes[LIST_MAX_NUM_NODES];
static List lists[LIST_MAX_NUM_HEADS];

static bool ready = false;
static void initialiseFreeArays();
static void freeHead(int index);
static int findFreeHead();
static int findFreeNode();
static void freeNode(int index);


// Makes a new, empty list, and returns its reference on success. 
// Returns a NULL pointer on failure.
List* List_create(){

    if(ready == false){
        initialiseFreeArays();
        ready = true;
    }
    
    if(listNodes.size != 0){

        int index = findFreeHead();

        List* pList = &lists[index];
        pList->size = 0;
	pList->head = NULL;
	pList->listCurrent = NULL;
        pList->ListOutOfBounds = LIST_OOB_FALSE;
	pList->tail = NULL;
        pList->index = index;
        pList->next = NULL;
        return pList;
    }
    return NULL;
}

void initialiseFreeArays(){

    for(int i = 0; i < LIST_MAX_NUM_HEADS; i++){
        List* list = &lists[i];
        list->index = i;

        if(i < LIST_MAX_NUM_HEADS){
            list->next = &lists[i+1];
        }

    }
    listNodes.head = &lists[0];
    listNodes.tail = &lists[LIST_MAX_NUM_HEADS-1];
    listNodes.size = LIST_MAX_NUM_HEADS;
   
    for(int i = 0; i < LIST_MAX_NUM_NODES; i++){
        Node* node = &nodes[i];
        node->index = i;

        if(i < LIST_MAX_NUM_NODES){
            node->next = &nodes[i+1];
        }

    }
    freeNodes.head = &nodes[0];
    freeNodes.tail = &nodes[LIST_MAX_NUM_NODES-1];
    freeNodes.size = LIST_MAX_NUM_NODES;
}

int findFreeHead(){

    if(listNodes.size == 0){
        return -1;
    }
    else{

        int index = listNodes.head->index;
        listNodes.head = listNodes.head->next;
        listNodes.size--;
        return index;      
    } 
}

int findFreeNode(){

    if(freeNodes.size == 0){
        return -1;
    }
    else{

        int index = freeNodes.head->index;
        freeNodes.head = freeNodes.head->next;
        freeNodes.size--;
        return index;      
    }
}

void freeNode(int index){

    if(freeNodes.size == LIST_MAX_NUM_NODES){
        return;
    }
    else{
    
        Node* node = &nodes[index];
        node->curr = NULL;
        node->prev = NULL;
        node->next = NULL;
        if(freeNodes.size == 0){
            freeNodes.head = node;
            freeNodes.tail = node;
        }
        else{
            freeNodes.tail->next = node;
            freeNodes.tail = node;
        }

        freeNodes.size++;
    }

}

void freeHead(int index){
    
    if(listNodes.size == LIST_MAX_NUM_HEADS){
        return;
    }
    else{
    
        List* list = &lists[index];
        list->next = NULL;
        if(listNodes.size == 0){
            listNodes.head = list;
            listNodes.tail = list;
        }
        else{
            listNodes.tail->next = list;
            listNodes.tail = list;
        }
        listNodes.size++;
    }
}




// Returns the number of items in pList.
int List_count(List* pList){
    return pList->size;
}

// Returns a pointer to the first item in pList and makes the first item the current item.
// Returns NULL and sets current item to NULL if list is empty.
void* List_first(List* pList){
    if(pList->size == 0){
        pList->listCurrent = NULL;
        pList->ListOutOfBounds = LIST_OOB_FALSE;
        return NULL;
    }
    pList->listCurrent = pList->head;
    return pList->listCurrent->curr;
}

// Returns a pointer to the last item in pList and makes the last item the current item.
// Returns NULL and sets current item to NULL if list is empty.
void* List_last(List* pList){
    if(pList->size == 0){
        pList->listCurrent = NULL;
        pList->ListOutOfBounds = LIST_OOB_FALSE;
        return NULL;
    }
    pList->listCurrent = pList->tail;
    return pList->listCurrent->curr;

}

// Advances pList's current item by one, and returns a pointer to the new current item.
// If this operation advances the current item beyond the end of the pList, a NULL pointer 
// is returned and the current item is set to be beyond end of pList.
void* List_next(List* pList){

    if(pList->size == 0 || pList->ListOutOfBounds == LIST_OOB_END){
        return NULL;
    }  

    else if( pList->ListOutOfBounds == LIST_OOB_START){
        pList->listCurrent = pList->head;
        pList->ListOutOfBounds = LIST_OOB_FALSE;
        return pList->listCurrent->curr;
    }

    else{

        if(pList->listCurrent != pList->tail){
            pList->listCurrent = pList->listCurrent->next;
            return pList->listCurrent->curr;
        }
       else{
            pList->listCurrent = NULL;
            pList->ListOutOfBounds = LIST_OOB_END;
            return NULL;
        }
    }

}

// Backs up pList's current item by one, and returns a pointer to the new current item. 
// If this operation backs up the current item beyond the start of the pList, a NULL pointer 
// is returned and the current item is set to be before the start of pList.
void* List_prev(List* pList){
    

    if(pList->size == 0 || pList->ListOutOfBounds == LIST_OOB_START){
        return NULL;
    }  

    else if( pList->ListOutOfBounds == LIST_OOB_END){
        pList->listCurrent = pList->tail;
        pList->ListOutOfBounds = LIST_OOB_FALSE;
        return pList->listCurrent->curr;
    }

    else{

        if(pList->listCurrent != pList->head){
            pList->listCurrent = pList->listCurrent->prev;
            return pList->listCurrent->curr;
        }
       else{
            pList->listCurrent = NULL;
            pList->ListOutOfBounds = LIST_OOB_START;
            return NULL;
        }
    }

}
// Returns a pointer to the current item in pList.
void* List_curr(List* pList){
    if(pList->listCurrent != NULL){
        return pList->listCurrent->curr;
    }
    return NULL;
}

// Adds the new item to pList directly after the current item, and makes item the current item. 
// If the current pointer is before the start of the pList, the item is added at the start. If 
// the current pointer is beyond the end of the pList, the item is added at the end. 
// Returns 0 on success, -1 on failure.
int List_add(List* pList, void* pItem){

    if(freeNodes.size  != 0){

        int index = findFreeNode();
        Node* node = &nodes[index];
        node->curr = pItem;
        node->next = NULL;
        node->prev = NULL;

        if(pList->size == 0){
            pList->head = node;
            pList->tail = node;
            pList->size++;
            pList->listCurrent = node;
            pList->ListOutOfBounds = LIST_OOB_FALSE;
            return 0;
        }
        else{
            if(pList->ListOutOfBounds == LIST_OOB_START){
                node->next = pList->head;
                node->next->prev = node;
                pList->head = node;
                pList->size++;
                pList->listCurrent = node;
                pList->ListOutOfBounds = LIST_OOB_FALSE;
                return 0;
            }
            else if(pList->ListOutOfBounds == LIST_OOB_END || pList->listCurrent == pList->tail){
                node->prev = pList->tail;
                node->prev->next = node;
                pList->tail = node;
                pList->size++;
                pList->listCurrent = node;
                pList->ListOutOfBounds = LIST_OOB_FALSE;
                return 0;
            }
            else{
                node->next = pList->listCurrent->next;  
                pList->listCurrent->next = node;
                node->prev = pList->listCurrent;
                node->next->prev = node;
                pList->size++;
                pList->listCurrent = node;
                pList->ListOutOfBounds = LIST_OOB_FALSE;
                return 0;
            }
        }
    }
    return -1;
}

// Adds item to pList directly before the current item, and makes the new item the current one. 
// If the current pointer is before the start of the pList, the item is added at the start. 
// If the current pointer is beyond the end of the pList, the item is added at the end. 
// Returns 0 on success, -1 on failure.
int List_insert(List* pList, void* pItem){


    if(freeNodes.size  != 0){

        int index = findFreeNode();
        Node* node = &nodes[index];
        node->curr = pItem; 
        node->next = NULL;
        node->prev = NULL;

        if(pList->size == 0){
            pList->head = node;
            pList->tail = node;
            pList->size++;
            pList->listCurrent = node;
            pList->ListOutOfBounds = LIST_OOB_FALSE;
            return 0;
        }
        else{
            if(pList->ListOutOfBounds == LIST_OOB_START || pList->listCurrent == pList->head){
                node->next = pList->head;
                node->next->prev = node;
                pList->head = node;
                pList->size++;
                pList->listCurrent = node;
                pList->ListOutOfBounds = LIST_OOB_FALSE;
                return 0;
            }
            else if(pList->ListOutOfBounds == LIST_OOB_END ){
                node->prev = pList->tail;
                node->prev->next = node;
                pList->tail = node;
                pList->size++;
                pList->listCurrent = node;
                pList->ListOutOfBounds = LIST_OOB_FALSE;
                return 0;
            }
            else{
                node->next = pList->listCurrent;
                node->prev = pList->listCurrent->prev;
                pList->listCurrent->prev->next = node;
                pList->listCurrent->prev = node; 
                pList->size++;
                pList->listCurrent = node;
                pList->ListOutOfBounds = LIST_OOB_FALSE;
                return 0;
            }
        }
    }    
    return -1;
}

// Adds item to the end of pList, and makes the new item the current one. 
// Returns 0 on success, -1 on failure.
int List_append(List* pList, void* pItem){

    if(freeNodes.size  != 0){

        int index = findFreeNode();
        Node* node = &nodes[index];
        node->curr = pItem;
        node->index = index; 
        node->next = NULL;
        node->prev = NULL;

        if(pList->size == 0){
            pList->head = node;
            pList->tail = node;
            pList->size++;
            pList->listCurrent = node;
            pList->ListOutOfBounds = LIST_OOB_FALSE;
            return 0;
        }
        else{
            node->prev = pList->tail;
            node->prev->next = node;
            pList->tail = node;
            pList->size++;
            pList->listCurrent = node;
            pList->ListOutOfBounds = LIST_OOB_FALSE;
            return 0;
        }
    }
    return -1;

}

// Adds item to the front of pList, and makes the new item the current one. 
// Returns 0 on success, -1 on failure.
int List_prepend(List* pList, void* pItem){

    if(freeNodes.size  != 0){

        int index = findFreeNode();
        Node* node = &nodes[index];
        node->curr = pItem;
        node->index = index; 
        node->next = NULL;
        node->prev = NULL;

        if(pList->size == 0){
            pList->head = node;
            pList->tail = node;
            pList->size++;
            pList->listCurrent = node;
            pList->ListOutOfBounds = LIST_OOB_FALSE;
            return 0;
        }
        else{
            
            node->next = pList->head;
            node->next->prev = node;
            pList->head = node;
            pList->size++;
            pList->listCurrent = node;
            pList->ListOutOfBounds = LIST_OOB_FALSE;
            return 0;
        }
    }
    return -1;

}

// Return current item and take it out of pList. Make the next item the current one.
// If the current pointer is before the start of the pList, or beyond the end of the pList,
// then do not change the pList and return NULL.
void* List_remove(List* pList){

    Node* node = pList->listCurrent;
    void* curr = node->curr;

    if(pList->listCurrent == NULL || pList->size == 0){
        return NULL;
    }
    else{
        Node *prevNode = pList->listCurrent->prev;
        Node *nextNode = pList->listCurrent->next;

        if(prevNode == NULL || nextNode == NULL){

            if( pList->listCurrent == pList->tail && pList->listCurrent == pList->head){
                pList->head = NULL;
                pList->tail = NULL;
                pList->listCurrent = NULL;
                pList->size--;
                freeNode(node->index);
                return curr;
            }
            else if(pList->listCurrent == pList->head){
                pList->head = pList->head->next;
                pList->head->prev = NULL;
                pList->listCurrent = pList->head;
                pList->size--;
                freeNode(node->index);
                return curr;
            }

            else if(pList->listCurrent == pList->tail){
                pList->tail = pList->tail->prev;
                pList->tail->next = NULL;
                pList->listCurrent = pList->tail;
                pList->size--;
                freeNode(node->index);
                return curr;
            }

        }
        else{
            pList->listCurrent->prev->next = pList->listCurrent->next;
            pList->listCurrent->next->prev = pList->listCurrent->prev;
            pList->listCurrent = pList->listCurrent->next;
            pList->size--;
            freeNode(node->index);
            return curr;

        }

    }
}

// Adds pList2 to the end of pList1. The current pointer is set to the current pointer of pList1. 
// pList2 no longer exists after the operation; its head is available
// for future operations.
void List_concat(List* pList1, List* pList2){

    if(pList2->size > 0){

        if(pList1->size > 0){
           
            pList1->tail->next = pList2->head;
            pList2->head->prev = pList1->tail;
            pList1->tail = pList2->tail;
            pList1->size = pList1->size + pList2->size;
            pList2->head = NULL;
            pList2->tail = NULL;
            pList2->listCurrent = NULL;
            pList2->size = 0;
            pList2->ListOutOfBounds = LIST_OOB_FALSE;
            freeHead(pList2->index);
        }
        else{

            pList1->tail = pList2->tail;
            pList1->head = pList2->head;
            pList1->size = pList2->size;
            pList2->head->prev = NULL;
            pList2->tail->next = NULL;
            pList2->head = NULL;
            pList2->tail = NULL;
            pList2->listCurrent = NULL;
            pList2->size = 0;
            pList2->ListOutOfBounds = LIST_OOB_FALSE;
            freeHead(pList2->index);
        }
    }
    else{
	    pList2->head = NULL;
	    pList2->tail = NULL;
	    pList2->listCurrent = NULL;
	    pList2->size = 0;
	    pList2->ListOutOfBounds = LIST_OOB_FALSE;
	    freeHead(pList2->index);
    }
}

// Delete pList. pItemFreeFn is a pointer to a routine that frees an item. 
// It should be invoked (within List_free) as: (*pItemFreeFn)(itemToBeFreedFromNode);
// pList and all its nodes no longer exists after the operation; its head and nodes are 
// available for future operations.
typedef void (*FREE_FN)(void* pItem);
void List_free(List* pList, FREE_FN pItemFreeFn){

    if(pList->size == 0){
        pList->head = NULL;
        pList->tail = NULL;
        pList->listCurrent = NULL;
        pList->size = 0;
        pList->ListOutOfBounds = LIST_OOB_FALSE;
        freeHead(pList->index);
    }
    else{
        Node* node = pList->head;
        while(node != NULL){
            Node *nextNode = node->next;
            void* itemToBeFreedFromNode = node->curr;          
            node->curr = NULL;
            node->next = NULL;
            node->prev = NULL;
            freeNode(node->index);
            (*pItemFreeFn)(itemToBeFreedFromNode);
            node = nextNode;
        }
        pList->head = NULL;
        pList->tail = NULL;
        pList->listCurrent = NULL;
        pList->size = 0;
        pList->ListOutOfBounds = LIST_OOB_FALSE;
        freeHead(pList->index);
    }
}

// Return last item and take it out of pList. Make the new last item the current one.
// Return NULL if pList is initially empty.
void* List_trim(List* pList){
    if(pList->size == 0){
        return NULL;
    }
    else{

        if(pList->head == pList->tail){
            Node* prevTail = pList->tail;
            pList->tail = NULL;
            pList->head = NULL;
            pList->listCurrent = NULL;
            pList->size = 0;
            pList->ListOutOfBounds = LIST_OOB_FALSE;
            freeNode(prevTail->index);
            return prevTail;
        }

        Node* node = pList->tail->prev;
        Node* prevTail = pList->tail;
        node->next = NULL;
        pList->tail = node;
        freeNode(prevTail->index);
        pList->ListOutOfBounds = LIST_OOB_FALSE;
        pList->listCurrent = pList->tail;
        pList->size--;
        return prevTail;
    }
}

// Search pList, starting at the current item, until the end is reached or a match is found. 
// In this context, a match is determined by the comparator parameter. This parameter is a
// pointer to a routine that takes as its first argument an item pointer, and as its second 
// argument pComparisonArg. Comparator returns 0 if the item and comparisonArg don't match, 
// or 1 if they do. Exactly what constitutes a match is up to the implementor of comparator. 
// 
// If a match is found, the current pointer is left at the matched item and the pointer to 
// that item is returned. If no match is found, the current pointer is left beyond the end of 
// the list and a NULL pointer is returned.
// 
// If the current pointer is before the start of the pList, then start searching from
// the first node in the list (if any).
typedef bool (*COMPARATOR_FN)(void* pItem, void* pComparisonArg);
void* List_search(List* pList, COMPARATOR_FN pComparator, void* pComparisonArg){
    
    Node* node = pList->listCurrent;

    if(pList->ListOutOfBounds == LIST_OOB_START){
        node = pList->head;
    }

    while(node != NULL){
        if((*pComparator)(node->curr, pComparisonArg) == 1){
            pList->listCurrent = node;
            return node->curr;
        }
        node = node->next;
    }

    pList->listCurrent = NULL;
    pList->ListOutOfBounds = LIST_OOB_END;
    return NULL;
}



