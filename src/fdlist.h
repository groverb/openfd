#ifndef _LINKED_LIST_H
#define _LINKED_LIST_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _fdnode{
	void* val;
	struct _fdnode* next;
	struct _fdnode* prev;
	char type; // c: char, d/i: int, s: char*, l/f: double/float...
} fdnode; 

typedef struct _fdlist{
	fdnode* HEAD;
	fdnode* TAIL;
	size_t size;
} fdlist;

fdlist* make_fdlist();

int push_front(fdlist* l, void* val, char type);
	
const fdnode* fdlist_head(fdlist* l);

int pop_front(fdlist* l);

int __push_back(fdlist* l, void* val, char type);

int push_back(fdlist* l, void* val, char type);

int __pop_back(fdlist* l);

int pop_back(fdlist* l);

void fdlist_foreach(fdlist* l, void (*f)(void*));

void print_ch_fdlist(fdlist* l);

fdnode* get(fdlist* l, int index);

size_t size(fdlist* l);

void print_fdlist(fdlist* l);


#define free_fdlist_custom(l,f,t){ \
	fdnode* temp;													\
	while(l->HEAD != NULL){												\
		temp = l->HEAD;												\
		l->HEAD = l->HEAD->next;										\
		(f)((t*)(temp->val));											\
		free(temp);												\
		temp = NULL;												\
	}														\
	l->size = 0;													\
	free(l);													\
	l = NULL;													\
}

size_t free_fdlist(fdlist* l);


#endif // _LINKED_LIST_H
