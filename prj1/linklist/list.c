#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
 * Allocate memory for a node of type struct nodeStruct and initialize
 * it with the value item. Return a pointer to the new node.
 */


Node* List_createNode(int item){
	Node* node = (Node*)(malloc(sizeof(Node)));
	node->item = item;
	node->next = NULL;
	return node;
}

/*
 * Insert node at the head of the list.
 */
void List_insertHead (struct nodeStruct **headRef, struct nodeStruct *node){
	if (*headRef == NULL){
		*headRef = node;
	}

	else {
		//point the existing list to node->next 
		node->next = *headRef;
		//assign new head node
		*headRef = node; 
	}
}

/*
 * Insert node after the tail of the list.
 */
void List_insertTail (struct nodeStruct **headRef, struct nodeStruct *node){
	Node* tempnode = *headRef;
	if (*headRef == NULL){
		*headRef = node; 
	}
	else {
		while (tempnode->next != NULL){
			tempnode = tempnode->next; 
		}

		tempnode->next = node;
		*headRef = tempnode; 
	}
}

/*
 * Count number of nodes in the list.
 * Return 0 if the list is empty, i.e., head == NULL
 */
int List_countNodes (struct nodeStruct *head){
	int list_size = 0;
	Node* tempnode = head;

	while (tempnode != NULL){
		tempnode = tempnode->next;
		list_size ++;
	}
	return list_size; 

}

/*
 * Return the first node holding the value item, return NULL if none found
 */
struct nodeStruct* List_findNode(struct nodeStruct *head, int item){
	Node* tempnode = head;

	//check for the first node
	if (tempnode->item == item) {
		return tempnode;
	}
	while (tempnode->item != item && tempnode->next != NULL){
		if (tempnode->item == item){
			return tempnode;
		}
		else{
			tempnode = tempnode->next;
		}
	}

	// check for the last node
	if (tempnode->item == item) {
		return tempnode;
	}

	return NULL;
}

/*
 * Delete node from the list and free memory allocated to it.
 * This function assumes that node has been properly set to a valid node 
 * in the list. For example, the client code may have found it by calling 
 * List_findNode(). If the list contains only node, the head of the list 
 * should be set to NULL.
 */
void List_deleteNode (struct nodeStruct **headRef, struct nodeStruct *node){

	Node* current = *headRef;
	Node* previous = NULL;
	int list_size = List_countNodes(current);
	// only one node just delete the headnode
	if (list_size == 1){
		if (previous == NULL){
			free(current);
			*headRef = NULL;
		}
	}

	else if (list_size > 1){
		while (current != NULL){
			if (current->item == node->item){
				if (previous != NULL){
					previous->next = current->next;
				}
				else{
					*headRef = (*headRef)->next;
				}		
				free(current);
				current = NULL;
			}
			else {
				previous = current;
				current = current->next;
			}

		}
	}
}

/*
 * Sort the list in ascending order based on the item field.
 * Any sorting algorithm is fine.
 */
void List_sort(struct nodeStruct **headRef) {
	if (*headRef == NULL || (*headRef)->next == NULL)
		return;

	Node* A = *headRef;
	Node* B;

	while (A != NULL)
	{
		Node* current = A;
		A = A->next;
		if (*headRef == NULL || current->item < (*headRef)->item)
		{
			current->next = *headRef;
			*headRef = current;
		}
		else
		{
			B = *headRef;
			while (B != NULL)
			{
				if (B->next == NULL || current->item < B->next->item)
				{
					current->next = B->next;
					B->next = current;
					break;
				}
				B = B->next;
			}
		}
	}
	B->next = NULL;
}
