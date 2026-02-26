#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>

// Helper to calculate row padding in a BMP file
static int get_padding(int width) {
    return (4 - (width * sizeof(Pixel)) % 4) % 4;
}

BMPImage* read_bmp(const char* filename) {
    FILE* file = fopen(filename, "rb"); // Open in binary mode
    if (!file) {
        printf("Error: Could not open file %s for reading.\n", filename);
        return NULL;
    }

    // Allocate memory for the image structure
    BMPImage* image = (BMPImage*)malloc(sizeof(BMPImage));
    if (!image) {
        printf("Error: Memory allocation failed.\n");
        fclose(file);
        return NULL;
    }

    // Read headers
    fread(&image->file_header, sizeof(BMPFileHeader), 1, file);
    fread(&image->info_header, sizeof(BMPInfoHeader), 1, file);

    // Verify it's a valid 24-bit uncompressed BMP file
    if (image->file_header.file_type != 0x4D42 || image->info_header.bit_count != 24 || image->info_header.compression != 0) {
        printf("Error: Unsupported image format. Must be an uncompressed 24-bit BMP.\n");
        free(image);
        fclose(file);
        return NULL;
    }

    int width = image->info_header.width;
    int height = image->info_header.height;
    
    // Allocate contiguous memory for the raw pixels
    int total_pixels = width * abs(height);
    image->data = (Pixel*)malloc(total_pixels * sizeof(Pixel));

    if (!image->data) {
        printf("Error: Pixel memory allocation failed.\n");
        free(image);
        fclose(file);
        return NULL;
    }

    // Jump to the pixel data offset (just in case there is a color palette or extra data)
    fseek(file, image->file_header.offset_data, SEEK_SET);

    // Read pixel data row by row due to BMP padding alignment rules
    int padding = get_padding(width);
    for (int y = 0; y < abs(height); y++) {
        // Read the exact pixels of the row
        fread(image->data + (y * width), sizeof(Pixel), width, file);
        // Skip the padding at the end of the row
        fseek(file, padding, SEEK_CUR);
    }

    fclose(file);
    return image;
}

void write_bmp(const char* filename, BMPImage* image) {
    if (!image || !image->data) return;

    FILE* file = fopen(filename, "wb"); // Open in binary write mode
    if (!file) {
        printf("Error: Could not open file %s for writing.\n", filename);
        return;
    }

    // Write Headers
    fwrite(&image->file_header, sizeof(BMPFileHeader), 1, file);
    fwrite(&image->info_header, sizeof(BMPInfoHeader), 1, file);

    // Create correct padding
    int padding = get_padding(image->info_header.width);
    uint8_t pad_bytes[3] = {0, 0, 0};

    // Write pixels row by row
    for (int y = 0; y < abs(image->info_header.height); y++) {
        // Write the exact pixels
        fwrite(image->data + (y * image->info_header.width), sizeof(Pixel), image->info_header.width, file);
        // Write the padding
        fwrite(pad_bytes, 1, padding, file);
    }

    fclose(file);
}

void free_bmp(BMPImage* image) {
    if (image) {
        if (image->data) {
            free(image->data); // Free the pixel array first
        }
        free(image); // Free the holding structure
    }
}
