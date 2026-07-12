/************************************************************************
**
** NAME:        imageloader.c
**
** DESCRIPTION: CS61C Fall 2020 Project 1
**
** AUTHOR:      Dan Garcia  -  University of California at Berkeley
**              Copyright (C) Dan Garcia, 2020. All rights reserved.
**              Justin Yokota - Starter Code
**				YOUR NAME HERE
**
**
** DATE:        2020-08-15
**
**************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include "imageloader.h"

//Opens a .ppm P3 image file, and constructs an Image object. 
//You may find the function fscanf useful.
//Make sure that you close the file with fclose before returning.
Image *readData(char *filename) 
{
	FILE *fp;
	Image *image;

	fp = fopen(filename, "r");
	if (fp == NULL) {
		fprintf(stderr, "fopen failed\n");
		return NULL;
	}

	image = (Image *)malloc(sizeof(Image));
	image->image = NULL;

	char magic[3];
	if (fscanf(fp, "%2s", magic) != 1) {
		fprintf(stderr, "wrong format\n");
		fclose(fp);
		freeImage(image);
		return NULL;
	}

	if (strcmp(magic, "P3") != 0) {
		fprintf(stderr, "wrong format\n");
		fclose(fp);
		freeImage(image);
		return NULL;
	}

	if (fscanf(fp, "%u %u", &image->cols, &image->rows) != 2) {
		fprintf(stderr, "wrong format\n");
		fclose(fp);
		freeImage(image);
		return NULL;
	}

	uint32_t maxVal;
	if (fscanf(fp, "%u", &maxVal) != 1) {
		fprintf(stderr, "wrong format\n");
		fclose(fp);
		freeImage(image);
		return NULL;
	}

	image->image = (Color **)calloc(image->rows, sizeof(Color*));
	for (uint32_t i = 0; i < image->rows; i++) {
		image->image[i] = malloc(sizeof(Color) * image->cols);
		for (uint32_t j = 0; j < image->cols; j++) {
			if(fscanf(fp, "%hhu %hhu %hhu", &image->image[i][j].R, &image->image[i][j].G, &image->image[i][j].B) != 3) {
				fprintf(stderr, "wrong format\n");
				fclose(fp);
				freeImage(image);
				return NULL;
			}
		}
	}

	fclose(fp);
	return image;
}

//Given an image, prints to stdout (e.g. with printf) a .ppm P3 file with the image's data.
void writeData(Image *image)
{
	printf("P3\n");
	printf("%u %u\n", image->cols, image->rows);
	printf("255\n");

	for (uint32_t i = 0; i < image->rows; i++) {
		for (uint32_t j = 0; j < image->cols; j++) {
			printf("%3hhu %3hhu %3hhu", image->image[i][j].R, image->image[i][j].G, image->image[i][j].B);

			if (j < image->cols - 1) {
				printf("   ");
			} else {
				printf("\n");
			}
		}
	}
}

//Frees an image
void freeImage(Image *image)
{
	if (!image) {
		return;
	}

	if (!image->image) {
		free(image);
		return;
	}

	for (uint32_t i = 0; i < image->rows; i++) {
		if (image->image[i]) {
			free(image->image[i]);
		}
	}
	free(image->image);
	free(image);
	return;
}