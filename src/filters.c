#include "filters.h"
#include <math.h>
#include <stdlib.h>

// Helper to constrain values between 0 and 255
static uint8_t cap_color(int value) {
    if (value > 255) return 255;
    if (value < 0) return 0;
    return (uint8_t)value;
}

void apply_grayscale(BMPImage* image) {
    if (!image || !image->data) return;

    int total_pixels = image->info_header.width * abs(image->info_header.height);

    for (int i = 0; i < total_pixels; i++) {
        // Weighted average based on human visual perception
        int gray = (image->data[i].red * 0.299) + 
                   (image->data[i].green * 0.587) + 
                   (image->data[i].blue * 0.114);

        // In Gray, all channels are identical
        uint8_t capped_gray = cap_color(gray);
        image->data[i].red = capped_gray;
        image->data[i].green = capped_gray;
        image->data[i].blue = capped_gray;
    }
}

void apply_sepia(BMPImage* image) {
    if (!image || !image->data) return;

    int total_pixels = image->info_header.width * abs(image->info_header.height);

    for (int i = 0; i < total_pixels; i++) {
        uint8_t orig_r = image->data[i].red;
        uint8_t orig_g = image->data[i].green;
        uint8_t orig_b = image->data[i].blue;

        // Vintage sepia transform matrix
        int sepia_r = (orig_r * 0.393) + (orig_g * 0.769) + (orig_b * 0.189);
        int sepia_g = (orig_r * 0.349) + (orig_g * 0.686) + (orig_b * 0.168);
        int sepia_b = (orig_r * 0.272) + (orig_g * 0.534) + (orig_b * 0.131);

        image->data[i].red = cap_color(sepia_r);
        image->data[i].green = cap_color(sepia_g);
        image->data[i].blue = cap_color(sepia_b);
    }
}

void apply_edge_detection(BMPImage* image) {
    if (!image || !image->data) return;

    int width = image->info_header.width;
    int height = abs(image->info_header.height);

    // Create a temporary buffer so we don't calculate new edges based on already modified pixels
    Pixel* temp = (Pixel*)malloc(width * height * sizeof(Pixel));
    if (!temp) return; // Silent fail if out of memory

    // Sobel Operator Kernels
    int gx[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };

    int gy[3][3] = {
        {-1, -2, -1},
        { 0,  0,  0},
        { 1,  2,  1}
    };

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int sum_x_r = 0, sum_y_r = 0;
            int sum_x_g = 0, sum_y_g = 0;
            int sum_x_b = 0, sum_y_b = 0;

            // Apply 3x3 convolution
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    int neighbor_y = y + ky;
                    int neighbor_x = x + kx;
                    int idx = neighbor_y * width + neighbor_x;

                    int wx = gx[ky + 1][kx + 1];
                    int wy = gy[ky + 1][kx + 1];

                    sum_x_r += image->data[idx].red * wx;
                    sum_y_r += image->data[idx].red * wy;

                    sum_x_g += image->data[idx].green * wx;
                    sum_y_g += image->data[idx].green * wy;

                    sum_x_b += image->data[idx].blue * wx;
                    sum_y_b += image->data[idx].blue * wy;
                }
            }

            // Combine gradients
            int mag_r = sqrt(sum_x_r * sum_x_r + sum_y_r * sum_y_r);
            int mag_g = sqrt(sum_x_g * sum_x_g + sum_y_g * sum_y_g);
            int mag_b = sqrt(sum_x_b * sum_x_b + sum_y_b * sum_y_b);

            int out_idx = y * width + x;
            temp[out_idx].red = cap_color(mag_r);
            temp[out_idx].green = cap_color(mag_g);
            temp[out_idx].blue = cap_color(mag_b);
        }
    }

    // Overwrite original pixels (skipping the border 1px ring which we ignored)
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int idx = y * width + x;
            image->data[idx] = temp[idx];
        }
    }

    free(temp);
}
