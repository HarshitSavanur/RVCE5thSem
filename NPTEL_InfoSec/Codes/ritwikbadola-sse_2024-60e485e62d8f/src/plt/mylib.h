#ifndef __MYLIB_H__
#define __MYLIB_H__

typedef struct _node_t{
	char data[16];	
	struct _node_t *next;
	struct _node_t *prev;
}node_t;


node_t *newnode();
node_t *delete(node_t *n);
void print();

extern node_t headnode;

#endif
