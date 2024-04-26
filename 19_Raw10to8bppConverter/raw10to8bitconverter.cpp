#include <iostream>
#include <fstream>
#include <vector>

int main(int argc, char *argv[]) {
  auto srcFilename = argv[1];
  auto dstFilename = argv[2];
  auto width = atoi(argv[3]);
  auto height = atoi(argv[4]);

  std::cout << "Input filename : " << srcFilename << std::endl;
  std::cout << "Output filename : " << dstFilename << std::endl;
  std::cout << "Resolution : " << width << " x " << height << std::endl;

  std::vector<unsigned char> inbuf(width * height * 1.25);
  std::vector<unsigned char> outbuf(width * height);

  std::ifstream fin(srcFilename, std::ios::binary);
  std::ofstream fout(dstFilename, std::ios::binary);
  if (!fin) {
    std::cerr << "Error: Opening " << srcFilename << std::endl;
    return -1;
  }

  fin.read(reinterpret_cast<char*>(inbuf.data()), width * height * 1.25);

  auto srcPtr = inbuf.data();
  auto dstPtr = outbuf.data();

  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width / 4; col++) {
      *dstPtr++ = *srcPtr++;
      *dstPtr++ = *srcPtr++;
      *dstPtr++ = *srcPtr++;
      *dstPtr++ = *srcPtr++;
      srcPtr++;
    }
  }

  fout.write(reinterpret_cast<char*>(outbuf.data()), width * height);

  fin.close();
  fout.close();
  return 0;
}
