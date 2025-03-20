#include <stdio.h>
#include <string.h>

#include <mylib.h>

void main(int argc, char **argv)
{
	node_t *n1, *n2, *n3, *n4;

	init(&headnode);
	
	n1 = newnode();
	strcpy(n1->data, "A");

	n2 = newnode();
	strcpy(n2->data, "B");

	n3 = newnode();
	strcpy(n3->data, "C");

	print();

	delete(n3);
	delete(n2);
	delete(n1);
	print();
}

