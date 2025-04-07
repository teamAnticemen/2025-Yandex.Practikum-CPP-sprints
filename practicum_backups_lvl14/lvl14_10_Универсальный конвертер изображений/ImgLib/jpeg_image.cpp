#include "jpeg_image.h"
#include "img_lib.h"

#include <cstdio>
#include <cstddef>
#include <vector>
#include <setjmp.h>
#include <jpeglib.h>

using namespace std;

namespace img_lib {

struct JpegErrorHandler {
    struct jpeg_error_mgr base;
    jmp_buf jump_buffer;
};

typedef JpegErrorHandler* JpegErrorPtr;

METHODDEF(void) HandleJpegError(j_common_ptr cinfo) {
    JpegErrorPtr myerr = (JpegErrorPtr)cinfo->err;
    (*cinfo->err->output_message)(cinfo);
    longjmp(myerr->jump_buffer, 1);
}

bool SaveJPEG(const Path& file, const Image& image) {
    struct jpeg_compress_struct cinfo;
    JpegErrorHandler jerr;
    
    FILE* output_file = nullptr;
    JSAMPROW row_ptr[1];
    int row_width;

    cinfo.err = jpeg_std_error(&jerr.base);
    jerr.base.error_exit = HandleJpegError;

    if (setjmp(jerr.jump_buffer)) {
        if (output_file) fclose(output_file);
        jpeg_destroy_compress(&cinfo);
        return false;
    }

    jpeg_create_compress(&cinfo);

#ifdef _MSC_VER
    if ((output_file = _wfopen(file.wstring().c_str(), L"wb")) == nullptr) {
#else
    if ((output_file = fopen(file.string().c_str(), "wb")) == nullptr) {
#endif
        jpeg_destroy_compress(&cinfo);
        return false;
    }

    jpeg_stdio_dest(&cinfo, output_file);

    cinfo.image_width = image.GetWidth();
    cinfo.image_height = image.GetHeight();
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_start_compress(&cinfo, TRUE);

    row_width = image.GetWidth() * 3;
    vector<JSAMPLE> pixel_row(row_width);

    while (cinfo.next_scanline < cinfo.image_height) {
        const Color* img_line = image.GetLine(cinfo.next_scanline);
        for (int i = 0; i < image.GetWidth(); ++i) {
            pixel_row[i * 3 + 0] = static_cast<JSAMPLE>(img_line[i].r);
            pixel_row[i * 3 + 1] = static_cast<JSAMPLE>(img_line[i].g);
            pixel_row[i * 3 + 2] = static_cast<JSAMPLE>(img_line[i].b);
        }
        row_ptr[0] = pixel_row.data();
        jpeg_write_scanlines(&cinfo, row_ptr, 1);
    }

    jpeg_finish_compress(&cinfo);
    fclose(output_file);
    jpeg_destroy_compress(&cinfo);

    return true;
}

Image LoadJPEG(const Path& file) {
    struct jpeg_decompress_struct cinfo;
    JpegErrorHandler jerr;
    
    FILE* input_file = nullptr;
    JSAMPARRAY buffer;
    int row_width;

#ifdef _MSC_VER
    if ((input_file = _wfopen(file.wstring().c_str(), L"rb")) == nullptr) {
#else
    if ((input_file = fopen(file.string().c_str(), "rb")) == nullptr) {
#endif
        return {};
    }

    cinfo.err = jpeg_std_error(&jerr.base);
    jerr.base.error_exit = HandleJpegError;

    if (setjmp(jerr.jump_buffer)) {
        jpeg_destroy_decompress(&cinfo);
        if (input_file) fclose(input_file);
        return {};
    }

    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, input_file);
    jpeg_read_header(&cinfo, TRUE);

    cinfo.out_color_space = JCS_RGB;
    cinfo.output_components = 3;

    jpeg_start_decompress(&cinfo);
    
    row_width = cinfo.output_width * cinfo.output_components;
    buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, row_width, 1);

    Image result(cinfo.output_width, cinfo.output_height, Color::Black());

    while (cinfo.output_scanline < cinfo.output_height) {
        int current_line = cinfo.output_scanline;
        jpeg_read_scanlines(&cinfo, buffer, 1);

        Color* result_line = result.GetLine(current_line);
        for (int i = 0; i < result.GetWidth(); ++i) {
            result_line[i] = Color{
                byte{buffer[0][i * 3 + 0]}, 
                byte{buffer[0][i * 3 + 1]}, 
                byte{buffer[0][i * 3 + 2]}, 
                byte{255}
            };
        }
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(input_file);

    return result;
}

} // namespace img_lib