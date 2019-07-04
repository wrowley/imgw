
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
 * been opened. */
void
imgw_bmp_read_properties
    (const imgw_bmp_read *bmp_reader
    ,      unsigned      *width
    ,      unsigned      *height
    ,      unsigned      *bpp
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
