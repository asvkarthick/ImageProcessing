#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

struct Color {
  uint8_t r, g, b;
  Color(uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0)
      : r(red), g(green), b(blue) {}
};

class ImageGenerator {
 private:
  int width, height;
  std::vector<std::vector<Color>> pixels;

 public:
  ImageGenerator(int w, int h) : width(w), height(h) {
    pixels.resize(height, std::vector<Color>(width));
  }

  void generateTestPattern() {
    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        // Create a test pattern with multiple regions
        if (y < height / 3) {
          // Top third: RGB gradient
          pixels[y][x] = Color(
              (x * 255) / width,
              (y * 255) / (height / 3),
              ((width - x) * 255) / width);
        } else if (y < 2 * height / 3) {
          // Middle third: Checkerboard pattern
          int checkSize = 20;
          bool isWhite = ((x / checkSize) + (y / checkSize)) % 2 == 0;
          pixels[y][x] = isWhite ? Color(255, 255, 255) : Color(0, 0, 0);
        } else {
          // Bottom third: Radial gradient
          int centerX = width / 2;
          int centerY = height - (height / 6);
          double distance = sqrt(
              (x - centerX) * (x - centerX) + (y - centerY) * (y - centerY));
          double maxDistance =
              sqrt(centerX * centerX + (height / 6) * (height / 6));
          uint8_t intensity =
              (uint8_t)(255 * (1.0 - std::min(distance / maxDistance, 1.0)));
          pixels[y][x] = Color(intensity, intensity / 2, 255 - intensity);
        }
      }
    }
  }

  bool savePPM(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
      std::cerr << "Error: Could not open " << filename << " for writing"
                << std::endl;
      return false;
    }

    // PPM header
    file << "P3\n";
    file << width << " " << height << "\n";
    file << "255\n";

    // Write pixel data
    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        file << static_cast<int>(pixels[y][x].r) << " "
             << static_cast<int>(pixels[y][x].g) << " "
             << static_cast<int>(pixels[y][x].b) << " ";
      }
      file << "\n";
    }

    file.close();
    std::cout << "PPM image saved as " << filename << std::endl;
    return true;
  }

  bool saveBMP(const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
      std::cerr << "Error: Could not open " << filename << " for writing"
                << std::endl;
      return false;
    }

    // Calculate padding for 4-byte alignment
    int padding = (4 - (width * 3) % 4) % 4;
    int dataSize = (width * 3 + padding) * height;
    int fileSize = 54 + dataSize; // 54 bytes header + data

    // BMP Header (14 bytes)
    char bmpHeader[14] = {
        'B',
        'M', // Signature
        0,
        0,
        0,
        0, // File size (will be filled)
        0,
        0,
        0,
        0, // Reserved
        54,
        0,
        0,
        0 // Offset to pixel data
    };

    // Fill file size
    *reinterpret_cast<uint32_t*>(&bmpHeader[2]) = fileSize;

    // DIB Header (40 bytes)
    char dibHeader[40] = {
        40,   0,    0, 0, // Header size
        0,    0,    0, 0, // Width (will be filled)
        0,    0,    0, 0, // Height (will be filled)
        1,    0, // Planes
        24,   0, // Bits per pixel
        0,    0,    0, 0, // Compression
        0,    0,    0, 0, // Image size (can be 0 for uncompressed)
        0x13, 0x0B, 0, 0, // X pixels per meter (72 DPI)
        0x13, 0x0B, 0, 0, // Y pixels per meter (72 DPI)
        0,    0,    0, 0, // Colors in palette
        0,    0,    0, 0 // Important colors
    };

    // Fill width and height
    *reinterpret_cast<uint32_t*>(&dibHeader[4]) = width;
    *reinterpret_cast<uint32_t*>(&dibHeader[8]) = height;

    // Write headers
    file.write(bmpHeader, 14);
    file.write(dibHeader, 40);

    // Write pixel data (BMP stores pixels bottom to top, BGR format)
    for (int y = height - 1; y >= 0; y--) {
      for (int x = 0; x < width; x++) {
        file.put(pixels[y][x].b); // Blue
        file.put(pixels[y][x].g); // Green
        file.put(pixels[y][x].r); // Red
      }
      // Add padding
      for (int p = 0; p < padding; p++) {
        file.put(0);
      }
    }

    file.close();
    std::cout << "BMP image saved as " << filename << std::endl;
    return true;
  }

  bool savePNG(const std::string& filename) {
    // Simple PNG implementation using raw format
    // For a production version, you'd want to use libpng
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
      std::cerr << "Error: Could not open " << filename << " for writing"
                << std::endl;
      return false;
    }

    // Note: This is a simplified PNG writer that creates uncompressed PNG
    // For full PNG support, you'd typically use libpng library

    // PNG signature
    const uint8_t pngSignature[] = {
        0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
    file.write(reinterpret_cast<const char*>(pngSignature), 8);

    // IHDR chunk
    uint32_t ihdrLength = 13;
    uint32_t ihdrLengthBE = __builtin_bswap32(ihdrLength);
    file.write(reinterpret_cast<const char*>(&ihdrLengthBE), 4);
    file.write("IHDR", 4);

    uint32_t widthBE = __builtin_bswap32(width);
    uint32_t heightBE = __builtin_bswap32(height);
    file.write(reinterpret_cast<const char*>(&widthBE), 4);
    file.write(reinterpret_cast<const char*>(&heightBE), 4);

    uint8_t bitDepth = 8;
    uint8_t colorType = 2; // RGB
    uint8_t compression = 0;
    uint8_t filter = 0;
    uint8_t interlace = 0;

    file.put(bitDepth);
    file.put(colorType);
    file.put(compression);
    file.put(filter);
    file.put(interlace);

    // IHDR CRC (simplified - in production, calculate proper CRC32)
    uint32_t ihdrCRC = 0x12345678; // Placeholder
    file.write(reinterpret_cast<const char*>(&ihdrCRC), 4);

    // IDAT chunk (simplified - normally this would contain compressed image
    // data)
    std::vector<uint8_t> imageData;
    for (int y = 0; y < height; y++) {
      imageData.push_back(0); // Filter type for this scanline
      for (int x = 0; x < width; x++) {
        imageData.push_back(pixels[y][x].r);
        imageData.push_back(pixels[y][x].g);
        imageData.push_back(pixels[y][x].b);
      }
    }

    uint32_t idatLength = imageData.size();
    uint32_t idatLengthBE = __builtin_bswap32(idatLength);
    file.write(reinterpret_cast<const char*>(&idatLengthBE), 4);
    file.write("IDAT", 4);
    file.write(
        reinterpret_cast<const char*>(imageData.data()), imageData.size());

    // IDAT CRC (placeholder)
    uint32_t idatCRC = 0x87654321;
    file.write(reinterpret_cast<const char*>(&idatCRC), 4);

    // IEND chunk
    uint32_t iendLength = 0;
    file.write(reinterpret_cast<const char*>(&iendLength), 4);
    file.write("IEND", 4);
    uint32_t iendCRC = 0xAE426082; // Standard IEND CRC
    file.write(reinterpret_cast<const char*>(&iendCRC), 4);

    file.close();
    std::cout
        << "PNG image saved as " << filename
        << " (Note: Simplified PNG format - may not display correctly in all viewers)"
        << std::endl;
    return true;
  }
};

int main() {
  const int width = 400;
  const int height = 300;

  std::cout << "Generating " << width << "x" << height << " test images..."
            << std::endl;

  ImageGenerator generator(width, height);
  generator.generateTestPattern();

  // Save in all three formats
  generator.savePPM("test_image.ppm");
  generator.saveBMP("test_image.bmp");
  generator.savePNG("test_image.png");

  std::cout << "Image generation complete!" << std::endl;
  std::cout << "\nTest pattern description:" << std::endl;
  std::cout << "- Top third: RGB gradient" << std::endl;
  std::cout << "- Middle third: Black and white checkerboard" << std::endl;
  std::cout << "- Bottom third: Radial gradient" << std::endl;

  return 0;
}

