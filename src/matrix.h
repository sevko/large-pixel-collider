#define NUM_POINT_VARS 4
#define X_AXIS 0
#define Y_AXIS 1
#define Z_AXIS 2
#define RAD (M_PI / 180)

typedef struct {
	double * points[NUM_POINT_VARS];
	int numPoints;
} Matrix_t;

Matrix_t * createMatrix();
void freeMatrix(Matrix_t * const matrix);

void addPoint(Matrix_t * const matrix, double x, double y, double z);
void addTransformPoint(Matrix_t * const matrix, double x, double y, double z,
	double w);
void addEdge(Matrix_t * const matrix, double x1, double y1, double z1,
	double x2, double y2, double z2);
void drawMatrixLines(const Matrix_t * const matrix);
void multiplyScalar(double scalar, Matrix_t * const matrix);
void multiplyMatrix(Matrix_t * const m1, Matrix_t * const matrix);
Matrix_t * createTranslation(double dx, double dy, double dz);
Matrix_t * createScale(double dx, double dy, double dz);
Matrix_t * createRotation(int axis, double angle);
void printPointMatrix(const Matrix_t * const matrix);
void printMatrix(const Matrix_t * const matrix);
