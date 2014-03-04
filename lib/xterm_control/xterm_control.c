#include <stdio.h>
#include "xterm_control.h"

void xt_par0(char *control_sequence) { 
	printf("%s",control_sequence); 
}

void xt_par1(char *control_sequence, int parameter1) {
	printf(control_sequence, parameter1);
}

void xt_par2(char *control_sequence, int parameter1, int parameter2) {
	printf(control_sequence, parameter1, parameter2);
}

