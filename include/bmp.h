#ifndef BMP_H
#define BMP_H

#include <stdint.h>

#pragma pack(push, 1) // Ensure structures are memory-aligned to exactly 1 byte

// BMP File Header (14 Bytes)
typedef struct {
    uint16_t file_type;     // File type always 'BM' (0x4D42)
    uint32_t file_size;     // Size of the file (in bytes)
    uint16_t reserved1;     // Always 0
    uint16_t reserved2;     // Always 0
    uint32_t offset_data;   // Start position of pixel data (bytes from the beginning of the file)
} BMPFileHeader;

// BMP Information Header (40 Bytes)
typedef struct {
    uint32_t size;          // Size of this header (in bytes)
    int32_t width;          // width of bitmap in pixels
    int32_t height;         // height of bitmap in pixels
    uint16_t planes;        // No. of planes for the target device, this is always 1
    uint16_t bit_count;     // No. of bits per pixel
    uint32_t compression;   // 0 or 3 - uncompressed.
    uint32_t size_image;    // 0 - for uncompressed images
    int32_t x_pixels_per_meter;
    int32_t y_pixels_per_meter;
    uint32_t colors_used;   // No. color indexes in the color table. Use 0 for the max number of colors allowed by bit_count
    uint32_t colors_important; // No. of colors used for displaying the bitmap. If 0 all colors are required
} BMPInfoHeader;

#pragma pack(pop)

// Represents a 24-bit Pixel (BGR format in memory)
typedef struct {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
} Pixel;

// Structure to hold the entire loaded Image
typedef struct {
    BMPFileHeader file_header;
    BMPInfoHeader info_header;
    Pixel* data; // Dynamically allocated array of pixels
} BMPImage;

// Function Prototypes
BMPImage* read_bmp(const char* filename);
void write_bmp(const char* filename, BMPImage* image);
void free_bmp(BMPImage* image);

#endif
