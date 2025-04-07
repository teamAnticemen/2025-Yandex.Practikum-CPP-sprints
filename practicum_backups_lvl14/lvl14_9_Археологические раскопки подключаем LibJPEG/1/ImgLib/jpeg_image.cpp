#include "jpeg_image.h"
#include "img_lib.h"

#include <cstdio>
#include <cstddef>
#include <vector>
#include <setjmp.h>
#include <jpeglib.h>

using namespace std;

namespace img_lib {

struct my_error_mgr {
    struct jpeg_error_mgr pub;
    jmp_buf setjmp_buffer;
};

typedef struct my_error_mgr* my_error_ptr;

METHODDEF(void)
my_error_exit(j_common_ptr cinfo) {
    my_error_ptr myerr = (my_error_ptr)cinfo->err;
    (*cinfo->err->output_message)(cinfo);
    longjmp(myerr->setjmp_buffer, 1);
}

bool SaveJPEG(const Path& file, const Image& image) {
    jpeg_compress_struct cinfo;
    my_error_mgr jerr;
    
    FILE* outfile = nullptr;
    JSAMPROW row_pointer[1];
    int row_stride;

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;

    if (setjmp(jerr.setjmp_buffer)) {
        if (outfile) fclose(outfile);
        jpeg_destroy_compress(&cinfo);
        return false;
    }

    jpeg_create_compress(&cinfo);

#ifdef _MSC_VER
    if ((outfile = _wfopen(file.wstring().c_str(), L"wb")) == nullptr) {
#else
    if ((outfile = fopen(file.string().c_str(), "wb")) == nullptr) {
#endif
        jpeg_destroy_compress(&cinfo);
        return false;
    }

    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = image.GetWidth();
    cinfo.image_height = image.GetHeight();
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_start_compress(&cinfo, TRUE);

    row_stride = image.GetWidth() * 3;
    std::vector<JSAMPLE> buffer(row_stride);

    while (cinfo.next_scanline < cinfo.image_height) {
        const Color* line = image.GetLine(cinfo.next_scanline);
        
        for (int x = 0; x < image.GetWidth(); ++x) {
            buffer[x * 3 + 0] = static_cast<JSAMPLE>(line[x].r);
            buffer[x * 3 + 1] = static_cast<JSAMPLE>(line[x].g);
            buffer[x * 3 + 2] = static_cast<JSAMPLE>(line[x].b);
        }
        
        row_pointer[0] = buffer.data();
        (void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    fclose(outfile);
    jpeg_destroy_compress(&cinfo);

    return true;
}

void SaveScanlineToImage(const JSAMPLE* row, int y, Image& out_image) {
    Color* line = out_image.GetLine(y);
    for (int x = 0; x < out_image.GetWidth(); ++x) {
        const JSAMPLE* pixel = row + x * 3;
        line[x] = Color{byte{pixel[0]}, byte{pixel[1]}, byte{pixel[2]}, byte{255}};
    }
}

Image LoadJPEG(const Path& file) {
    jpeg_decompress_struct cinfo;
    my_error_mgr jerr;
    
    FILE* infile = nullptr;
    JSAMPARRAY buffer;
    int row_stride;

#ifdef _MSC_VER
    if ((infile = _wfopen(file.wstring().c_str(), L"rb")) == nullptr) {
#else
    if ((infile = fopen(file.string().c_str(), "rb")) == nullptr) {
#endif
        return {};
    }

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;

    if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_decompress(&cinfo);
        if (infile) fclose(infile);
        return {};
    }

    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    (void)jpeg_read_header(&cinfo, TRUE);

    cinfo.out_color_space = JCS_RGB;
    cinfo.output_components = 3;

    (void)jpeg_start_decompress(&cinfo);
    
    row_stride = cinfo.output_width * cinfo.output_components;
    buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);

    Image result(cinfo.output_width, cinfo.output_height, Color::Black());

    while (cinfo.output_scanline < cinfo.output_height) {
        int y = cinfo.output_scanline;
        (void)jpeg_read_scanlines(&cinfo, buffer, 1);
        SaveScanlineToImage(buffer[0], y, result);
    }

    (void)jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);

    return result;
}

} // namespace img_lib