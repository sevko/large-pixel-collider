struct Script {
	char ** script;
	int numLines;
};

typedef struct Script Script_t;
Script_t *  readScriptFile(char * filePath);
void freeScript(Script_t * script);
