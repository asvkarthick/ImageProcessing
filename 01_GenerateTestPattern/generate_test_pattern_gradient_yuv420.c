// This program generates YUV (YUV422 - YUYV) file with frames of specified color

// Compile this program with
// gcc generate_test_pattern.c -o generate_test_pattern

// Run this progam with the following command to generate Yellow color frames
// ./generate_test_pattern 352 288 300 output.yuv 5

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned char *frame_buffer;

void fillBuffer(unsigned char *buffer, int width, int height, int color)
{
    int row, col;
    unsigned char y, u, v;

    y = (unsigned char) color;
    u = 0x80;
    v = 0x80;

    memset(buffer, y, width * height);
    memset(buffer + width * height, u, width * height / 4);
    memset(buffer + width * height * 5 / 4, v, width * height / 4);
}

int main(int argc, char* argv[])
{
    int width, height, num_frames, frame, color;
    FILE *fout;

    if(argc < 5)
    {
        fprintf(stderr, "Usage: %s <width> <height> <numFrames> <filename>\n", argv[0]);
        return -1;
    }

    width = atoi(argv[1]);
    height = atoi(argv[2]);
    num_frames = atoi(argv[3]);

    if((fout = fopen(argv[4], "wb")) == NULL)
    {
        fprintf(stderr, "Error: Opening file %s\n", argv[4]);
        return -1;
    }

    // Allocate memory
    frame_buffer = (unsigned char*) malloc(width * height * 3 / 2);

    color = 0;
    for(frame = 0; frame < num_frames; frame++)
    {
        fillBuffer(frame_buffer, width, height, color);
        fwrite(frame_buffer, 1, width * height * 3 / 2, fout);
        printf("Frame %d\n", frame);
        color = (color + 1) % 256;
    }
    printf("Done.\n");

    fclose(fout);
    free(frame_buffer);

    return 0;
}
