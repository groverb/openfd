#include "fdlist.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

fdlist* make_fdlist(){
	fdlist* l = malloc(sizeof(fdlist));
	l->HEAD = l->TAIL = NULL;
	l->size = 0;
	return l;
}

int push_front(fdlist* l, void* val, char type){
	if(val == NULL) return 0;
	fdnode* new_fdnode = (fdnode*) malloc(sizeof(fdnode));
	if(new_fdnode == NULL || l == NULL){
		return 0;
	}
	new_fdnode->val = val;
	new_fdnode->type = type;
	fdnode* cur_h = l->HEAD;
	new_fdnode->next = cur_h;
	l->HEAD = new_fdnode;
	l->size++;
	return 1;
}

const fdnode* fdlist_head(fdlist* l){
	return l->HEAD;
}

int pop_front(fdlist* l){
	if(l == NULL) return 0;
	fdnode* cur_h = l->HEAD;
	l->HEAD = l->HEAD->next;
	free(cur_h->val);
	free(cur_h);
	l->size--;
	return 1;
}

int __push_back(fdlist* l, void* val, char type){
	if(val == NULL) return 0;
	fdnode* new_fdnode = (fdnode*) malloc(sizeof(fdnode));
	if(new_fdnode != NULL){
		new_fdnode->val = val;
		new_fdnode->type = type;
		new_fdnode->next = NULL;
		if(l->HEAD == NULL){
			l->HEAD = new_fdnode;
			return 1;
		}
		fdnode* cur = l->HEAD;
		while(cur->next != NULL){
			cur = cur->next;
		}
		cur->next = new_fdnode;
		l->TAIL = new_fdnode;
		l->size++;
		return 1;
	}
	return 0;
}

int push_back(fdlist* l, void* val, char type){
	fdnode* new_fdnode = malloc(sizeof(fdnode));
	if(new_fdnode != NULL){
		new_fdnode->next = NULL;
		new_fdnode->val = val;
		new_fdnode->type = type;
		l->size++;
		if(l->HEAD == NULL && l->TAIL == NULL){ l->TAIL = l->HEAD = new_fdnode; return 1; }

		new_fdnode->prev = l->TAIL;
		l->TAIL->next = new_fdnode;
		l->TAIL = l->TAIL->next;
		return 1;
	}

	return 0;
}
int __pop_back(fdlist* l){
	if(l != NULL){
		fdnode* cur = l->HEAD;
		if(cur == NULL) { return 1 ; }
		if(cur->next == NULL) { free(cur->val); free(cur); l->HEAD = NULL; return 1; }
		fdnode* cur_n = cur->next;
		while(cur_n->next != NULL){
			cur = cur->next;
			cur_n = cur->next;
		}
		free(cur_n->val);
		free(cur_n);
		cur->next = NULL;
		l->size--;
		return 1;
	}
	return 0;
}

int pop_back(fdlist* l){
	if(l->TAIL != NULL){
		if(l->HEAD->next == NULL && l->TAIL->prev == NULL) {
			free(l->HEAD->val); free(l->HEAD); l->HEAD = l->TAIL = NULL;
			return 1;
		}
		fdnode* tbf = l->TAIL;
		l->TAIL = l->TAIL->prev;

		free(tbf->val);
		free(tbf);

		l->TAIL->next = NULL;
		l->size--;
		return 1;
	}
	return 0;
}

void fdlist_foreach(fdlist* l, void (*f)(void*)){
	if(l != NULL){
		fdnode* cur = l->HEAD;
		while(cur != NULL){
			f(cur);
			cur = cur->next;
		}
	}

}
void print_ch_fdlist(fdlist* l){
	if(l != NULL){
		fdnode* cur = l->HEAD;
		while(cur != NULL){
			printf("%s\n",(char*) cur->val);
			cur = cur->next;
		}
	}
}

fdnode* get(fdlist* l, int index){
	int ind = 0;
	fdnode* cur_n = l->HEAD;
	if(cur_n != NULL){
		while(cur_n->next != NULL){
			if(ind == index) return cur_n;
			cur_n = cur_n->next;
			ind++;
		}
	}
	return NULL;
}

size_t size(fdlist* l){
	return l->size;
}


void print_fdlist(fdlist* l){
	if(l != NULL){
		fdnode* cur = l->HEAD;
		while(cur != NULL){
			switch(cur->type){
				case 'd':
				case 'i':
					printf("%d\n",*((int*) cur->val));
					break;
				case 'f':
					printf("%f\n",*((float*) cur->val));
					break;
				case 'l':
					printf("%f\n", *((double*) cur->val));
					break;
				case 'c':
					printf("%c\n", *((char*)cur->val));
					break;
				case 's':
					printf("%s\n", ((char*)cur->val));
					break;
			}
			cur = cur->next;
		}
	}
}


size_t free_fdlist(fdlist* l){
	fdnode* temp;
	while(l->HEAD != NULL){
		temp = l->HEAD;
		l->HEAD = l->HEAD->next;
		free(temp->val);
		free(temp);
		temp = NULL;
	}
	l->size = 0;
	int finalsz = l->size;

	free(l);
	l = NULL;
	return finalsz;
}

