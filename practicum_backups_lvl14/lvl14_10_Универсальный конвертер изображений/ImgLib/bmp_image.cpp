#include "bmp_image.h"
#include "pack_defines.h"

#include <array>
#include <fstream>
#include <string_view>

using namespace std;

namespace img_lib {

PACKED_STRUCT_BEGIN BmpFileHead {
    uint8_t magic_bytes[2] = {'B', 'M'};
    uint32_t total_size;
    uint32_t unused_value = 0;
    uint32_t pixel_array_start;
}
PACKED_STRUCT_END

PACKED_STRUCT_BEGIN BmpInfoPart {
    uint32_t struct_size = sizeof(BmpInfoPart);
    int32_t pixels_wide;
    int32_t pixels_high;
    uint16_t color_planes = 1;
    uint16_t bits_per_dot = 24;
    uint32_t compression_mode = 0;
    uint32_t raw_data_size;
    int32_t horizontal_res = 11811;
    int32_t vertical_res = 11811;
    uint32_t palette_colors = 0;
    uint32_t important_colors = 0x1000000;
}
PACKED_STRUCT_END

static int CalculateRowPadding(int w) {
    return 4 * ((w * 3 + 3) / 4);
}

bool SaveBMP(const Path& file, const Image& image) {
    ofstream output_stream(file, ios::binary);
    if (!output_stream) {
        return false;
    }

    const int img_w = image.GetWidth();
    const int img_h = image.GetHeight();
    const int row_pad = CalculateRowPadding(img_w);

    BmpFileHead file_head;
    file_head.pixel_array_start = sizeof(BmpFileHead) + sizeof(BmpInfoPart);
    file_head.total_size = file_head.pixel_array_start + row_pad * img_h;

    BmpInfoPart info_part;
    info_part.pixels_wide = img_w;
    info_part.pixels_high = img_h;
    info_part.raw_data_size = row_pad * img_h;

    output_stream.write(reinterpret_cast<const char*>(&file_head), sizeof(file_head));
    output_stream.write(reinterpret_cast<const char*>(&info_part), sizeof(info_part));

    vector<char> temp_buffer(row_pad);
    for (int row_idx = img_h - 1; row_idx >= 0; --row_idx) {
        const Color* image_row = image.GetLine(row_idx);
        for (int col_idx = 0; col_idx < img_w; ++col_idx) {
            temp_buffer[col_idx * 3 + 0] = static_cast<char>(image_row[col_idx].b);
            temp_buffer[col_idx * 3 + 1] = static_cast<char>(image_row[col_idx].g);
            temp_buffer[col_idx * 3 + 2] = static_cast<char>(image_row[col_idx].r);
        }
        output_stream.write(temp_buffer.data(), row_pad);
    }

    return output_stream.good();
}

Image LoadBMP(const Path& file) {
    ifstream input_stream(file, ios::binary);
    if (!input_stream) {
        return {};
    }

    BmpFileHead file_head;
    input_stream.read(reinterpret_cast<char*>(&file_head), sizeof(file_head));
    
    if (file_head.magic_bytes[0] != 'B' || file_head.magic_bytes[1] != 'M') {
        return {};
    }

    BmpInfoPart info_part;
    input_stream.read(reinterpret_cast<char*>(&info_part), sizeof(info_part));

    const int img_width = info_part.pixels_wide;
    const int img_height = info_part.pixels_high;
    const int row_padding = CalculateRowPadding(img_width);

    Image result_image(img_width, img_height, Color::Black());
    vector<char> temp_buffer(row_padding);

    for (int row_idx = img_height - 1; row_idx >= 0; --row_idx) {
        input_stream.read(temp_buffer.data(), row_padding);
        Color* result_row = result_image.GetLine(row_idx);
        for (int col_idx = 0; col_idx < img_width; ++col_idx) {
            result_row[col_idx].r = static_cast<byte>(temp_buffer[col_idx * 3 + 2]);
            result_row[col_idx].g = static_cast<byte>(temp_buffer[col_idx * 3 + 1]);
            result_row[col_idx].b = static_cast<byte>(temp_buffer[col_idx * 3 + 0]);
        }
    }

    return result_image;
}

}  // namespace img_lib