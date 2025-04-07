#include "ppm_image.h"
#include <algorithm>
#include <iostream>
#include <string_view>

using namespace std;

void VMirrInplace(img_lib::Image& image) {
    const int height = image.GetHeight();
    
    for (int y = 0; y < height / 2; ++y) {
        img_lib::Color* line1 = image.GetLine(y);
        img_lib::Color* line2 = image.GetLine(height - 1 - y);
        swap_ranges(line1, line1 + image.GetWidth(), line2);
    }
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

    VMirrInplace(image);

    if (!img_lib::SavePPM(argv[2], image)) {
        cerr << "Error saving image"sv << endl;
        return 3;
    }

    cout << "Image saved successfully!"sv << endl;
    return 0;
}