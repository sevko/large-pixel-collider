typedef struct {
	char ** script;
	int numLines;
} Script_t;

Script_t *  readScriptFile(char * filePath);
void evaluateScript(Script_t * script);
void freeScript(Script_t * script);
