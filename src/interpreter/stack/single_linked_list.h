/*!
 *  @file
 *  @brief An implementation of a singly-linked list, generalized for arbitrary
 *      (void *) data-types.
 */

#pragma once

//! An element (chain-link) of a ::SingleLList_t.
typedef struct ListElement {
	void * data; //! A generalized memory pointer.
	//! Pointer to the next element in the ::SingleLList_t.
	struct ListElement * nextElement;
} ListElement_t;

//! A singly-linked list, composed of ::ListElement_t.
typedef struct {
	int length; //! The number of ::ListElement_t.
	ListElement_t * head, * tail; //! Pointers to the front and end of the list.
} SingleLList_t;

/*
 * @brief Allocate memory for a ::SingleLList_t, and initialize all member
 *      variables.
 *
 * @return A pointer to the newly allocate ::SingleLList_t.
 */
SingleLList_t * createList(void);

/*
 * @brief Insert an ::ListElement_t as a ::SingleLList_t::head.
 *
 * @param list The ::SingleLList_t into whose ::SingleLList_t::head position
 *      the new ::ListElement_t will be inserted.
 * @param data The data pointer to be contained by the new
 *      ::SingleLList_t::head.
 */
void insertHead(SingleLList_t * const list, void * const data);

/*
 * @brief Remove a ::SingleLList_t's ::SingleLList_t::head.
 *
 * @param list The ::SingleLList_t whose ::SingleLList_t::head ::ListElement_t
 *      will be removed.
 *
 * @return A pointer to the data contained by the removed ::SingleLList_t::head.
 */
void * removeHead(SingleLList_t * const list);

/*
 * @brief Insert a ::ListElement_t containing a given data pointer after
 *      another ::ListElement_t in a ::SingleLList_t.
 *
 * @param list The ::SingleLList_t to contain the new ::ListElement_t.
 * @param elem The ::ListElement_t after which to insert the new
 *      ::ListElement_t.
 * @param data A pointer to the data to be contained in the new ::ListElement_t.
 */
void insertAfter(SingleLList_t * const list, ListElement_t * const elem,
	void * const data);

/*
 * @brief Remove the ::ListElement_t pointed to by another ::ListElement_t.
 *
 * @param list The ::SingleLList_t containing the pointing ::ListElement_t.
 * @param elem The ::ListElement_t whose ::ListElement_t::nextElement will be
 *      removed.
 *
 * @return A pointer to the data contained by the removed ::ListElement_t.
 */
void * removeAfter(SingleLList_t * const list, ListElement_t * const elem);

/*
 * @brief Deallocate all pointers encapsulated by a ::SingleLList_t.
 *
 * @param list The ::SingleLList_t to deallocate.
 * @param freeElement A pointer to a function used to free the objects pointed
 *      to by @p list's elements' void data pointers.
 */
void freeList(SingleLList_t * const list, void (* freeElement)(void * element));
