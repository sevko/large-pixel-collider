#define NUM_POINT_VARS 4

typedef struct {
	double * points[NUM_POINT_VARS];
	int numPoints;
} Matrix_t;

Matrix_t * createMatrix();
void freeMatrix(Matrix_t * const matrix);
void expandMatrix(Matrix_t * const matrix);
void addPoint(Matrix_t * const matrix, int x, int y, int z);
void addEdge(Matrix_t * const matrix, int x1, int y1, int z1, int x2, int y2, int z2);
void drawMatrixLines(const Matrix_t * const matrix);
void printMatrix(const Matrix_t * const matrix);
