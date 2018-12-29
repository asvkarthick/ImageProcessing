// Most of the camera sensor stream Bayer format images to the SoC
// Many a times we do not have Bayer format images as the SoC will be either
// providing YUV or RGB frames to the application
// This application converts YUV420 frames to Bayer format for experiments
// that need Bayer format frames

// Compile this program with
// gcc convertYUV2bayer.c -o convertYUV2bayer

// Run this program with
// ./convertYUV2bayer ../yuv_files/coastguard_cif.yuv coastguard_bayer.yuv 352 288 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function prototype declarations
void scaleChrominance(unsigned char *inpBuf,
                      unsigned char *scaledBuf,
                      unsigned int width,
                      unsigned int height);

void yuv2rgb(unsigned char *yuv,
             unsigned char *rgb,
             unsigned int width,
             unsigned int height);

void convertBayer(unsigned char *rgb,
                  unsigned char *cfa,
                  unsigned int width,
                  unsigned int height,
                  unsigned int sensorAlignment);

// Main function definition
int main(int argc, char *argv[])
{
    FILE *fin, *fout;
    unsigned int width, height;
    int oneFramesize, numFrames, filesize, frame, sensorAlignment;
    unsigned char *yBuf, *cbBuf, *crBuf;
    unsigned char *yuv444;
    unsigned char *cfaBuf;
    unsigned char *ptr;
    unsigned char *rgb;

    // Checking the command line arguments
    if(argc < 6)
    {
        fprintf(stderr, "Usage: %s <YUV_file> <CFA_file> <width> <height> <sensorAlignment(0 - BGGR, 1 - RGGB, 2 - GRBG, 3 - GBRG)\n", argv[0]);
        return -1;
    }

    // Open the input YUV file
    if((fin = fopen(argv[1], "rb")) == NULL)
    {
        fprintf(stderr, "Error: Opening input YUV file.\n");
        return -1;
    }
    // Open the output Bayer Pattern Color Filter Array Video file
    if((fout = fopen(argv[2], "wb")) == NULL)
    {
        fprintf(stderr, "Error: Opening output CFA file.\n");
        return -1;
    }

    // Get the width, height and sensorAlignment
    width = atoi(argv[3]);
    height = atoi(argv[4]);
    sensorAlignment = atoi(argv[5]);

    // Find the input YUV filesize
    fseek(fin, 0, SEEK_END);
    filesize = ftell(fin);
    rewind(fin);

    // Calculate one framesize
    oneFramesize = width * height * 3 / 2;

    // Find the number of frames in the input video file
    numFrames = filesize / oneFramesize;
    fprintf(stdout, "Number of frames in the input file = %d\n", numFrames);

    // Allocate memory for the buffers
    yBuf = (unsigned char*)malloc(width * height); // For luminance
    cbBuf = (unsigned char*)malloc(width * height / 4); // For chrominance
    crBuf = (unsigned char*)malloc(width * height / 4);
    yuv444 = (unsigned char*)malloc(width * height * 3); // For full color YUV 4:4:4 video
    rgb = (unsigned char*)malloc(width * height * 3);
    cfaBuf = (unsigned char*)malloc(width * height); // For Bayer Pattern Video

    for(frame = 0; frame < numFrames; ++frame)
    {
        fprintf(stdout, "Reading %d frame...", frame + 1);
        // Read the luminance
        fread(yBuf, sizeof(unsigned char), width * height, fin);
        // Read the chrominance
        fread(cbBuf, sizeof(unsigned char), width * height / 4, fin);
        fread(crBuf, sizeof(unsigned char), width * height / 4, fin);
        fprintf(stdout, "Done.\n");
        
        fprintf(stdout, "Performing color space conversion...");
        // Copy the luminance
        memcpy(yuv444, yBuf, width * height);

        // Scale the chrominance
        // Base address for YUV 4:4:4 cb
        ptr = yuv444 + width * height;
        scaleChrominance(cbBuf, ptr, width >> 1, height >> 1);

        // Base address for YUV 4:4:4 cr
        ptr = yuv444 + 2 * width * height;
        scaleChrominance(crBuf, ptr, width >> 1, height >> 1);

        // Convert from YUV to RGB color space
        yuv2rgb(yuv444, rgb, width, height);
        fprintf(stdout, "Done.\n");

        fprintf(stdout, "Converting to Bayer Pattern...");
        convertBayer(rgb, cfaBuf, width, height, sensorAlignment);
        fprintf(stdout, "Done.\n");

        // Write the Bayer Pattern/Color Filter Array Image
        fwrite(cfaBuf, sizeof(unsigned char), width * height, fout);
    }

    // Close both the file pointers
    fclose(fin);
    fclose(fout);
    return 0;
}

void scaleChrominance(unsigned char *inpBuf,
                      unsigned char *scaledBuf,
                      unsigned int width,
                      unsigned int height)
{
    unsigned int row, col;

    // Performing Nearest Neighbor Implementation
    for(row = 0; row < height; row++)
    {
        for(col = 0; col < width; col++)
        {
            *scaledBuf++ = *inpBuf;
            *(scaledBuf + 2 * width) = *inpBuf;
            *(scaledBuf + 2 * width - 1) = *inpBuf;
            *scaledBuf++ = *inpBuf++;
        }
        scaledBuf += (2 * width);
    }
    
    return;
}

void yuv2rgb(unsigned char *yuv,
             unsigned char *rgb,
             unsigned int width,
             unsigned int height)
{
    unsigned int row, col;
    unsigned char *y, *cb, *cr;
    unsigned char *r, *g, *b;
    double R, G, B;
    double Y, U, V;

    // Base addresses of Luminance & Chrominance
    y = yuv;
    cb = yuv + width * height;
    cr = yuv + 2 * width * height;
    r = rgb;
    g = rgb + width * height;
    b = rgb + 2 * width * height;

    for(row = 0; row < height; ++row)
    {
        for(col = 0; col < width; ++col)
        {
            // YUV to RGB Color Space Conversion
            Y = *y++;
            U = *cb++;
            V = *cr++;
            R = (Y - 16) * 1.164 + (U - 128) * 0 + (V - 128) * 1.596;
            G = (Y - 16) * 1.164 + (U - 128) * -0.813 + (V - 128) * 0.391;
            B = (Y - 16) * 1.164 + (U - 128) * 2.018 + (V - 128) * 0;

            if(R < 0) R = 0;
            if(G < 0) G = 0;
            if(B < 0) B = 0;
            if(R > 255) R = 255;
            if(G > 255) G = 255;
            if(B > 255) B = 255;

            *r++ = (unsigned char)(unsigned int)R;
            *g++ = (unsigned char)(unsigned int)G;
            *b++ = (unsigned char)(unsigned int)B;
        }
    }
    return;
}

void convertBayer(unsigned char *rgb,
                  unsigned char *cfa,
                  unsigned int width,
                  unsigned int height,
                  unsigned int sensorAlignment)
{
    unsigned char *r, *g1, *g2, *b, *g;
    unsigned int row, col;
    unsigned char *ptr1, *ptr2, *ptr3, *ptr4;

    /*************************************************************************/
    /*                   Sensor Alignment                                    */
    /*                   0 - BGGR, 1 - RGGB, 2 - GRBG, 3 - GBRG              */
    /*************************************************************************/
    
    // Base addresses of R, G and B
    r  = rgb;
    g  = rgb + width * height;
    b  = rgb + 2 * width * height;

    if(sensorAlignment == 0)
    {
        ptr1 = cfa + width + 1;
        ptr2 = cfa + 1;
        ptr3 = cfa + width;
        ptr4 = cfa;
        r = r + width + 1;
        g1 = g + 1;
        g2 = g + width;
        b = b;
    }
    else if(sensorAlignment == 1)
    {
        ptr1 = cfa;
        ptr2 = cfa + 1;
        ptr3 = cfa + width;
        ptr4 = cfa + width + 1;
        r = r;
        g1 = g + 1;
        g2 = g + width;
        b = b + width + 1;
    }
    else if(sensorAlignment == 2)
    {
        ptr1 = cfa + 1;
        ptr2 = cfa;
        ptr3 = cfa + width + 1;
        ptr4 = cfa + width;
        r = r + 1;
        g1 = g;
        g2 = g + width + 1;
        b = b + width;
    }
    else if(sensorAlignment == 3)
    {
        ptr1 = cfa + width;
        ptr2 = cfa;
        ptr3 = cfa + width + 1;
        ptr4 = cfa + 1;
        r = r + width;
        g1 = g;
        g2 = g + width + 1;
        b = b + 1;
    }

    for(row = 0; row < (height >> 1); ++row)
    {
        for(col = 0; col < (width >> 1); ++col)
        {
            *ptr1++ = *r++;
            *ptr2++ = *g1++;
            *ptr3++ = *g2++;
            *ptr4++ = *b++;
            ptr1++;
            ptr2++;
            ptr3++;
            ptr4++;
            r++;
            g1++;
            g2++;
            b++;
        }
        ptr1 += width;
        ptr2 += width;
        ptr3 += width;
        ptr4 += width;
        r    += width;
        g1   += width;
        g2   += width;
        b    += width;
    }
    
    return;
}

