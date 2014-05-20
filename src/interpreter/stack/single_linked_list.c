#include <stdlib.h>

#include "single_linked_list.h"

/*
 * @brief Indicate whether a ::ListElement_t is a ::SingleLList_t's head.
 *
 * @param list A ::SingleLList_t pointer.
 * @param elem A ::ListElement_t pointer.
 *
 * @return 1 if @p elem is @p list's ::SingleLList_t::head; 0 otherwise.
 */
#define IS_HEAD(list, elem) (elem == list->head)

/*
 * @brief Indicate whether a ::ListElement_t is a ::SingleLList_t's tail.
 *
 * @param list A ::SingleLList_t pointer.
 * @param elem A ::ListElement_t pointer.
 *
 * @return 1 if @p elem is @p list's ::SingleLList_t::tail; 0 otherwise.
 */
#define IS_TAIL(list, elem) (elem == list->tail)

SingleLList_t * createList(void){
	SingleLList_t * const newList = malloc(sizeof(SingleLList_t));
	newList->length = 0;
	newList->head = newList->tail = NULL;
	return newList;
}

void insertHead(SingleLList_t * const list, void * const data){
	ListElement_t * newElem = malloc(sizeof(ListElement_t));
	newElem->data = data;
	newElem->nextElement = list->head;

	if(list->tail == NULL)
		list->tail = newElem;
	list->head = newElem;
	list->length++;
}

void * removeHead(SingleLList_t * const list){
	ListElement_t * oldHead = list->head;
	list->head = list->head->nextElement;
	list->length--;

	void * data = oldHead->data;
	free(oldHead);
	return data;
}

void insertAfter(SingleLList_t * const list, ListElement_t * const elem,
	void * const data){
	ListElement_t * newElem = malloc(sizeof(ListElement_t));
	newElem->data = data;
	newElem->nextElement = elem->nextElement;
	elem->nextElement = newElem;

	list->length++;
	if(IS_TAIL(list, elem))
		list->tail = newElem;
}

void * removeAfter(SingleLList_t * const list, ListElement_t * const elem){
	ListElement_t * removedElem = elem->nextElement;
	elem->nextElement = elem->nextElement->nextElement;

	if(IS_TAIL(list, removedElem))
		list->tail = elem;
	list->length--;

	char * data = removedElem->data;
	free(removedElem);
	return data;
}

void freeList(SingleLList_t * const list, void (* freeElement)(void * element)){
	ListElement_t * currElem = list->head;
	while(currElem != NULL){
		ListElement_t * tmpNextElem = currElem->nextElement;
		freeElement(currElem->data);
		free(currElem);
		currElem = tmpNextElem;
	}

	free(list);
}
