
#include "imgw_bmp/include/imgw_bmp_write.h"

#include <stdio.h>

static
void
write_uchar
    (FILE          *fp
    ,unsigned char  u
    )
{
    fwrite(&u, 1, 1, fp);
}

static
void
write_uint_le
    (FILE          *fp
    ,unsigned       le
    )
{
    write_uchar(fp, (le & 0x00FF) >> 0);
    write_uchar(fp, (le & 0xFF00) >> 8);
}

static
void
write_ulong_le
    (FILE          *fp
    ,unsigned long  le
    )
{
    write_uchar(fp, (le & 0x000000FF) >>  0);
    write_uchar(fp, (le & 0x0000FF00) >>  8);
    write_uchar(fp, (le & 0x00FF0000) >> 16);
    write_uchar(fp, (le & 0xFF000000) >> 24);
}

int
imgw_bmp_write_8bit
    (const char          *filename
    ,const unsigned char *pixels
    ,      unsigned       width
    ,      unsigned       height
    )
{
    FILE *fp = fopen(filename, "w+");
    unsigned width_padded = (width + 3) / 4 * 4;
    unsigned idx, jdx;
    unsigned long pixel_array_offset = 
        14 +           /* Bitmap file header */
        40 +           /* DIB header (BITMAPINFOHEADER) */
        256 * 4        /* Colour table */
        ;
    unsigned long file_size =
        pixel_array_offset + 
        width_padded * height  /* Pixel array */
        ;

    if (fp == NULL) return 1;

    write_uchar(fp, 'B');
    write_uchar(fp, 'M');

    /* Bitmap file header */
    write_ulong_le(fp, file_size);
    write_uint_le(fp, 0); /* Reserved, meaningless */
    write_uint_le(fp, 0); /* Reserved, meaningless */
    write_ulong_le(fp, pixel_array_offset); /* Pixel array offset */

    /* DIB header (BITMAPINFOHEADER) */
    write_ulong_le(fp, 40);
    write_ulong_le(fp, width);  /* TODO: Signed ??? */
    write_ulong_le(fp, height); /* TODO: Signed ??? */
    write_uint_le(fp, 1); /* 1 layer */
    write_uint_le(fp, 8); /* 8 bpp */
    write_ulong_le(fp, 0);  /* No compression */
    write_ulong_le(fp, 0);  /* bitmap size -> 0 fine for no compression */
    write_ulong_le(fp, 2160); /* Horizontal pixels per metre... 1080 per 50cm? */
    write_ulong_le(fp, 2160); /* Vertical pixels per metre... 1080 per 50cm? */
    write_ulong_le(fp, 0); /* 2^N colors per palette */
    write_ulong_le(fp, 0); /* The number of important colours */

    /* Gray-scale colour table */
    for (idx = 0; idx < 256 ; idx++)
    {
        write_uchar(fp, idx);
        write_uchar(fp, idx);
        write_uchar(fp, idx);
        write_uchar(fp, 0);
    }

    /* Pixel array */
    for (jdx = 0; jdx < height; jdx++)
    {
        for (idx = 0; idx < width; idx++)
        {
            write_uchar(fp, pixels[(height - jdx - 1) * width + idx]);
        }
        /* Padding to 4 byte boundaries */
        for (       ; idx < width_padded; idx++)
        {
            write_uchar(fp, 0);
        }
    }

    fclose(fp);

    return 0;
}

/* A small test function for the function/s in this file */
#ifdef IMGW_BMP_WRITE_MAIN
#include <stdlib.h>
int main()
{
    unsigned width  = 1079;
    unsigned height = 719 ;
    unsigned x, y;
    int status;
    unsigned char *pixels = malloc(width * height);

    /* In this image, black should start off in the top left, and white should end up in the bottom right */
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            unsigned tone = (y * 0xFF / height + x * 0xFF / width) / 2;
            pixels[y * width + x] = (unsigned char)tone;
        }
    }

    status =
        imgw_bmp_write_8bit
            ("test-8bit.bmp"
            ,pixels
            ,width
            ,height
            );

    free(pixels);

    return status;
}
#endif
