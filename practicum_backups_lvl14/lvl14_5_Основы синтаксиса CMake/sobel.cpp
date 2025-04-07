#include "ppm_image.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <string_view>

using namespace std;

int Sum(img_lib::Color c) {
    return to_integer<int>(c.r) + to_integer<int>(c.g) + to_integer<int>(c.b);
}

img_lib::Image Sobel(const img_lib::Image& image) {
    const int width = image.GetWidth();
    const int height = image.GetHeight();
    img_lib::Image result(width, height, img_lib::Color::Black());

    if (width < 3 || height < 3) {
        return result;
    }

    for (int y = 1; y < height - 1; ++y) {
        const img_lib::Color* prev_line = image.GetLine(y - 1);
        const img_lib::Color* curr_line = image.GetLine(y);
        const img_lib::Color* next_line = image.GetLine(y + 1);
        img_lib::Color* result_line = result.GetLine(y);

        for (int x = 1; x < width - 1; ++x) {
            // Соседние пиксели
            int tl = Sum(prev_line[x - 1]);
            int tc = Sum(prev_line[x]);
            int tr = Sum(prev_line[x + 1]);
            int cl = Sum(curr_line[x - 1]);
            int cr = Sum(curr_line[x + 1]);
            int bl = Sum(next_line[x - 1]);
            int bc = Sum(next_line[x]);
            int br = Sum(next_line[x + 1]);

            // Вычисление оператора Собеля
            double gx = -tl - 2*tc - tr + bl + 2*bc + br;
            double gy = -tl - 2*cl - bl + tr + 2*cr + br;
            double value = sqrt(gx*gx + gy*gy);
            byte b = static_cast<byte>(clamp(value, 0.0, 255.0));

            result_line[x] = {b, b, b, byte{255}};
        }
    }

    return result;
}

int main(int argc, const char** argv) {
    if (argc != 3) {
        cerr << "Usage: "sv << argv[0] << " <input image> <output image>"sv << endl;
        return 1;
    }

    auto image = img_lib::LoadPPM(argv[1]);
    if (!image) {
        cerr << "Error loading image"sv << endl;
        return 2;
    }

    image = Sobel(image);

    if (!img_lib::SavePPM(argv[2], image)) {
        cerr << "Error saving image"sv << endl;
        return 3;
    }

    cout << "Image saved successfully!"sv << endl;
    return 0;
}