#define IMAGE_WIDTH 600
#define IMAGE_HEIGHT 600

#define ERROR(format, ...) \
	printf("%s: %d: " format "\n", __FILE__, __LINE__, ##__VA_ARGS__) 
