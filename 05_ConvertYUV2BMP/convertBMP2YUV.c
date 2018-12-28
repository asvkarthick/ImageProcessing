#include <stdio.h>
#include <stdlib.h>

// BMP Header is 54 bytes

typedef struct {
	unsigned short type;
	unsigned long size;
	unsigned short reserved1;
	unsigned short reserved2;
	unsigned long offsetbits;
} BITMAPFILEHEADER;

typedef struct {
	unsigned long size;
	unsigned long width;
	unsigned long height;
	unsigned short planes;
	unsigned short bitcount;
	unsigned long compression;
	unsigned long sizeimage;
	long xpelspermeter;
	long ypelspermeter;
	unsigned long colorsused;
	unsigned long colorsimportant;
} BITMAPINFOHEADER;

int main(void)
{

    return 0;
}
