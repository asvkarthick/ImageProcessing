#include <algorithm>
#include <iomanip>
#include <iostream>
#include <vector>

class Image {
public:
    std::vector<std::vector<float>> data_;
    int width_, height_;

    Image(int w, int h) : width_(w), height_(h) {
        data_.resize(height_, std::vector<float>(width_, 0.0f));
    }

    explicit Image(const std::vector<std::vector<float>> &input_data)
        : data_(input_data), width_(input_data.empty() ? 0 : input_data[0].size()),
          height_(input_data.size()) {}

    float &operator()(int row, int col) {
        return data_[row][col];
    }

    const float &operator()(int row, int col) const {
        return data_[row][col];
    }

    void print() const {
        for (int i = 0; i < height_; i++) {
            for (int j = 0; j < width_; j++) {
                std::cout << std::setw(8) << std::fixed << std::setprecision(2)
                          << data_[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }
};

Image apply3x3BoxFilter(const Image &input) {
    Image output(input.width_, input.height_);

    for (int row = 0; row < input.height_; row++) {
        for (int col = 0; col < input.width_; col++) {
            float sum = 0.0f;
            int count = 0;

            for (int dr = -1; dr <= 1; dr++) {
                for (int dc = -1; dc <= 1; dc++) {
                    int nr = row + dr;
                    int nc = col + dc;

                    // Check bounds
                    nr = std::max(0, std::min(nr, input.height_ - 1));
                    nc = std::max(0, std::min(nc, input.width_ - 1));

                    sum += input(nr, nc);
                    count++;
                }
            }
            // Average the values in the 3x3 neighborhood (box filter)
            output(row, col) = sum / count;
        }
    }

    return output;
}

Image apply3x3BoxFilterPadded(const Image &input) {
    Image output(input.width_, input.height_);

    for (int row = 0; row < input.height_; row++) {
        for (int col = 0; col < input.width_; col++) {
            float sum = 0.0f;
            int count = 0;

            for (int dr = -1; dr <= 1; dr++) {
                for (int dc = -1; dc <= 1; dc++) {
                    int nr = row + dr;
                    int nc = col + dc;

                    // Check bounds
                    if (nr >= 0 && nr < input.height_ && nc >= 0 && nc < input.width_) {
                        sum += input(nr, nc);
                        count++;
                    }
                }
            }
            // Average the values in the 3x3 neighborhood (box filter)
            output(row, col) = count > 0 ? sum / count : 0.0f;
        }
    }

    return output;
}

Image apply3x3BoxFilterSimple(const Image &input) {
    Image output(input.width_, input.height_);

    for (int row = 1; row < input.height_ - 1; row++) {
        for (int col = 1; col < input.width_ - 1; col++) {
            float sum = 0.0f;

            for (int dr = -1; dr <= 1; dr++) {
                for (int dc = -1; dc <= 1; dc++) {
                    sum += input(row + dr, col + dc);
                }
            }
            // Average the values in the 3x3 neighborhood (box filter)
            output(row, col) = sum / 9.0f;
        }
    }

    // Copy border pixels
    for (int i = 0; i < input.height_; i++) {
        output(i, 0) = input(i, 0);
        output(i, input.width_ - 1) = input(i, input.width_ - 1);
    }
    for (int j = 0; j < input.width_; j++) {
        output(0, j) = input(0, j);
        output(input.height_ - 1, j) = input(input.height_ - 1, j);
    }

    return output;
}

int main() {
    // Create a test image with some pattern
    std::vector<std::vector<float>> test_data = {
        {1.0f, 2.0f, 3.0f, 4.0f, 5.0f},
        {6.0f, 7.0f, 8.0f, 9.0f, 10.0f},
        {11.0f, 12.0f, 13.0f, 14.0f, 15.0f},
        {16.0f, 17.0f, 18.0f, 19.0f, 20.0f},
        {21.0f, 22.0f, 23.0f, 24.0f, 25.0f}
    };
    Image original(test_data);

    std::cout << "Original image : " << std::endl;
    original.print();
    std::cout << std::endl;

    // Apply box filter with border replication
    Image filtered1 = apply3x3BoxFilter(original);
    std::cout << "Box filter (Border replication)" << std::endl;
    filtered1.print();
    std::cout << std::endl;


    // Apply box filter with zero padding
    Image filtered2 = apply3x3BoxFilterPadded(original);
    std::cout << "Box filter (Zero Padding)" << std::endl;
    filtered2.print();
    std::cout << std::endl;

    // Apply simple box filter
    Image filtered3 = apply3x3BoxFilterSimple(original);
    std::cout << "Box filter (Interior only)" << std::endl;
    filtered3.print();
    std::cout << std::endl;

    return 0;
}
