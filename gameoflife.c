/************************************************************************
**
** NAME:        gameoflife.c
**
** DESCRIPTION: CS61C Fall 2020 Project 1
**
** AUTHOR:      Justin Yokota - Starter Code
**				YOUR NAME HERE
**
**
** DATE:        2020-08-23
**
**************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "imageloader.h"

// Helpers for circular/toroidal indexing (wrap around edges)
#define INDEX_INC(x, l) ((x) + 1 == (l) ? 0 : (x) + 1)
#define INDEX_DEC(x, l) ((x) == 0 ? (l) - 1 : (x) - 1)

// Process one color channel for all 8 bits
static uint8_t evaluateChannel(uint8_t current, const Color neighbors[8], char channel, uint32_t rule)
{
	uint8_t result = 0;
	for (int i = 0; i < 8; i++) {
		int count = 0;
		for (int j = 0; j < 8; j++) {
			uint8_t val;
			if (channel == 'R')
				val = neighbors[j].R;
			else if (channel == 'G')
				val = neighbors[j].G;
			else
				val = neighbors[j].B;
			count += (val & (1 << i)) ? 1 : 0;
		}
		uint32_t alive = 1 << count;
		if (current & (1 << i)) {
			alive <<= 9;
		}
		if (alive & rule) {
			result |= (1 << i);
		}
	}
	return result;
}

//Determines what color the cell at the given row/col should be. This function allocates space for a new Color.
//Note that you will need to read the eight neighbors of the cell in question. The grid "wraps", so we treat the top row as adjacent to the bottom row
//and the left column as adjacent to the right column.
Color *evaluateOneCell(Image *image, int row, int col, uint32_t rule)
{
	if (!image) {
		return NULL;
	}

	if (!image->image) {
		return NULL;
	}

	uint32_t r = image->rows;
	uint32_t c = image->cols;

	if (row < 0 || row >= r || col < 0 || col >= c) {
		fprintf(stderr, "invalid row or col.\n");
		return NULL;
	}

	Color *color = (Color *)malloc(sizeof(Color));
	if (!color) {
		fprintf(stderr, "malloc failed.\n");
		return NULL;
	}

	color->R = 0;
	color->G = 0;
	color->B = 0;

	Color neighbors[8];
	neighbors[0] = image->image[INDEX_DEC(row, r)][col];
	neighbors[1] = image->image[INDEX_DEC(row, r)][INDEX_INC(col, c)];
	neighbors[2] = image->image[row][INDEX_INC(col, c)];
	neighbors[3] = image->image[INDEX_INC(row, r)][INDEX_INC(col, c)];
	neighbors[4] = image->image[INDEX_INC(row, r)][col];
	neighbors[5] = image->image[INDEX_INC(row, r)][INDEX_DEC(col, c)];
	neighbors[6] = image->image[row][INDEX_DEC(col, c)];
	neighbors[7] = image->image[INDEX_DEC(row, r)][INDEX_DEC(col, c)];

	color->R = evaluateChannel(image->image[row][col].R, neighbors, 'R', rule);
	color->G = evaluateChannel(image->image[row][col].G, neighbors, 'G', rule);
	color->B = evaluateChannel(image->image[row][col].B, neighbors, 'B', rule);

	return color;
}

//The main body of Life; given an image and a rule, computes one iteration of the Game of Life.
//You should be able to copy most of this from steganography.c
Image *life(Image *image, uint32_t rule)
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

	for (uint32_t i = 0; i < new_image->rows; i++) {
		new_image->image[i] = (Color *)malloc(sizeof(Color) * image->cols);
		if (!new_image->image[i]) {
			fprintf(stderr, "malloc failed.\n");
			freeImage(new_image);
			return NULL;
		}

		for (uint32_t j = 0; j < new_image->cols; j++) {
			Color *new_color = evaluateOneCell(image, i, j, rule);
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
Loads a .ppm from a file, computes the next iteration of the game of life, then prints to stdout the new image.

argc stores the number of arguments.
argv stores a list of arguments. Here is the expected input:
argv[0] will store the name of the program (this happens automatically).
argv[1] should contain a filename, containing a .ppm.
argv[2] should contain a hexadecimal number (such as 0x1808). Note that this will be a string.
You may find the function strtol useful for this conversion.
If the input is not correct, a malloc fails, or any other error occurs, you should exit with code -1.
Otherwise, you should return from main with code 0.
Make sure to free all memory before returning!

You may find it useful to copy the code from steganography.c, to start.
*/
int main(int argc, char **argv)
{
	if (argc != 3) {
		fprintf(stderr, "\tusage: ./gameOfLife filename rule\r\n\tfilename is an ASCII PPM file (type P3) with maximum value 255.\r\n\trule is a hex number beginning with 0x; Life is 0x1808.\r\n");
		return -1;
	}

	char *end;
    uint32_t rule = (uint32_t)strtoul(argv[2], &end, 16);
    if (*end != '\0') {
        fprintf(stderr, "\tusage: ./gameOfLife filename rule\r\n\tfilename is an ASCII PPM file (type P3) with maximum value 255.\r\n\trule is a hex number beginning with 0x; Life is 0x1808.\r\n");
        return -1;
    }

	Image *img = readData(argv[1]);
	if (!img) return -1;

	Image *new_img = life(img, rule);
	if (!new_img) {
		freeImage(img);
		return -1;
	}

	writeData(new_img);
	freeImage(img);
	freeImage(new_img);
	return 0;
}
