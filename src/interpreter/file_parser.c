#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "src/globals.h"
#include "src/graphics/screen.h"
#include "src/interpreter/file_parser.h"
#include "src/interpreter/interpreter.h"
#include "src/interpreter/utils.h"
#include "src/interpreter/stack/stack.h"

/*!
 * Microseconds to wait before the file parser exits after successful script
 * evaluation.
 */
#define PARSER_EXIT_PAUSE 2e6

extern FILE * yyin;
extern int yyparse(void);

void readMDLFile(const char * const filePath){
	yyin = fopen(filePath, "r");
	yyparse();
	fclose(yyin);

	if(initializeVariables()){
		configureScreen();
		evaluateMDLScript();
		usleep(PARSER_EXIT_PAUSE);
		quitScreen();
	}
}
