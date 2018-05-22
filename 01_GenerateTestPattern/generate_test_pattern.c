#include <stdio.h>
#include <stdlib.h>

unsigned char *frame_buffer;
unsigned char *frame_buffer_orig;

void fillBuffer(unsigned char *buffer, int width, int height)
{
    int row, col;
    unsigned char y, u, v;

    y = 0x00;
    u = 0x80;
    v = 0x80;

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
    int width, height, num_frames, frame;
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

    if((fout = fopen(argv[4], "wb")) == NULL)
    {
        fprintf(stderr, "Error: Opening file %s\n", argv[4]);
        return -1;
    }

    // Allocate memory
    frame_buffer = (unsigned char*) malloc(width * height * 2);
    frame_buffer_orig = (unsigned char*) malloc(width * height * 2);

    fillBuffer(frame_buffer, width, height);
    fillBuffer(frame_buffer_orig, width, height);

    for(frame = 0; frame < num_frames; frame++)
    {
        fwrite(frame_buffer, 1, width * height * 2, fout);
    }

    fclose(fout);
    free(frame_buffer);
    free(frame_buffer_orig);

    return 0;
}
