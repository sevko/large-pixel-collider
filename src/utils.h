/*!
 *  @file utils.h
 *  @brief Functions that provide general, miscellaneous utility.
 */

/*!
 *  @brief A typedef'd struct to contain a file read with readFile().
 */
typedef struct {
	char ** buffer; /*!< The contents of the file. */
	int numLines;   /*!< The number of lines in the file. */
} ScannedFile_t;

/*
 *  @brief Read and return the contents of a file of arbitrary line-length and
 *      number of lines.
 *
 *  @param filename The name of the file to read.
 *
 *  @return A pointer to a ::ScannedFile_t containing the contents of the file
 *      named @p filename.
 */
ScannedFile_t * readFile(const char * const filename);

/*
 *  @brief Deallocate all the memory belonging to a ::ScannedFile_t.
 *
 *  @param file A pointer of the ::ScannedFile_t which, along with its members,
 *      will be deallocated.
 */
void freeFile(ScannedFile_t * file);
