// This program upscales input YUV both horizontally and vertically by a factor of 2
// This program assumes that the input YUV is in YUV420 Planar format

// Compile this program with
// gcc upscale_yuv.c -o upscale_yuv

// Run this program with
// ./upscale akiyo_qcif.yuv akiyo_cif.yuv 176 144

#include <stdio.h>
#include <stdlib.h>

void NearestNeighbor(unsigned char *in, unsigned char *out, unsigned int width, unsigned int height);
void Process(unsigned char *in, unsigned char *out, unsigned int width, unsigned int height);

unsigned char *yIn, *cbIn, *crIn;
unsigned char *yOut, *cbOut, *crOut;

int main(int argc, char *argv[])
{
    FILE *fin, *fout;
    unsigned int width, height;
    unsigned char *inbuf, *outbuf;
    unsigned int NoOfFrames, filesize, frame, framesize;

    if(argc < 5) {
        fprintf(stderr, "Usage: %s <input_yuv_file> <output_yuv_file> <width> <height>\n", argv[0]);
        return -1;
    }

    width = atoi(argv[3]);
    height = atoi(argv[4]);

    if ((fin = fopen(argv[1], "rb")) == NULL) {
        printf ("Error: Opening YUV file...\n");
        return -1;
    }
    if ((fout = fopen(argv[2], "wb")) == NULL) {
        printf ("Error: Opening output YUV file...\n");
        return -1;
    }

    framesize = width * height * 3 / 2;
    inbuf = (unsigned char*) malloc(framesize);
    outbuf = (unsigned char*) malloc((2 * width) * (2 * height) * 3 / 2);

    if (inbuf == NULL) {
        printf ("Error: Memory allocation of Input buffer.\n");
        return -1;
    }
    if (outbuf == NULL) {
        printf ("Error: Memory allocation of Output buffer.\n");
        return -1;
    }

    yIn = inbuf;
    cbIn = yIn + width * height;
    crIn = cbIn + (width >> 1) * (height >> 1);

    yOut = outbuf;
    cbOut = yOut + width * height * 4;
    crOut = cbOut + width * height;

    // Finding the Number of Frames
    fseek(fin, 0, SEEK_END);
    filesize = ftell(fin);
    rewind(fin);
    NoOfFrames = filesize / framesize;
    printf ("Number of Frames = %d\n", NoOfFrames);

    /************************************************************************
     * Read one frame & upscale the frame									*
     * Repeat the above process for all the frames in the file				*
     ************************************************************************/
    for (frame = 0; frame < NoOfFrames; frame++) {
        fread(inbuf, sizeof(unsigned char), framesize, fin);
        NearestNeighbor(inbuf, outbuf, (unsigned int) width, (unsigned int) height);
        fwrite(outbuf, sizeof(unsigned char), framesize * 4, fout);
        printf ("Processed Frame %d.\n", frame);
    }

    fclose(fin);
    fclose(fout);
    return 0;
}

void NearestNeighbor(unsigned char *in, unsigned char *out, unsigned int width, unsigned int height)
{
    unsigned char *inY, *inCb, *inCr, *outY, *outCb, *outCr;

    // Calculation of Input & Output YCbCr positions
    inY = in;
    inCb = inY + width * height;
    inCr = inCb + (width >> 1) * (height >> 1);
    outY = out;
    outCb = outY + 4 * width * height;
    outCr = outCb + width * height;

    // Processing the Luminance(Y)
    Process(inY, outY, width, height);
    // Process the Chrominance(CbCr)
    Process(inCb, outCb, width >> 1, height >> 1);
    Process(inCr, outCr, width >> 1, height >> 1);

    return;
}

void Process(unsigned char *in, unsigned char *out, unsigned int width, unsigned int height)
{
    unsigned int row, col;

    for (row = 0; row < height; row++) {
        for (col = 0; col < width; col++) {
            *out++                 = *in;
            *(out + 2 * width)     = *in;
            *(out + 2 * width - 1) = *in;
            *out++                 = *in++;
        }
        out += 2 * width;
    }

    return;
}
