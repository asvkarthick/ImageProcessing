// This program generates YUV (YUV422 - YUYV) file with frames of specified color

// Compile this program with
// gcc generate_test_pattern.c -o generate_test_pattern

// Run this progam with the following command to generate Yellow color frames
// ./generate_test_pattern 352 288 300 output.yuv 5

#include <stdio.h>
#include <stdlib.h>

unsigned char *frame_buffer;

void fillBuffer(unsigned char *buffer, int width, int height, int color)
{
    int row, col;
    unsigned char y, u, v;

    y = 0x00;
    u = 0x80;
    v = 0x80;

    if(color == 0)
    {
        y = 0x00;
        u = 0x00;
        v = 0x00;
    }
    else if(color == 1)
    {
        y = 0x00;
        u = 0x00;
        v = 0xFF;
    }
    else if(color == 2)
    {
        y = 0x00;
        u = 0xFF;
        v = 0x00;
    }
    else if(color == 3)
    {
        y = 0x00;
        u = 0xFF;
        v = 0xFF;
    }
    else if(color == 4)
    {
        y = 0xFF;
        u = 0x00;
        v = 0x00;
    }
    else if(color == 5)
    {
        y = 0xFF;
        u = 0x00;
        v = 0xFF;
    }
    else if(color == 6)
    {
        y = 0xFF;
        u = 0xFF;
        v = 0x00;
    }
    else if(color == 7)
    {
        y = 0xFF;
        u = 0xFF;
        v = 0xFF;
    }
    else if(color == 8)
    {
        y = 0x00;
        u = 0x80;
        v = 0x80;
    }
    else if(color == 9)
    {
        y = 0xFF;
        u = 0x80;
        v = 0x80;
    }

    for(row = 0; row < height; row++)
    {
        for(col = 0; col < (width >> 1); col++)
        {
            *buffer++ = y;
            *buffer++ = u;
            *buffer++ = y;
            *buffer++ = v;
        }
    }
}

int main(int argc, char* argv[])
{
    int width, height, num_frames, frame, color;
    FILE *fout;

    if(argc < 6)
    {
        fprintf(stderr, "Usage: %s <width> <height> <numFrames> <filename> <color>\n", argv[0]);
        printf("Color can have one of the following values\n");
        printf("0 - Green (Y = 0x00 U = 0x00 V = 0x00) \n");
        printf("1 - Red   (Y = 0x00 U = 0x00 V = 0xFF) \n");
        printf("2 - Blue  (Y = 0x00 U = 0xFF V = 0x00) \n");
        printf("3 - Purple(Y = 0x00 U = 0xFF V = 0xFF) \n");
        printf("4 - Green (Y = 0xFF U = 0x00 V = 0x00) \n");
        printf("5 - Yellow(Y = 0xFF U = 0x00 V = 0xFF) \n");
        printf("6 - Cyan  (Y = 0xFF U = 0xFF V = 0x00) \n");
        printf("7 - Pink  (Y = 0xFF U = 0xFF V = 0xFF) \n");
        printf("8 - Black (Y = 0x00 U = 0x80 V = 0x80) \n");
        printf("9 - White (Y = 0xFF U = 0x80 V = 0x80) \n");
        return -1;
    }

    width = atoi(argv[1]);
    height = atoi(argv[2]);
    num_frames = atoi(argv[3]);
    color = atoi(argv[5]);

    if((fout = fopen(argv[4], "wb")) == NULL)
    {
        fprintf(stderr, "Error: Opening file %s\n", argv[4]);
        return -1;
    }

    // Allocate memory
    frame_buffer = (unsigned char*) malloc(width * height * 2);

    fillBuffer(frame_buffer, width, height, color);

    for(frame = 0; frame < num_frames; frame++)
    {
        fwrite(frame_buffer, 1, width * height * 2, fout);
        printf("Frame %d\n", frame);
    }
    printf("Done.\n");

    fclose(fout);
    free(frame_buffer);

    return 0;
}
