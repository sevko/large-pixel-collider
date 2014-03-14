typedef struct {
	char ** buffer;
	int numLines;
} ScannedFile_t;

ScannedFile_t * readFile(const char * const filename);
void freeFile(ScannedFile_t * file);
