#include <stdio.h>
#include <stdlib.h>

#include "stack.h"
#include "src/graphics/matrix.h"

Stack_t * createStack(){
	Stack_t * stack = createList();
	push(stack, createIdentity());
	return stack;
}
