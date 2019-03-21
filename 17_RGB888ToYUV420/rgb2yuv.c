// This program converts RGB to YUV 4:2:0

// Compile this program with
// gcc rgb2yuv.c -o rgb2yuv

// Run this program with
// ./rgb2yuv <rgb_file> <yuv_file> <width> <height>

// The conversion formula has been taken from
// https://www.scribd.com/document/117222158/Converting-Between-YUV-and-RGB

#include <stdio.h>
#include <stdlib.h>

#define CLIP(x, y, z) ((x < y) ? y : ((x > z) ? z : x))

void rgb2yuv(unsigned char *src, unsigned char *dst, int width, int height)
{
    int row, col;
    unsigned char y, u, v;
    unsigned char r, g, b;

    for(row = 0; row < height; row++)
    {
        for(col = 0; col < width; col++)
        {
            r = src[row * width * 3 + col * 3];
            g = src[row * width * 3 + col * 3 + 1];
            b = src[row * width * 3 + col * 3 + 2];

            y = (unsigned char) (((66 * r + 129 * g + 25 * b + 128) >> 8) + 16);
            u = (unsigned char) (((-38 * r - 74 * g + 112 * b + 128) >> 8) + 128);
            v = (unsigned char) (((112 * r - 94 * g - 18 * b + 128) >> 8) + 128);

            dst[row * width + col] = y;
            dst[width * height + (row >> 1) * (width >> 1) + (col >> 1)] = u;
            dst[width * height + (width >> 1) * (height >> 1) + (row >> 1) * (width >> 1) + (col >> 1)] = v;
        }
    }
}

int main(int argc, char *argv[])
{
    FILE *fin, *fout;
    unsigned char *yuv_buffer;
    unsigned char *rgb_buffer;
    int width;
    int height;

    if(argc < 5)
    {
        fprintf(stderr, "Usage: %s <input_yuv_file> <output_rgb_file> <width> <height>\n", argv[0]);
        return -1;
    }

    if((fin = fopen(argv[1], "rb")) == NULL)
    {
        fprintf(stderr, "Error: Opening file %s\n", argv[1]);
        return -1;
    }
    if((fout = fopen(argv[2], "wb")) == NULL)
    {
        fprintf(stderr, "Error: Opening file %s\n", argv[2]);
        fclose(fin);
        return -1;
    }

    width = atoi(argv[3]);
    height = atoi(argv[4]);

    /* Allocating memory for YUV 4:2:0 buffer */
    yuv_buffer = (unsigned char*) malloc(width * height * 3 / 2);
    /* Allocating memory for RGB buffer */
    rgb_buffer = (unsigned char*) malloc(width * height * 3);

    fread(rgb_buffer, 1, width * height * 3, fin);
    rgb2yuv(rgb_buffer, yuv_buffer, width, height);
    fwrite(yuv_buffer, 1, width * height * 3 / 2, fout);

    free(yuv_buffer);
    free(rgb_buffer);

    fclose(fin);
    fclose(fout);

    return 0;
}
