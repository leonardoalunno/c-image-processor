#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bmp.h"
#include "filters.h"

void print_usage() {
    printf("Usage: ./bin/image_processor [filter] [input.bmp] [output.bmp]\n");
    printf("\nAvailable filters:\n");
    printf("  -g    Grayscale filter\n");
    printf("  -s    Sepia filter\n");
    printf("  -e    Edge Detection (Sobel filter)\n");
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Error: Incorrect number of arguments.\n");
        print_usage();
        return 1;
    }

    char* filter = argv[1];
    char* input_file = argv[2];
    char* output_file = argv[3];

    printf("Loading image from %s...\n", input_file);
    BMPImage* img = read_bmp(input_file);
    if (!img) {
        return 1; // Load failed
    }

    printf("Image loaded successfully. Size: %dx%d px.\n", img->info_header.width, abs(img->info_header.height));

    if (strcmp(filter, "-g") == 0) {
        printf("Applying Grayscale filter...\n");
        apply_grayscale(img);
    } 
    else if (strcmp(filter, "-s") == 0) {
        printf("Applying vintage Sepia filter...\n");
        apply_sepia(img);
    } 
    else if (strcmp(filter, "-e") == 0) {
        printf("Applying Sobel Edge Detection...\n");
        apply_edge_detection(img);
    } 
    else {
        printf("Error: Unknown filter '%s'.\n", filter);
        print_usage();
        free_bmp(img); // Avoid leak before exit
        return 1;
    }

    printf("Saving processed image to %s...\n", output_file);
    write_bmp(output_file, img);

    // Free all dynamically allocated memory!
    free_bmp(img);

    printf("Success! Exiting cleanly without memory leaks.\n");
    return 0;
}
