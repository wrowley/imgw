/* Writes an 8bit image with 256 colours defined.
 * 
 * Returns zero if a file was opened successfully, and non-zero otherwise. No
 * guarantees are made beyond this.
 *
 *   filename     - The path of the file to write to.
 *   pixels       - The 8-bit pixels of the image. The values correspond to a
 *      lookup in the colour_table.
 *   width        - The width of the image in pixels.
 *   height       - The height of the image in pixels.
 *   colour_table - This is a table of 4 * 256 bytes, where the colour of
 *      pixels[i] will be given by,
 *          colour_table[pixels[i] * 4 + 0] -> Blue(i)
 *          colour_table[pixels[i] * 4 + 1] -> Green(i)
 *          colour_table[pixels[i] * 4 + 2] -> Red(i)
 *          colour_table[pixels[i] * 4 + 3] -> 0x00
 */
int
imgw_bmp_write_8bit
    (const char          *filename
    ,const unsigned char *pixels
    ,      unsigned       width
    ,      unsigned       height
    ,const unsigned char *colour_table
    );

/* Writes an 8bit gray-scale image.
 * 
 * Returns zero if a file was opened successfully, and non-zero otherwise. No
 * guarantees are made beyond this.
 *
 *   filename - The path of the file to write to.
 *   pixels   - The 8-bit gray-scale pixels of the image. The value 0x00
 *      corresponds with the tone of black, and the value 0xFF corresponds with
 *      the tone of white. Pixels are stored in rows, and as such defined as,
 *          pixels[y * width + x]
 *      where:
 *          y in [0, height], starting at the top of the image
 *          x in [0, width ], starting at the left of the image
 *      Note that this treats the vertical axis opposite to the convention of
 *      the BMP image file format.
 *   width    - The width of the image in pixels.
 *   height   - The height of the image in pixels.
 */
int
imgw_bmp_write_8bit_grayscale
    (const char          *filename
    ,const unsigned char *pixels
    ,      unsigned       width
    ,      unsigned       height
    );


