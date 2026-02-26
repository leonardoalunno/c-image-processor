#ifndef FILTERS_H
#define FILTERS_H

#include "bmp.h"

// Applies a Grayscale filter based on human eye luminosity weighting
void apply_grayscale(BMPImage* image);

// Applies a vintage Sepia tone transformation matrix
void apply_sepia(BMPImage* image);

// (Optional Challenge) Applies a simple kernel convolution for Edge Detection
void apply_edge_detection(BMPImage* image);

#endif
