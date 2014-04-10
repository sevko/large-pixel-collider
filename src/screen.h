/*!
 *  @file screen.h
 *  @brief Functions for initializing and drawing to the engine's SDL screen.
 */

#pragma once

/*!
 *  @brief Initialize the SDL screen.
 */
void configureScreen(void);

/*!
 *  @brief Draw a pixel to the SDL screen.
 *
 *  @param x The x-coordinate of the pixel.
 *  @param y The y-coordinate of the pixel.
 */
void drawPixel(int x, int y);

/*!
 *  @brief Update the SDL screen to display pixels newly plotted with
 *      drawPixel().
 *
 *  renderScreen() must be called for the SDL screen to display any newly
 *  rendered pixels.
 */
void renderScreen(void);

/*!
 *  @brief Clear the SDL screen.
 *
 *  Clear the screen by filling it with black pixels.
 */
void clearScreen(void);

/*!
 *  @brief Deinitialize the SDL screen.
 *
 *  quitScreen() should be called once the engine finishes using the SDL screen.
 */
void quitScreen(void);

/*!
 *  @brief Save the currently displayed SDL screen to a BMP file.
 *
 *  @param filename The path of the BMP file to save the screen to.
 */
int writeScreen(const char * const filename);
