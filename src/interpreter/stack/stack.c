#include <stdio.h>
#include <stdlib.h>

#include "stack.h"

Stack_t * createStack(){
	Stack_t * stack = createList();
	Point_t * origin = malloc(sizeof(Point_t));
	origin->x = 0;
	origin->y = 0;
	origin->z = 0;
	push(stack, origin);

	return stack;
}
