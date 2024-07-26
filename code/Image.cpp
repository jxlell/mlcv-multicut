#include "Image.h"

/**
 * @brief Compare two RGB colors for equality.
 *
 * This function compares two RGB colors by comparing their individual color components.
 *
 * @param color1 The first RGB color to compare.
 * @param color2 The second RGB color to compare.
 * @return true if the colors are equal, false otherwise.
 */
bool compareRGB(const RGB& color1, const RGB& color2) {
    return (color1.red == color2.red) && (color1.green == color2.green) && (color1.blue == color2.blue);
}