#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <string.h>

void convertYUV420P2SP(unsigned char *inbuf, unsigned char *outbuf, int width, int height)
{
    unsigned char *uPtr, *vPtr, *uvPtr;
    uPtr = inbuf + width * height;
    vPtr = uPtr + (width >> 1) * (height >> 1);
    uvPtr = outbuf + width * height;
    memcpy(outbuf, inbuf, width * height);
    for (int row = 0; row < (height >> 1); row++) {
        for (int col = 0; col < (width >> 1); col++) {
            *uvPtr++ = *uPtr++;
            *uvPtr++ = *vPtr++;
        }
    }
}

int main(int argc, char *argv[])
{
    std::cout << "Program to convert YUV420P to YUV420SP" << std::endl;
    if (argc < 5) {
        std::cerr << "Usage: " << argv[0] << " <inputfile> <output file> <width> <height>" << std::endl;
        return -1;
    }

    auto src_filename  = argv[1];
    auto dst_filename  = argv[2];
    auto width   = atoi(argv[3]);
    auto height  = atoi(argv[4]);

    std::vector<unsigned char> inbuf(width * height * 3 / 2);
    std::vector<unsigned char> outbuf(width * height * 3 / 2);

    std::ifstream fin(src_filename, std::ios::binary);
    std::ofstream fout(dst_filename, std::ios::binary);
    if (!fin) {
        std::cerr << "Error: Opening file " << src_filename << std::endl;
        return -1;
    }

    fin.read((char*)(inbuf.data()), width * height * 3 / 2);
    convertYUV420P2SP(inbuf.data(), outbuf.data(), width, height);
    fout.write((char*)(outbuf.data()), width * height * 3 / 2);

    fin.close();
    fout.close();
    return 0;
}
