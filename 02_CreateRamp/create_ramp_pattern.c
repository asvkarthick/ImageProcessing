#include <stdio.h>
#include <stdlib.h>

unsigned char *frame_buffer;

void createRamp(unsigned char *buffer, int width, int height)
{
    int row, col;
    unsigned char pixel_value;

    for(row = 0; row < height; row++)
    {
        pixel_value = (unsigned char)(int)((float)row / (float)height * 255.0f);
        for(col = 0; col < width; col++)
        {
            *buffer++ = pixel_value;
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

    createRamp(frame_buffer, width, height);

    for(frame = 0; frame < num_frames; frame++)
    {
        fwrite(frame_buffer, 1, width * height * 2, fout);
    }

    fclose(fout);
    free(frame_buffer);

    return 0;
}
