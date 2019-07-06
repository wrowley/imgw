
#include <stdio.h>

typedef struct imgw_bmp_read_s imgw_bmp_read;

/* Opens a BMP file for doing things with.
 * 
 * Returns zero if the file was opened successfully and appears to be a BMP, and
 * non-zero otherwise. No guarantees are made beyond this.
 *
 *   filename   - The path of the file to read.
 *   bmp_reader - Pointer to a imgw_bmp_read struct.
 */
int
imgw_bmp_read_open
    (const char          *filename
    ,      imgw_bmp_read *bmp_reader
    );

/* Closes a BMP file after doing things
 *
 * bmp_reader - A imgw_bmp_read struct which was initialised using a call
 *    to imgw_bmp_read_open which returned 0.
 */
void
imgw_bmp_read_close
    (imgw_bmp_read *bmp_reader);

/* Provides the width, height, and bits-per-pixel of a BMP that has successfully
 * been opened
 *
 * bmp_reader - A imgw_bmp_read struct which was initialised using a call
 *      to imgw_bmp_read_open which returned 0.
 * width, height, bpp - Must all be valid pointers to the unsigned datatype
 */
void
imgw_bmp_read_properties
    (const imgw_bmp_read *bmp_reader
    ,      unsigned      *width
    ,      unsigned      *height
    ,      unsigned      *bpp
    );

/* Writes the pixel array into memory provided by the caller.
 *
 * bmp_reader - A imgw_bmp_read struct which was initialised using a call
 *      to imgw_bmp_read_open which returned 0.
 * pixel_array - Must point to at least width * height * 3 octets of memory.
 *      The colour of pixel 'i' is given by,
 *          pixel_array[i * 3 + 0] -> Blue(i)
 *          pixel_array[i * 3 + 1] -> Green(i)
 *          pixel_array[i * 3 + 2] -> Red(i)
 *      Pixels are stored in rows, such that the index of pixel 'i' is given by
 *          i = y * width + x
 *      where:
 *          y in [0, height], starting at the top of the image
 *          x in [0, width ], starting at the left of the image
 *      Note that this treats the vertical axis opposite to the convention of
 *      the BMP image file format.
 *
 * Using imgw_bmp_read_properties() will allow the caller to determine the
 * memory requirements for pixel_array.
 */
void
imgw_bmp_read_pixel_array
    (imgw_bmp_read *bmp_reader
    ,unsigned char *pixel_array
    );

/*** Private, exposed only for the purposes of stack allocation ***/
struct imgw_bmp_read_s
{
    FILE          *fp;
    unsigned long  file_size;
    unsigned long  pixel_array_offset;

    unsigned       width;
    unsigned       height;
    unsigned       bpp;

    int            eof_reached;
};
