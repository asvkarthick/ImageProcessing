#include <stdio.h>

unsigned char *frame_buffer;
unsigned char *frame_buffer_orig;

void fillBuffer(unsigned char *buffer, int width, int height)
{
    int row, col;

    for(row = 0; row < height; row++)
    {
        for(col = 0; col < width; col++)
        {
            *buffer++ = 0x00;
            *buffer++ = 0x80;
        }
    }
}

int main(int argc, char* argv[])
{
    int width, height, num_frames, frame;
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
