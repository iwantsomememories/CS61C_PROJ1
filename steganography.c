/************************************************************************
**
** NAME:        steganography.c
**
** DESCRIPTION: CS61C Fall 2020 Project 1
**
** AUTHOR:      Dan Garcia  -  University of California at Berkeley
**              Copyright (C) Dan Garcia, 2020. All rights reserved.
**				Justin Yokota - Starter Code
**				YOUR NAME HERE
**
** DATE:        2020-08-23
**
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "imageloader.h"

//Determines what color the cell at the given row/col should be. This should not affect Image, and should allocate space for a new Color.
Color *evaluateOnePixel(Image *image, int row, int col)
{
	if (!image || !image->image || image->rows <= row || image->cols <= col) {
		fprintf(stderr, "failed to get pixel\n");
		return NULL;
	}

	Color *color = (Color *)malloc(sizeof(Color));
	if (!color) {
		fprintf(stderr, "malloc failed.\n");
		return NULL;
	}

	if ((image->image[row][col].B & 1) != 0) {
		color->R = 255;
		color->G = 255;
		color->B = 255;
	} else {
		color->R = 0;
		color->G = 0;
		color->B = 0;
	}

	return color;
}

//Given an image, creates a new image extracting the LSB of the B channel.
Image *steganography(Image *image)
{
	if (!image || !image->image) {
		return NULL;
	}

	Image *new_image  = (Image *)malloc(sizeof(Image));
	if (!new_image) {
		fprintf(stderr, "malloc failed.\n");
		return NULL;
	}

	new_image->rows = image->rows;
	new_image->cols = image->cols;
	new_image->image = (Color **)calloc(image->rows, sizeof(Color*));
	if (!new_image->image) {
		fprintf(stderr, "calloc failed.\n");
		free(new_image);
		return NULL;
	}

	for (uint32_t i = 0; i < image->rows; i++) {
		new_image->image[i] = (Color *)malloc(sizeof(Color) * image->cols);
		if (!new_image->image[i]) {
			fprintf(stderr, "malloc failed.\n");
			freeImage(new_image);
			return NULL;
		}

		for (uint32_t j = 0; j < image->cols; j++) {
			Color *new_color = evaluateOnePixel(image, i, j);
			if (!new_color) {
				fprintf(stderr, "evaluateOnePixel failed.\n");
				freeImage(new_image);
				return NULL;
			}
			new_image->image[i][j].R = new_color->R;
			new_image->image[i][j].G = new_color->G;
			new_image->image[i][j].B = new_color->B;
			free(new_color);
		}
	}

	return new_image;
}

/*
Loads a file of ppm P3 format from a file, and prints to stdout (e.g. with printf) a new image, 
where each pixel is black if the LSB of the B channel is 0, 
and white if the LSB of the B channel is 1.

argc stores the number of arguments.
argv stores a list of arguments. Here is the expected input:
argv[0] will store the name of the program (this happens automatically).
argv[1] should contain a filename, containing a file of ppm P3 format (not necessarily with .ppm file extension).
If the input is not correct, a malloc fails, or any other error occurs, you should exit with code -1.
Otherwise, you should return from main with code 0.
Make sure to free all memory before returning!
*/
int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage: steganography [file]\n");
		return -1;
	}

	Image *image = readData(argv[1]);
	if (!image) {
		fprintf(stderr, "load image failed.\n");
		return -1;
	}

	Image *new_image = steganography(image);
	freeImage(image);
	if (!new_image) {
		fprintf(stderr, "steganography failed.\n");
		freeImage(image);
		return -1;
	}

	writeData(new_image);
	freeImage(new_image);
	freeImage(image);
	return 0;
}
