
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

void
imgw_bmp_read_pixel_array
    (imgw_bmp_read *bmp_reader
    ,unsigned char *pixel_array
    )
{
    const unsigned width        = bmp_reader->width;
    const unsigned height       = bmp_reader->height;
    const unsigned width_padded = (width * 3 + 3) / 4 * 4;

    if (bmp_reader->bpp == 24)
    {
        unsigned x, y;

        fseek(bmp_reader->fp, bmp_reader->pixel_array_offset, SEEK_SET);

        /* Pixel array */
        for (y = 0; y < height; y++)
        {
            unsigned bytes_in_row = 0;
            for (x = 0; x < width; x++)
            {
                /* Convert bottom-to-top to top-to-bottom */
                unsigned i = (height - y - 1) * width + x;

                pixel_array[i * 3 + 0] = read_uchar(bmp_reader); /* Blue */
                pixel_array[i * 3 + 1] = read_uchar(bmp_reader); /* Green */
                pixel_array[i * 3 + 2] = read_uchar(bmp_reader); /* Red */

                bytes_in_row += 3;
            }
            /* Padding to 4 byte boundaries */
            for (       ; bytes_in_row < width_padded; bytes_in_row++)
            {
                read_uchar(bmp_reader);
            }
        }
    }
    else
    {
        /* TODO: implement */
        unsigned idx;
        for (idx = 0; idx < width * height; idx++)
        {
            pixel_array[3 * idx + 0] = 0;
            pixel_array[3 * idx + 1] = 0;
            pixel_array[3 * idx + 2] = 0;
        }
    }
}

/* A small test function for the function/s in this file */
#ifdef IMGW_BMP_READ_MAIN
#include <stdlib.h>
#include "imgw_bmp/include/imgw_bmp_write.h"
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
        unsigned y, x;
        unsigned char *pixel_array;
        imgw_bmp_read_properties
            (&bmp_reader
            ,&width
            ,&height
            ,&bpp
            );
        if (bpp != 24) abort();

        pixel_array = malloc(width * height * 3);

        imgw_bmp_read_pixel_array
            (&bmp_reader
            ,pixel_array
            );

        printf("Properties of %s\n", "test-24bit.bmp");
        printf("    width  -> %d\n", width );
        printf("    height -> %d\n", height);
        printf("    bpp    -> %d\n", bpp   );

        for (y = 0; y < height; y++)
        {
            for (x = 0; x < width; x++)
            {
                int i = y * width + x;

                int b = pixel_array[i * 3 + 0];
                int g = pixel_array[i * 3 + 1];
                int r = pixel_array[i * 3 + 2];

                unsigned xscale = width - x;

                b = b * xscale / width; /* Scale down colours by distance from left */
                g = g * xscale / width; /* Scale down colours by distance from left */
                r = r * xscale / width; /* Scale down colours by distance from left */

                pixel_array[i * 3 + 0] = (unsigned char)r; /* Rotate colours */
                pixel_array[i * 3 + 1] = (unsigned char)b; /* Rotate colours */
                pixel_array[i * 3 + 2] = (unsigned char)g; /* Rotate colours */
            }
        }

        status =
            imgw_bmp_write_24bit
                ("test-24bit-modified.bmp"
                ,pixel_array
                ,width
                ,height
                );

        free(pixel_array);
        imgw_bmp_read_close(&bmp_reader);
    }

    return status;
}
#endif
