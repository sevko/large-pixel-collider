#define ERROR(format, ...) \
	printf("%s: %d: " format "\n", __FILE__, __LINE__, ##__VA_ARGS__) 
