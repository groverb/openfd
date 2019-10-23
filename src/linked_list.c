#include "linked_list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

list* make_list(){
	list* l = malloc(sizeof(list));
	l->HEAD = l->TAIL = NULL;
	l->size = 0;
	return l;
}

int push_front(list* l, void* val, char type){
	if(val == NULL) return 0;
	node* new_node = (node*) malloc(sizeof(node));
	if(new_node == NULL || l == NULL){
		return 0;
	}
	new_node->val = val;
	new_node->type = type;
	node* cur_h = l->HEAD;
	new_node->next = cur_h;
	l->HEAD = new_node;
	l->size++;
	return 1;
}

const node* list_head(list* l){
	return l->HEAD;
}

int pop_front(list* l){
	if(l == NULL) return 0;
	node* cur_h = l->HEAD;
	l->HEAD = l->HEAD->next;
	free(cur_h->val);
	free(cur_h);
	l->size--;
	return 1;
}

int __push_back(list* l, void* val, char type){
	if(val == NULL) return 0;
	node* new_node = (node*) malloc(sizeof(node));
	if(new_node != NULL){
		new_node->val = val;
		new_node->type = type;
		new_node->next = NULL;
		if(l->HEAD == NULL){
			l->HEAD = new_node;
			return 1;
		}
		node* cur = l->HEAD;
		while(cur->next != NULL){
			cur = cur->next;
		}
		cur->next = new_node;
		l->TAIL = new_node;
		l->size++;
		return 1;
	}
	return 0;
}

int push_back(list* l, void* val, char type){
	node* new_node = malloc(sizeof(node));
	if(new_node != NULL){
		new_node->next = NULL;
		new_node->val = val;
		new_node->type = type;
		if(l->HEAD == NULL && l->TAIL == NULL){ l->TAIL = l->HEAD = new_node; return 1; }

		new_node->prev = l->TAIL;
		l->TAIL->next = new_node;
		l->TAIL = l->TAIL->next;
		l->size++;
		return 1;
	}

	return 0;
}
int __pop_back(list* l){
	if(l != NULL){
		node* cur = l->HEAD;
		if(cur == NULL) { return 1 ; }
		if(cur->next == NULL) { free(cur->val); free(cur); l->HEAD = NULL; return 1; }
		node* cur_n = cur->next;
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

int pop_back(list* l){
	if(l->TAIL != NULL){
		if(l->HEAD->next == NULL && l->TAIL->prev == NULL) {
			free(l->HEAD->val); free(l->HEAD); l->HEAD = l->TAIL = NULL;
			return 1;
		}
		node* tbf = l->TAIL;
		l->TAIL = l->TAIL->prev;

		free(tbf->val);
		free(tbf);

		l->TAIL->next = NULL;
		l->size--;
		return 1;
	}
	return 0;
}

void list_foreach(list* l, void (*f)(void*)){
	if(l != NULL){
		node* cur = l->HEAD;
		while(cur != NULL){
			f(cur);
			cur = cur->next;
		}
	}

}
void print_ch_list(list* l){
	if(l != NULL){
		node* cur = l->HEAD;
		while(cur != NULL){
			printf("%s\n",(char*) cur->val);
			cur = cur->next;
		}
	}
}

node* get(list* l, int index){
	int ind = 0;
	node* cur_n = l->HEAD;
	if(cur_n != NULL){
		while(cur_n->next != NULL){
			if(ind == index) return cur_n;
			cur_n = cur_n->next;
			ind++;
		}
	}
	return NULL;
}

size_t size(list* l){
	return l->size;
}


void print_list(list* l){
	if(l != NULL){
		node* cur = l->HEAD;
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

size_t free_list(list* l){
	node* temp;
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

