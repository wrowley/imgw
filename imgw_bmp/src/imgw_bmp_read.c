
#include "imgw_bmp/include/imgw_bmp_read.h"

#include <assert.h>

static
unsigned char
read_uchar
    (imgw_bmp_read *bmp_reader)
{
    int c = fgetc(bmp_reader->fp);
    bmp_reader->eof_reached = (c == EOF);
    return bmp_reader->eof_reached ? 0 : (unsigned char)c;
}

static
unsigned
read_uint_le
    (imgw_bmp_read *bmp_reader)
{
    unsigned int result = 0;
    result += read_uchar(bmp_reader) << 0;
    result += read_uchar(bmp_reader) << 8;
    return result;
}

static
unsigned
read_ulong_le
    (imgw_bmp_read *bmp_reader)
{
    unsigned long result = 0;
    result += read_uchar(bmp_reader) <<  0;
    result += read_uchar(bmp_reader) <<  8;
    result += read_uchar(bmp_reader) << 16;
    result += read_uchar(bmp_reader) << 24;
    return result;
}

static
int
read_header
    (imgw_bmp_read *bmp_reader)
{
    if ('B' != read_uchar(bmp_reader)) return 1;
    if ('M' != read_uchar(bmp_reader)) return 1;

    /* Bitmap file header */
    bmp_reader->file_size = read_ulong_le(bmp_reader);
    read_uint_le(bmp_reader); /* Reserved, meaningless */
    read_uint_le(bmp_reader); /* Reserved, meaningless */
    bmp_reader->pixel_array_offset = read_ulong_le(bmp_reader); /* Pixel array offset */

    /* DIB header (BITMAPINFOHEADER) */
    read_ulong_le(bmp_reader);
    bmp_reader->width = read_ulong_le(bmp_reader);  /* TODO: Signed ??? */
    bmp_reader->height = read_ulong_le(bmp_reader); /* TODO: Signed ??? */
    read_uint_le(bmp_reader); /* 1 layer */
    bmp_reader->bpp = read_uint_le(bmp_reader); /* 8 bpp */
    if (0 != read_ulong_le(bmp_reader)) return 1;  /* No compression */
    read_ulong_le(bmp_reader); /* bitmap size -> 0 fine for no compression */
    read_ulong_le(bmp_reader); /* Horizontal pixels per metre... 1080 per 50cm? */
    read_ulong_le(bmp_reader); /* Vertical pixels per metre... 1080 per 50cm? */
    if (0 != read_ulong_le(bmp_reader)) return 1; /* 2^N colors per palette */
    read_ulong_le(bmp_reader); /* The number of important colours */

    return 0;
}

int
imgw_bmp_read_open
    (const char          *filename
    ,      imgw_bmp_read *bmp_reader
    )
{
    bmp_reader->fp = fopen(filename, "rb");

    if (bmp_reader->fp == NULL) return 1;
    bmp_reader->eof_reached = 0;

    if (read_header(bmp_reader)) return 1;

    return bmp_reader->eof_reached;
}

void
imgw_bmp_read_close
    (imgw_bmp_read *bmp_reader)
{
    assert(bmp_reader->fp != NULL);
    fclose(bmp_reader->fp);
}

void
imgw_bmp_read_properties
    (const imgw_bmp_read *bmp_reader
    ,      unsigned      *width
    ,      unsigned      *height
    ,      unsigned      *bpp
    )
{
    *width  = bmp_reader->width;
    *height = bmp_reader->height;
    *bpp    = bmp_reader->bpp;
}

/* A small test function for the function/s in this file */
#ifdef IMGW_BMP_READ_MAIN
#include <stdlib.h>
int main()
{
    unsigned width  = 0;
    unsigned height = 0;
    unsigned bpp    = 0;
    imgw_bmp_read bmp_reader;
    int status =
        imgw_bmp_read_open
            ("test-24bit.bmp"
            ,&bmp_reader
            );

    if (status == 0)
    {
        imgw_bmp_read_properties
            (&bmp_reader
            ,&width
            ,&height
            ,&bpp
            );
        imgw_bmp_read_close(&bmp_reader);
    }

    printf("Properties of %s\n", "test-24bit.bmp");
    printf("    width  -> %d\n", width );
    printf("    height -> %d\n", height);
    printf("    bpp    -> %d\n", bpp   );

    return status;
}
#endif
