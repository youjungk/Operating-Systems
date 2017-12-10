#ifndef LIST_H
#define LIST_H
/*
 * Allocate memory for a node of type struct nodeStruct and initialize
 * it with the value item. Return a pointer to the new node.
 */

typedef struct nodeStruct {
	int item;
	struct nodeStruct *next;
} Node;

struct nodeStruct* List_createNode(int item);
void List_insertHead (struct nodeStruct **headRef, struct nodeStruct *node);
void List_insertTail (struct nodeStruct **headRef, struct nodeStruct *node);
int List_countNodes (struct nodeStruct *head);
struct nodeStruct* List_findNode(struct nodeStruct *head, int item);
void List_deleteNode (struct nodeStruct **headRef, struct nodeStruct *node);
void List_sort (struct nodeStruct **headRef);

#endif