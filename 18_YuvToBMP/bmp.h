#include <stdio.h>

struct BITMAPFILEHEADER
{
    unsigned short signature;
    unsigned int size;
    unsigned short reserved1;
    unsigned short reserved2;
    unsigned int offset;
};

struct BITMAPINFOHEADER
{
    unsigned int size;
    unsigned int width;
    unsigned int height;
    unsigned short num_planes;
    unsigned short num_bpp;
    unsigned int compression;
    unsigned int imagesize;
    unsigned int hres;
    unsigned int vres;
    unsigned int num_colors;
    unsigned int num_imp_colors;
};
