#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mylib.h>

/* head of the list */
node_t headnode;


void init(node_t *list)
{
	list->next = NULL;
	list->prev = NULL;
	memset(list, 0, sizeof(list));
}

/* Insert node at the end of the doubly linked list 
   pointed to by list 
*/
node_t *newnode()
{
	node_t *list = &headnode;
	node_t *n;

	while(list->next != NULL){
		list = list->next;
	}

	n = (node_t *) malloc(sizeof(node_t));
	list->next = n;
	n->prev = list;
	n->next = NULL;

	return n;
}

/* Delete the node n from the list */
node_t *delete(node_t *n)
{
	node_t *FD = n->next;
	node_t *BK = n->prev;

	if(n->prev != NULL) n->prev->next = n->next;
	if(n->next != NULL) n->next->prev = n->prev;
	if(n != NULL) free(n);
}

/* Print them all out */
void print()
{	
	node_t *list = &headnode;

	while(list->next != NULL){
		list = list->next;
		printf("%s\n", list->data);
	}
}



