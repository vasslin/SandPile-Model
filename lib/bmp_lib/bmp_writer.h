#include <cstdint>
#include <fstream>
#include <iostream>

// File Header (14 bites)

namespace BMPConstants {
// file header constants
const uint32_t kFileHeaderSize = 14;
const uint16_t kFileType = 0x4D42;
const uint32_t kReserved = 0;

// information header constants
const uint32_t kInformationHeaderSize = 40;
const uint16_t kCountOfPlanes = 1;
const uint32_t kBitsPerPixel = 4;
const uint32_t kCompressionValue = 0;
const uint32_t kXPixelsPerMetr = 3780;
const uint32_t kYPixelsPerMetr = 3780;
const uint32_t kColorCount = 5;
const uint32_t kImportantCount = 5;
}  // namespace BMPConstants

#pragma pack(push, 1)
struct FileHeader {
    FileHeader(uint32_t width, uint32_t height);

    uint16_t file_type;  // 0x4D42
    uint32_t file_size;
    uint32_t reserved;
    uint32_t data_offset;
};

// Bitmap information header (40 bites)
struct InformationHeader {
    InformationHeader(uint32_t width, uint32_t height);

    uint32_t header_size = 40;  // 40
    uint32_t width;
    uint32_t height;
    uint16_t planes = 1;          // 1
    uint16_t bits_per_pixel = 4;  // 4
    uint32_t compression = 0;     // 0 - without compression
    uint32_t image_size;          // size of image without header size
    uint32_t x_pixels_per_metr;
    uint32_t y_pixels_per_metr;
    uint32_t colour_count;
    uint32_t important_colours;
};

#pragma pack(pop)

struct Color {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t reserved;
};

class BMPWriter {
   public:
    BMPWriter(std::ofstream& out, uint32_t width, uint32_t height);
    ~BMPWriter() {};
    void writePixels(char* data, size_t sz);
    void writeHeaders();

   private:
    void writeHeadersPrivate(const FileHeader& file_header, const InformationHeader& inf_header);

    std::pair<uint32_t, uint32_t> size_;

    std::ofstream& out_;
};
