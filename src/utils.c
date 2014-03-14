#include <stdio.h>
#include <stdlib.h>

#include "globals.h"
#include "utils.h"

// read file named filePath into a ScannedFile_t, return it
ScannedFile_t * readFile(const char * const filePath){
	FILE * file = fopen(filePath, "r");
	if(file == NULL){
		ERROR("Failed to open file: %s", filePath);
		exit(EXIT_FAILURE);
	}

	char byte, ** buffer = malloc(sizeof(char *));
	buffer[0] = NULL;
	int numLines = 0, lineInd = 0;

	while((byte = fgetc(file)) != EOF){
		buffer[numLines] = realloc(buffer[numLines],
			(lineInd + 1) * sizeof(char *));
		buffer[numLines][lineInd++] = byte;

		if(byte == '\n'){
			buffer[numLines] = realloc(buffer[numLines],
				(lineInd + 1) * sizeof(char *));
			buffer[numLines][lineInd] = '\0';
			buffer = realloc(buffer, (numLines + 2) * sizeof(char *));
			buffer[++numLines] = NULL;
			lineInd = 0;
		}
	}
	fclose(file);

	ScannedFile_t * scannedFile = malloc(sizeof(ScannedFile_t));
	scannedFile->buffer = buffer;
	scannedFile->numLines = numLines;
	return scannedFile;
}

// deallocate all internal pointers of a ScannedFile_t
void freeFile(ScannedFile_t * file){
	int line;
	for(line = 0; line < file->numLines; line++)
		free(file->buffer[line]);
	free(file->buffer);
	free(file);
}
