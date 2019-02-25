#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void crop(unsigned char *in, unsigned char *out, int inWidth, int inHeight, int outWidth, int outHeight)
{
    int row;

    // For top-left cropping
    // in = in + ((inHeight - outHeight) / 2) * inWidth + ((inWidth - outWidth) / 2);
    for(row = 0; row < outHeight; row++)
    {
        memcpy(out, in, outWidth * 2);
        out += outWidth * 2;
        in += inWidth * 2;
    }
}

int main(int argc, char *argv[])
{
    FILE *fin, *fcrop;
    unsigned char *inbuf, *cropbuf;
    int inWidth, inHeight, cropWidth, cropHeight;

    if(argc < 7)
    {
        fprintf(stderr, "Usage: %s <infile> <cropfile> <inWidth> <inHeight> <cropWidth> <cropHeight>\n", argv[0]);
        return -1;
    }

    // Open input and output files
    if((fin = fopen(argv[1], "rb")) == NULL)
    {
        fprintf(stderr, "Error: Opening file %s\n", argv[1]);
        return -1;
    }
    if((fcrop = fopen(argv[2], "wb")) == NULL)
    {
        fprintf(stderr, "Error: Opening file %s\n", argv[3]);
        fclose(fin);
        return -1;
    }

    inWidth = atoi(argv[3]);
    inHeight = atoi(argv[4]);
    cropWidth = atoi(argv[5]);
    cropHeight = atoi(argv[6]);

    // Allocate buffers
    inbuf = (unsigned char*) malloc(inWidth * inHeight * 2);
    cropbuf = (unsigned char*) malloc(cropWidth * cropHeight * 2);

    if((inWidth < cropWidth) || (inHeight < cropHeight))
        goto end;

    fread(inbuf, 1, inWidth * inHeight * 2, fin);
    crop(inbuf, cropbuf, inWidth, inHeight, cropWidth, cropHeight);
    fwrite(cropbuf, 1, cropWidth * cropHeight * 2, fcrop);
    fprintf(stdout, "Cropping done.\n");

end:
    free(inbuf);
    free(cropbuf);

    fclose(fin);
    fclose(fcrop);

    return 0;
}
