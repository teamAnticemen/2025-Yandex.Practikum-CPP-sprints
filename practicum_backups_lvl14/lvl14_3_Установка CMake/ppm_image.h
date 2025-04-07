#pragma once
#include "img_lib.h"

#include <filesystem>
#include <string_view>

namespace img_lib {
using Path = std::filesystem::path;

static constexpr std::string_view PPM_SIG{"P6"};
static constexpr int PPM_MAX = 255;

bool SavePPM(const Path& file, const Image& image);
Image LoadPPM(const Path& file);

}  // namespace img_lib