typedef struct {
	char ** script;
	int numLines;
} Script_t;

Script_t *  readScriptFile(char * filePath);
void evaluateScript(Script_t * script);
int evaluateCommand(char ** command, Matrix_t * points, Matrix_t ** transform);
int argsRequired(char cmd);
void freeScript(Script_t * script);
