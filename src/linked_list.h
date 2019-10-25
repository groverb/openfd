#ifndef _LINKED_LIST_H
#define _LINKED_LIST_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _node{
	void* val;
	struct _node* next;
	struct _node* prev;
	char type; // c: char, d/i: int, s: char*, l/f: double/float...
} node; 

typedef struct _list{
	node* HEAD;
	node* TAIL;
	size_t size;
} list;

list* make_list();

int push_front(list* l, void* val, char type);
	
const node* list_head(list* l);

int pop_front(list* l);

int __push_back(list* l, void* val, char type);

int push_back(list* l, void* val, char type);

int __pop_back(list* l);

int pop_back(list* l);

void list_foreach(list* l, void (*f)(void*));

void print_ch_list(list* l);

node* get(list* l, int index);

size_t size(list* l);

void print_list(list* l);


#define free_list_custom(l,f){ \
	node* temp;													\
	while(l->HEAD != NULL){												\
		temp = l->HEAD;												\
		l->HEAD = l->HEAD->next;										\
		(f)((temp->val));											\
		free(temp);												\
		temp = NULL;												\
	}														\
	l->size = 0;													\
	int finalsz = l->size;												\
	free(l);													\
	l = NULL;													\
}

size_t free_list(list* l);


#endif // _LINKED_LIST_H
