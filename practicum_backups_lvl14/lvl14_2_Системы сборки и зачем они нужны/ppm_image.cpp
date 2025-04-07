#include "ppm_image.h"

#include <array>
#include <fstream>
#include <string_view>

using namespace std;

namespace img_lib {

bool SavePPM(const Path& file, const Image& image) {
    ofstream ofs(file, ios::binary);
    if (!ofs) {
        return false;
    }

    // Write header
    ofs << PPM_SIG << '\n'
        << image.GetWidth() << ' ' << image.GetHeight() << '\n'
        << PPM_MAX << '\n';

    // Prepare buffer for one line of RGB data
    vector<char> buffer(image.GetWidth() * 3);

    for (int y = 0; y < image.GetHeight(); ++y) {
        const Color* line = image.GetLine(y);
        
        // Convert each pixel to RGB (ignore alpha)
        for (int x = 0; x < image.GetWidth(); ++x) {
            buffer[x * 3 + 0] = static_cast<char>(line[x].r);
            buffer[x * 3 + 1] = static_cast<char>(line[x].g);
            buffer[x * 3 + 2] = static_cast<char>(line[x].b);
        }

        // Write the whole line at once
        if (!ofs.write(buffer.data(), buffer.size())) {
            return false;
        }
    }

    return ofs.good();
}

Image LoadPPM(const Path& file) {
    ifstream ifs(file, ios::binary);
    string sign;
    int w, h, color_max;

    ifs >> sign >> w >> h >> color_max;

    if (sign != PPM_SIG || color_max != PPM_MAX) {
        return {};
    }

    const char next = ifs.get();
    if (next != '\n') {
        return {};
    }

    Image result(w, h, Color::Black());
    vector<char> buff(w * 3);

    for (int y = 0; y < h; ++y) {
        Color* line = result.GetLine(y);
        ifs.read(buff.data(), w * 3);

        for (int x = 0; x < w; ++x) {
            line[x].r = static_cast<byte>(buff[x * 3 + 0]);
            line[x].g = static_cast<byte>(buff[x * 3 + 1]);
            line[x].b = static_cast<byte>(buff[x * 3 + 2]);
        }
    }

    return result;
}

}  // namespace img_lib