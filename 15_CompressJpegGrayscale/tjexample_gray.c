/*
 * Copyright (C)2011-2012, 2014-2015, 2017 D. R. Commander.
 *                                         All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name of the libjpeg-turbo Project nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS",
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *Program to compress grayscale image in JPEG format
 *
 * This requires external library called libjpeg-turbo and I will add
 * steps to link with that library shortly
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <turbojpeg.h>

#define DEFAULT_SUBSAMP  TJSAMP_444
#define DEFAULT_QUALITY  95

const char *subsampName[TJ_NUMSAMP] = {
  "4:4:4", "4:2:2", "4:2:0", "Grayscale", "4:4:0", "4:1:1"
};

int main(int argc, char **argv)
{
    int outSubsamp = -1, outQual = -1;
    int flags = 0;
    int width, height;
    FILE *jpegFile = NULL;
    unsigned char *imgBuf = NULL, *jpegBuf = NULL;
    int retval = 0, i, pixelFormat = TJPF_UNKNOWN;
    tjhandle tjInstance = NULL;

    FILE *fin;
    width = 352;
    height = 288;
    outSubsamp = TJSAMP_GRAY;
    pixelFormat = TJPF_GRAY;
    imgBuf = (unsigned char*) malloc(width * height);
    fin = fopen(argv[1], "rb");
    fread(imgBuf, 1, width * height, fin);
    fclose(fin);

    printf("Input Image:  %d x %d pixels\n", width, height);

    /* Output image format is JPEG.  Compress the uncompressed image. */
    unsigned long jpegSize = 0;

    outQual = DEFAULT_QUALITY;
    outQual = 40;
    printf(", %s subsampling, quality = %d\n", subsampName[outSubsamp],
           outQual);

    if ((tjInstance = tjInitCompress()) == NULL)
    {
        fprintf(stderr, "Error: Initializing compressor\n");
        return -1;
    }
    if (tjCompress2(tjInstance, imgBuf, width, 0, height, pixelFormat,
                    &jpegBuf, &jpegSize, outSubsamp, outQual, flags) < 0)
    {
        fprintf(stderr, "Error: Compressing image\n");
        return -1;
    }
    tjDestroy(tjInstance);  tjInstance = NULL;

    /* Write the JPEG image to disk. */
    if ((jpegFile = fopen(argv[2], "wb")) == NULL)
    {
        fprintf(stderr, "Error: Opening output file\n");
        return -1;
    }
    if (fwrite(jpegBuf, jpegSize, 1, jpegFile) < 1)
    {
        fprintf(stderr, "Error: Writing output file\n");
        return -1;
    }
    tjDestroy(tjInstance);  tjInstance = NULL;
    fclose(jpegFile);  jpegFile = NULL;
    tjFree(jpegBuf);  jpegBuf = NULL;

bailout:
    if (imgBuf) tjFree(imgBuf);
    if (tjInstance) tjDestroy(tjInstance);
    if (jpegBuf) tjFree(jpegBuf);
    if (jpegFile) fclose(jpegFile);
    return retval;
}
