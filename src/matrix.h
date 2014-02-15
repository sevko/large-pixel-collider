#define NUM_POINT_VARS 4

typedef struct {
	double * points[NUM_POINT_VARS];
	int numPoints;
} Matrix_t;

Matrix_t * createMatrix();
void freeMatrix(Matrix_t * matrix);
void expandMatrix(Matrix_t * matrix);
void addPoint(Matrix_t * matrix, int x, int y, int z);
void addEdge(Matrix_t * matrix, int x1, int y1, int z1, int x2, int y2, int z2);
void drawMatrixLines(Matrix_t * matrix);
void printMatrix(Matrix_t * matrix);
