// This program converts YUV 4:2:0 to RGB

// Compile this program with
// gcc yuv2rgb.c -o yuv2rgb

// Run this program with
// ./yuv2rgb <yuv_file> <rgb_file> <width> <height>

// The conversion formula has been taken from
// https://www.scribd.com/document/117222158/Converting-Between-YUV-and-RGB

#include <stdio.h>
#include <stdlib.h>
#include "bmp.h"

#define CLIP(x, y, z) ((x < y) ? y : ((x > z) ? z : x))

void yuv2rgb(unsigned char *src, unsigned char *dst, int width, int height)
{
    int row, col;
    unsigned char y, u, v;
    unsigned char r, g, b;
    int c, d, e;

    for(row = 0; row < height; row++)
    {
        for(col = 0; col < width; col++)
        {
            y = src[row * width + col];
            u = src[width * height + (row >> 1) * (width >> 1) + (col >> 1)];
            v = src[width * height + (width >> 1) * (height >> 1) + (row >> 1) * (width >> 1) + (col >> 1)];

            c = y - 16;
            d = u - 128;
            e = v - 128;

            r = (unsigned char) CLIP(((298 * c + 409 * e + 128) >> 8), 0, 255);
            g = (unsigned char) CLIP(((298 * c  - 100 * d - 208 * 3 + 128) >> 8), 0, 255);
            b = (unsigned char) CLIP(((298 * c + 516 * d + 128) >> 8), 0, 255);

            dst[row * width * 3 + col * 3] = b;
            dst[row * width * 3 + col * 3 + 1] = g;
            dst[row * width * 3 + col * 3 + 2] = r;
        }
    }
}

void prepareBMP(struct BITMAPFILEHEADER *fheader, struct BITMAPINFOHEADER *iheader, int width, int height)
{
    fheader->signature = 0x4D42;
    fheader->size = 54 + width * height * 3;
    fheader->reserved1 = 0x00;
    fheader->reserved2 = 0x00;
    fheader->offset = 54;

    iheader->size = 40;
    iheader->width = width;
    iheader->height = height;
    iheader->num_planes = 1;
    iheader->num_bpp = 24;
    iheader->compression = 0;
    iheader->imagesize = width * height * 3;
    iheader->hres = 0;
    iheader->vres = 0;
    iheader->num_colors = 0;
    iheader->num_imp_colors = 0;
}

int main(int argc, char *argv[])
{
    FILE *fin, *fout;
    unsigned char *yuv_buffer;
    unsigned char *rgb_buffer;
    int width;
    int height;
    struct BITMAPFILEHEADER fheader;
    struct BITMAPINFOHEADER iheader;

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

    fread(yuv_buffer, 1, width * height * 3 / 2, fin);
    yuv2rgb(yuv_buffer, rgb_buffer, width, height);

    // Prepare BMP Header
    prepareBMP(&fheader, &iheader, width, height);

    // Write BMP Header
    fwrite(&fheader.signature, 1, sizeof(unsigned short), fout);
    fwrite(&fheader.size, 1, sizeof(unsigned int), fout);
    fwrite(&fheader.reserved1, 1, sizeof(unsigned short), fout);
    fwrite(&fheader.reserved2, 1, sizeof(unsigned short), fout);
    fwrite(&fheader.offset, 1, sizeof(unsigned int), fout);
    fwrite(&iheader, 1, sizeof(struct BITMAPINFOHEADER), fout);
    // Write pixels to BMP file
    fwrite(rgb_buffer, 1, width * height * 3, fout);

    free(yuv_buffer);
    free(rgb_buffer);

    fclose(fin);
    fclose(fout);

    return 0;
}
