// Compile this program with
// gcc convertYUV444To422.c -o convertYUV444To422

// Run this program with
//

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    FILE *fin, *fout;
    int num_frames, framesize, width, height;

    if(argc < 5)
    {
        fprintf(stderr, "Usage: %s <input_yuv444 file> <output yuv422 file> <width> <height>\n", argv[0]);
        return -1;
    }

    if((fin = fopen(argv[1], "rb")) == NULL)
    {
        fprintf(stderr, "Error: Opening %s\n", argv[1]);
        return -1;
    }

    if((fout = fopen(argv[2], "wb")) == NULL)
    {
        fprintf(stderr, "Error: Opening %s\n", argv[2]);
        return -1;
    }

    width = atoi(argv[3]);
    height = atoi(argv[4]);

    fclose(fin);
    fclose(fout);

    return 0;
}
