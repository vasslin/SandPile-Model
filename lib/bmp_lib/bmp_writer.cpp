#include "bmp_writer.h"

FileHeader::FileHeader(uint32_t width, uint32_t height)
    : file_type(0x4D42),
      file_size(BMPConstants::kFileHeaderSize + BMPConstants::kInformationHeaderSize + width * height),
      reserved(0),
      data_offset(BMPConstants::kFileHeaderSize + BMPConstants::kInformationHeaderSize) {}

InformationHeader::InformationHeader(uint32_t width, uint32_t height)
    : header_size(BMPConstants::kInformationHeaderSize),
      width(width),
      height(height),
      planes(BMPConstants::kCountOfPlanes),
      bits_per_pixel(BMPConstants::kBitsPerPixel),
      compression(BMPConstants::kCompressionValue),
      image_size(width * height),
      x_pixels_per_metr(BMPConstants::kXPixelsPerMetr),
      y_pixels_per_metr(BMPConstants::kYPixelsPerMetr),
      colour_count(BMPConstants::kColorCount),
      important_colours(BMPConstants::kImportantCount) {}

// write file header, information header and colour palette
void BMPWriter::writeHeadersPrivate(const FileHeader& file_header, const InformationHeader& inf_header) {
    out_.write((char*)&file_header.file_type, sizeof(file_header.file_type));
    out_.write((char*)&file_header.file_size, sizeof(file_header.file_size));
    out_.write((char*)&file_header.reserved, sizeof(file_header.reserved));
    out_.write((char*)&file_header.data_offset, sizeof(file_header.data_offset));

    out_.write((char*)&inf_header.header_size, sizeof(inf_header.header_size));
    out_.write((char*)&inf_header.width, sizeof(inf_header.width));
    out_.write((char*)&inf_header.height, sizeof(inf_header.height));
    out_.write((char*)&inf_header.planes, sizeof(inf_header.planes));
    out_.write((char*)&inf_header.bits_per_pixel, sizeof(inf_header.bits_per_pixel));
    out_.write((char*)&inf_header.compression, sizeof(inf_header.compression));
    out_.write((char*)&inf_header.image_size, sizeof(inf_header.image_size));
    out_.write((char*)&inf_header.x_pixels_per_metr, sizeof(inf_header.x_pixels_per_metr));
    out_.write((char*)&inf_header.y_pixels_per_metr, sizeof(inf_header.y_pixels_per_metr));
    out_.write((char*)&inf_header.colour_count, sizeof(inf_header.colour_count));
    out_.write((char*)&inf_header.important_colours, sizeof(inf_header.important_colours));

    Color palette[] = {{255, 255, 255, 0}, {0, 255, 0, 0}, {128, 0, 128, 0}, {0, 255, 255, 0}, {0, 0, 0, 0}};

    for (int i = 0; i < 5; i++) {
        out_.write((char*)&palette[i], sizeof(Color));
    }
}

// void WritePixels(std::ofstream& out_, char* data, size_t sz) { out_.write(data, sz); }

BMPWriter::BMPWriter(std::ofstream& out, uint32_t width, uint32_t height) : out_(out), size_({width, height}) {
}

void BMPWriter::writeHeaders() {
    writeHeadersPrivate(FileHeader(size_.first, size_.second), InformationHeader(size_.first, size_.second));
}

void BMPWriter::writePixels(char* data, size_t sz) { out_.write(data, sz); }