#include <stdio.h>
#include <string.h>
#include <stdlib.h>

unsigned char *frame_buffer;
unsigned char *frame_buffer_orig;
unsigned char *block_buffer;

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

void addBlock(unsigned char* image_buffer, int width_image, int height_image,
              unsigned char* block_buffer, int width_block, int height_block,
              int x, int y)
{
    int row;
    for(row = 0; row < height_block; row++)
    {
        memcpy(image_buffer + (row + y) * width_image * 2 + x * 2, block_buffer + row * 2 * width_block, width_block * 2);
    }
}

int x_next = 0, y_next = 0;

void calculateNextCoordinate(int* x, int* y, int width_image, int height_image, int width_block, int height_block)
{
    int x_prev = *x, y_prev = *y;
    int x_next, y_next;
    int update_y = 0;

    if(x_prev + width_block > width_image)
    {
        x_next = 0;
        update_y = 1;
    }
    else
        x_next = x_prev + width_block;

    y_next = y_prev;

    if(update_y)
    {
        if(y_prev + height_block > height_image)
            y_next = 0;
        else
            y_next = y_prev + height_block;
    }

    *x = x_next;
    *y = y_next;
}

int main(int argc, char* argv[])
{
    int width, height, num_frames, frame;
    int block_width, block_height;
    FILE *fout;

    if(argc < 7)
    {
        fprintf(stderr, "Usage: %s <image-width> <image-height> <numFrames> <filename> <block-width> <block-height>\n", argv[0]);
        return -1;
    }

    width = atoi(argv[1]);
    height = atoi(argv[2]);
    num_frames = atoi(argv[3]);
    block_width = atoi(argv[5]);
    block_height = atoi(argv[6]);

    if((fout = fopen(argv[4], "wb")) == NULL)
    {
        fprintf(stderr, "Error: Opening file %s\n", argv[4]);
        return -1;
    }

    // Allocate memory
    frame_buffer = (unsigned char*) malloc(width * height * 2);
    frame_buffer_orig = (unsigned char*) malloc(width * height * 2);
    block_buffer = (unsigned char*) malloc(block_width * block_height * 2);

    fillBuffer(frame_buffer, width, height, 8);
    fillBuffer(frame_buffer_orig, width, height, 8);
    fillBuffer(block_buffer, block_width, block_height, 9);

    for(frame = 0; frame < num_frames; frame++)
    {
        addBlock(frame_buffer, width, height, block_buffer, block_width, block_height, x_next, y_next);
        calculateNextCoordinate(&x_next, &y_next, width, height, block_width, block_height);
        fwrite(frame_buffer, 1, width * height * 2, fout);
    }

    fclose(fout);
    free(frame_buffer);
    free(frame_buffer_orig);
    free(block_buffer);

    return 0;
}
