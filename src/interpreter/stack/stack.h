/*!
 *  @file
 *  @brief A collection of wrapper macros for .single_linked_list.h functions,
 *      which imitate LIFO stack behavior for arbitrary data-types.
 */

#pragma once

#include "src/interpreter/stack/single_linked_list.h"

/*
 * @brief Return the data pointer contained by the top element of a ::Stack_t.
 *
 * @param stack (::Stack_t *)
 *
 * @return (void *)
 */
#define peek(stack) (stack->head->data)

/*
 * @brief Push an element containing a given data pointer onto a ::Stack_t.
 *
 * @param stack (::Stack_t *) The stack the new element will be placed on.
 * @param data (void *) A generalized memory-pointer to be contained by the new
 *      top element of @p stack.
 */
#define push(stack, data) insertHead(stack, data)

/*
 * @brief Pop the top element off a ::Stack_t.
 *
 * @param stack (::Stack_t *) A pointer to the stack whose top element will be
 *  popped off.
 *
 * @return (void *) The data pointer contained by the top element of @p stack.
 */
#define pop(stack) removeHead(stack)

/*
 * @brief Deallocate all pointers encapsulated by a ::Stack_t.
 *
 * @param stack (::Stack_t *) A pointer to the @p stack to be deallocated.
 */
#define freeStack(stack) freeList(stack)

//! A wrapper stack representation of a linked-list.
typedef SingleLList_t Stack_t;

/*
 * @brief Allocate a ::Stack_t, and initialize member variables.
 *
 * The newly created ::Stack_t contains an identity ::Matrix_t.
 *
 * @return A pointer to the newly created ::Stack_t.
 */
Stack_t * createStack();
