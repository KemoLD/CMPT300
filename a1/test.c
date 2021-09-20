#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "list.h"

void print(List* pList);
void test_add();
void test_create();
void test_insert();
void test_first();
void test_last();
void test_next();
void test_prev();
void test_curr();
void test_remove();
void test_prepend();
void test_append();
void test_remove();
void test_concat();
void test_free();
void free(void* curr);
void test_trim();
bool equal(void* curr, void* pComparisonArg);
void test_search();

int one = 1;
int two = 2;
int three = 3;
int four = 4;
int five = 5;
void* a = &one;
void *b = &two;
void *c = &three;
void *d = &four;
void *e = &five;


int main() {
    test_create();
    //test_add();
    //test_insert();
    //test_first();
    //test_next();
    //test_prev();
    //test_last();
    //test_first();
    //test_curr();
    //test_remove();
    //test_prepend();
    //test_append();
    //test_concat();
    //test_free();
    //test_search();
}

void test_create(){

    printf(" ------------------create----------------- \n");
    
    printf("----1----- \n");
    List* list = List_create();
    print(list);
    printf("nodes： %d \n", List_count(list) );

    printf("---2------ \n");
    List* list2 = List_create();
    print(list2);
    printf("nodes： %d \n", List_count(list2) );

    printf("-----3------ \n");
    List* list3 = List_create();
    print(list3);
    printf("nodes： %d \n", List_count(list3) );

    printf("---4----- \n");
    List* list4 = List_create();
    print(list4);
    printf("nodes： %d \n", List_count(list4) );

    printf("----5------ \n");
    List* list5 = List_create();
    print(list5);
    printf("nodes： %d \n", List_count(list5) );
}

void test_add(){

    printf(" ------------------add----------------- \n"); 

    List* list = List_create();

    printf("----1----- \n");
    List_add(list, a);
    print(list);
    printf("nodes： %d \n", List_count(list) );

    printf("----2----- \n");
    List_add(list, b);
    print(list);
    printf("nodes： %d \n", List_count(list) );

    List_next(list);

    printf("----3(next)----- \n");
    List_add(list, c);
    print(list);
    printf("nodes： %d \n", List_count(list) );

    List_prev(list);
    List_prev(list);
    List_prev(list);

    printf("----4(3prev)----- \n");
    List_add(list, d);
    print(list);
    printf("nodes： %d \n", List_count(list) );

    printf("----5----- \n");
    List_add(list, e);
    print(list);
    printf("nodes： %d \n", List_count(list) );
    
}

void test_insert(){

    printf(" ------------------insert----------------- \n"); 
    List* list = List_create();

    printf("----1----- \n");
    List_insert(list, a);
    print(list);
    printf("nodes： %d \n", List_count(list) );

    printf("----2----- \n");
    List_insert(list, b);
    print(list);
    printf("nodes： %d \n", List_count(list) );

    List_prev(list);

    printf("----3(prev)----- \n");
    List_insert(list, c);
    print(list);
    printf("nodes： %d \n", List_count(list) );

    List_next(list);
    List_next(list);
    List_next(list);

    printf("----4(3next)----- \n");
    List_insert(list, d);
    print(list);
    printf("nodes： %d \n", List_count(list) );

    printf("----5----- \n");
    List_insert(list, e);
    print(list);
    printf("nodes： %d \n", List_count(list) );
}

void test_first(){

    printf(" ------------------first----------------- \n"); 
    List* list = List_create();

    printf("----1----- \n");
    List_add(list, b);
    printf("first is: %d, and should be: %d  \n", two, *(int *)List_first(list));


    printf("----2----- \n");
    List_insert(list, a);
    printf("first is: %d, and should be: %d  \n", one, *(int *)List_first(list));

    printf("----3----- \n");
    List_prepend(list, c);
    printf("first is: %d, and should be: %d  \n", three, *(int *)List_first(list));

    printf("----4----- \n");
    List_insert(list, d);
    printf("first is: %d, and should be: %d \n", four, *(int *)List_first(list));
}


void test_last(){
    printf(" ------------------last----------------- \n"); 
    List* list = List_create();

    printf("----1----- \n");
    List_insert(list, a);
    printf("last is: %d, and should be: %d  \n", one, *(int *)List_last(list));

    printf("----2----- \n");
    List_add(list, b);
    printf("last is: %d, and should be: %d  \n", two, *(int *)List_last(list));

    printf("----3----- \n");
    List_append(list, c);
    printf("last is: %d, and should be: %d  \n", three, *(int *)List_last(list));

    printf("----4----- \n");
    List_add(list, d);
    printf("last is: %d, and should be: %d  \n", four, *(int *)List_last(list));

}


void test_next(){
    printf(" ------------------next----------------- \n"); 
    List* list = List_create();

    List_insert(list, a);
    List_insert(list, b);
    List_insert(list, c);
    List_insert(list, d);
    
    print(list);


    printf("next is: %d \n", *(int*)List_next(list));
    printf("next is: %d \n", *(int*)List_next(list));
    printf("next is: %d \n", *(int*)List_next(list));
    Node* node = List_next(list);
    if (node == NULL){
    	printf("next is null");
    }
  
   print(list);
}

void test_prev(){

    printf(" ------------------prev----------------- \n"); 
    List* list = List_create();

    List_add(list, a);
    List_add(list, b);
    List_add(list, c);
    List_add(list, d);
    
    print(list);


    printf("prev is: %d \n", *(int*)List_prev(list));
    printf("prev is: %d \n", *(int*)List_prev(list));
    printf("prev is: %d \n", *(int*)List_prev(list));
    Node* node = List_prev(list);
    if (node == NULL){
    	printf("next is null");
    }
     
    print(list);

}

void test_curr(){

    printf(" ------------------curr----------------- \n"); 
    List* list = List_create();

    printf("----1----- \n");
    List_insert(list, a);
    printf("curr is: %d, and should be: %d  \n", one, *(int *)List_curr(list));

    printf("----2----- \n");
    List_add(list, b);
    printf("curr is: %d, and should be: %d  \n", two, *(int *)List_curr(list));

    printf("----3----- \n");
    List_append(list, c);
    printf("curr is: %d, and should be: %d  \n", three, *(int *)List_curr(list));

    printf("----4----- \n");
    List_prepend(list, d);
    printf("curr is: %d, and should be: %d  \n", four, *(int *)List_curr(list));

    printf("----5----- \n");
    List_last(list);
    printf("curr is: %d, and should be: %d  \n", four, *(int *)List_curr(list));

    printf("----6----- \n");
    List_first(list);
    printf("curr is: %d, and should be: %d  \n", three, *(int *)List_curr(list));


}


void test_remove(){
    printf(" ------------------remove----------------- \n"); 
    List* list = List_create();

    List_add(list, a);
    List_add(list, b);
    List_add(list, c);
    List_add(list, d);

    print(list);

    printf("----1----- \n");
    printf("%d \n", *(int *)List_remove(list));
    print(list);

    printf("----2----- \n");
    printf("%d \n", *(int *)List_remove(list));
    print(list);

    printf("----3----- \n");
    printf("%d \n", *(int *)List_remove(list));
    print(list);

    printf("----4----- \n");
    printf("%d \n", *(int *)List_remove(list));
    print(list);
}


void test_prepend(){
    printf(" ------------------prepend----------------- \n"); 

    List* list = List_create();

    printf("----1----- \n");
    List_prepend(list, a);
    print(list);
    
    printf("----2----- \n");
    List_prepend(list, b);
    print(list);

    printf("----3----- \n");
    List_prepend(list, c);
    print(list);

}


void test_append(){
    printf(" ------------------append----------------- \n"); 

    List* list = List_create();

    printf("----1----- \n");
    List_append(list, a);
    print(list);

    printf("----2----- \n");
    List_append(list, b);
    print(list);

    printf("----3----- \n");
    List_append(list, c);
    print(list);
}


void test_concat(){
    printf(" ------------------concat----------------- \n"); 
    List* list = List_create();

    List_add(list, a);
    List_add(list, b);
    List_add(list, c);
    List_add(list, d);
    printf("--list 1-- \n");
    print(list);

    List* list1 = List_create();
    List_add(list1, a);
    List_add(list1, b);
    List_add(list1, c);
    List_add(list1, d);
    printf("--list 2-- \n");
    print(list1);

    List* list2 = List_create();

    printf("--concat non empty-- \n");
    List_concat(list,list1);
    print(list);

    printf("--concat empty--  \n");
    List_concat(list,list2);
    print(list);
}


void test_free(){
    printf(" ------------------free----------------- \n"); 
    List* list = List_create();

    List_add(list, a);
    List_add(list, b);
    List_add(list, c);
    List_add(list, d);
    printf("--list 1-- \n");
    if(list != NULL){
       print(list);
    }

    printf("--free list 1-- \n");
    List_free(list,free);
    printf("done free \n");
    if(list != NULL){
       print(list);
    }

    List* list1 = List_create();
    printf("--free empty list-- \n");
    List_free(list1,free);
    if(list1 != NULL){
       print(list1);
    }
}

void free(void* curr){
    printf("freed: %d \n", *(int *)curr);
}

void test_trim(){
    printf(" ------------------trim----------------- \n"); 
    List* list = List_create();

    List_add(list, a);
    List_add(list, b);
    List_add(list, c);
    List_add(list, d);
    print(list);

    printf("----1----- \n");
    printf("%d \n", *(int *)List_trim(list));
    print(list);

    printf("----2----- \n");
    printf("%d \n", *(int *)List_trim(list));
    print(list);

    printf("----3----- \n");
    printf("%d \n", *(int *)List_trim(list));
    print(list);

    printf("----4----- \n");
    printf("%d \n", *(int *)List_trim(list));
    print(list);
}
void test_search(){

    printf(" ------------------search----------------- \n"); 
    List* list = List_create();

    List_add(list, a);
    List_add(list, b);
    List_add(list, c);
    List_add(list, d);
    print(list);

    printf("---search 4(true)--- \n");
    void* result = List_search(list,equal,d);
    if (result == NULL){
        printf("not found");
    }
    else{
        printf("found = %d \n",*(int *)result);
    }

    printf("---search 1(false)--- \n");
    result = List_search(list,equal,a);
    if (result == NULL){
        printf("not found \n");
    }
    else{
        printf("found = %d \n",*(int *)result);
    }
    
    printf("---search 4(false)--- \n");
    result = List_search(list,equal,d);
    if (result == NULL){
        printf("not found \n");
    }
    else{
        printf("found = %d \n",*(int *)result);
    }
    
    
    printf("---went back to first node--- \n");
    List_first(list);  
    printf("---search 3(true)--- \n");
    result = List_search(list,equal,c);
    if (result == NULL){
        printf("not found \n");
    }
    else{
        printf("found = %d \n",*(int *)result);
    }
}


bool equal(void* curr, void* pComparisonArg){
    if ( *(int *)curr == *(int *)pComparisonArg){
        return true;
    }else{
        return false;
    }
}

void print(List *pList){

    printf("print: ");
    Node* node = pList->head;

    while (node != NULL)
    {
       printf("%d ", *(int*)node->curr);
       node = node->next;
    }
    printf(" \n");

    printf("size: %d \n", pList->size); 
    printf("ListOutOfBounds: %d \n", pList->ListOutOfBounds);
}


