#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void convertTo8Bit(unsigned char *in, unsigned char *out, int width, int height)
{
    unsigned short *inbuf = (unsigned short*) in;
    int row, col;

    for(row = 0; row < height; row++)
    {
        for(col = 0; col < width; col++)
        {
            out[row * width + col] = (unsigned char)(inbuf[row * width + col] >> 2) & 0xFF;
        }
    }
}

void crop(unsigned char *in, unsigned char *out, int inWidth, int inHeight, int outWidth, int outHeight)
{
    int row;

    in = in + ((inHeight - outHeight) / 2) * inWidth + ((inWidth - outWidth) / 2);
    for(row = 0; row < outHeight; row++)
    {
        memcpy(out, in, outWidth);
        out += outWidth;
        in += inWidth;
    }
}

int main(int argc, char *argv[])
{
    FILE *fin, *fout, *fcrop;
    unsigned char *inbuf, *outbuf, *cropbuf;
    int inWidth, inHeight, cropWidth, cropHeight;

    if(argc < 8)
    {
        fprintf(stderr, "Usage: %s <infile> <outfile> <cropfile> <inWidth> <inHeight> <cropWidth> <cropHeight>\n", argv[0]);
        return -1;
    }

    // Open input and output files
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
    if((fcrop = fopen(argv[3], "wb")) == NULL)
    {
        fprintf(stderr, "Error: Opening file %s\n", argv[3]);
        fclose(fin);
        fclose(fout);
        return -1;
    }

    inWidth = atoi(argv[4]);
    inHeight = atoi(argv[5]);
    cropWidth = atoi(argv[6]);
    cropHeight = atoi(argv[7]);

    // Allocate buffers
    inbuf = (unsigned char*) malloc(inWidth * inHeight * 2);
    outbuf = (unsigned char*) malloc(inWidth * inHeight);
    cropbuf = (unsigned char*) malloc(cropWidth * cropHeight);

    if((inWidth < cropWidth) || (inHeight < cropHeight))
        goto end;

    fread(inbuf, 1, inWidth * inHeight * 2, fin);
    convertTo8Bit(inbuf, outbuf, inWidth, inHeight);
    crop(outbuf, cropbuf, inWidth, inHeight, cropWidth, cropHeight);
    fwrite(outbuf, 1, inWidth * inHeight, fout);
    fwrite(cropbuf, 1, cropWidth * cropHeight, fcrop);
    fprintf(stdout, "Cropping done.\n");

end:
    free(inbuf);
    free(outbuf);
    free(cropbuf);

    fclose(fin);
    fclose(fout);
    fclose(fcrop);

    return 0;
}
