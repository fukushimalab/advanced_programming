#include "imageUtil.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printImageInfo(Image *src)
{
	printf("w h c = %d %d %d\n", src->width, src->height, src->channel);
}

void freeImage(Image *img)
{
	free(img->data);
	free(img);
}

Image *cloneImage(const Image *src)
{
	Image *img = (Image *)malloc(sizeof(Image));

	img->width = src->width;
	img->height = src->height;
	img->channel = src->channel;
	const int size = sizeof(unsigned char) * src->width * src->height * src->channel;
	img->data = (unsigned char *)malloc(size);
	memcpy(img->data, src->data, size);
	return img;
}

Image *createImage(const int width, const int height, const int channels)
{
	Image *img = (Image *)malloc(sizeof(Image));

	img->width = width;
	img->height = height;
	img->channel = channels;
	img->data = (unsigned char *)malloc(sizeof(unsigned char) * width * height * channels);
	return img;
}

Image *readPXM(const char *name)
{
	Image *dest = (Image *)malloc(sizeof(Image));

	char buffer[256];
	FILE *fp = fopen(name, "rb");
	if (fp == NULL)
	{
		fprintf(stderr, "file open error %s\n", name);
		return NULL;
	}

	fgets(buffer, 256, fp);
	if (buffer[1] == '3' || buffer[1] == '6')
		dest->channel = 3; // color
	if (buffer[1] == '2' || buffer[1] == '5')
		dest->channel = 1; // binary

	for (;;)
	{
		if (fgetc(fp) == '#')
		{
			fgets(buffer, 256, fp); // skip comment
			continue;
		}
		else
		{
			fseek(fp, -1, SEEK_CUR);
			fscanf(fp, "%d %d", &dest->width, &dest->height);
			break;
		}
	}
	fscanf(fp, "%s\n", buffer); // skip intensity
	// printImageInfo(dest);

	int size = dest->width * dest->height * dest->channel;
	dest->data = (unsigned char *)malloc(sizeof(unsigned char) * size);
	fread(dest->data, sizeof(unsigned char), size, fp);

	fclose(fp);
	return dest;
}

Image *rotateImage(const Image *src, const int angle)
{
	// Check angle is valid (0, 90, 180, 270)
	if (angle != 0 && angle != 90 && angle != 180 && angle != 270)
	{
		fprintf(stderr, "Invalid angle. Must be 0, 90, 180, or 270.\n");
		return;
	}

	// For 0 and 180 degrees, width and height remain the same.
	// For 90 and 270, they are swapped.
	Image *dest = (Image *)malloc(sizeof(Image));
	if (angle == 0 || angle == 180)
	{
		dest->width = src->width;
		dest->height = src->height;
	}
	else
	{
		dest->width = src->height;
		dest->height = src->width;
	}

	dest->channel = src->channel;
	dest->data = (unsigned char *)malloc(dest->width * dest->height * dest->channel * sizeof(unsigned char));

	for (int y = 0; y < src->height; y++)
	{
		for (int x = 0; x < src->width; x++)
		{
			for (int c = 0; c < src->channel; c++)
			{
				int src_index = (y * src->width + x) * src->channel + c;
				int dest_index;

				switch (angle)
				{
				case 0:
					dest_index = src_index;
					break;
				case 90:
					dest_index = (x * dest->width + (src->height - y - 1)) * dest->channel + c;
					break;
				case 180:
					dest_index = ((src->height - y - 1) * dest->width + (src->width - x - 1)) * dest->channel + c;
					break;
				case 270:
					dest_index = ((src->width - x - 1) * dest->width + y) * dest->channel + c;
					break;
				}

				dest->data[dest_index] = src->data[src_index];
			}
		}
	}
	return dest;
}

void writePXM(const char *name, Image *src)
{
	FILE *fp = fopen(name, "wb");
	if (fp == NULL)
	{
		fprintf(stderr, "file open error %s\n", name);
		return;
	}

	if (src->channel == 1)
	{
		fprintf(fp, "%s\n", "P5");
	}
	else if (src->channel == 3)
	{
		fprintf(fp, "%s\n", "P6");
	}

	fprintf(fp, "# Created by Programming OUYOU\n");
	fprintf(fp, "%d %d\n", src->width, src->height);
	fprintf(fp, "255\n");

	int size = src->width * src->height * src->channel;
	fwrite(src->data, sizeof(unsigned char), size, fp);
	fprintf(fp, "\n");
	fclose(fp);
}

void cvtColorGray(const Image *src, Image *dest)
{
	if (src->width != dest->width || src->height != dest->height)
	{
		fprintf(stderr, "src and dest image have different size.\n");
		return;
	}
	if (dest->channel != 1)
	{
		fprintf(stderr, "dest image is not 1 channel.\n");
	}
	int size = src->width * src->height;

	int i;
	for (i = 0; i < size; i++)
	{
		dest->data[i] = (unsigned char)(((src->data[3 * i + 0] + src->data[3 * i + 1] + src->data[3 * i + 2]) * 0.3333f) + 0.5f);
	}
}

void drawRectangle(Image *src, const Point pt, const int template_width, const int template_height)
{
	if (src->channel == 1)
	{
		int i, j;
		for (i = 0; i < template_width; i++)
		{
			src->data[src->width * pt.y + pt.x + i] = 255;
			src->data[src->width * (pt.y + template_height) + pt.x + i] = 255;
		}
		for (j = 0; j < template_height; j++)
		{
			src->data[src->width * (pt.y + j) + pt.x] = 255;
			src->data[src->width * (pt.y + j) + pt.x + template_width] = 255;
		}
	}
	else if (src->channel == 3)
	{
		int i, j;
		for (i = 0; i < template_width; i++)
		{
			src->data[3 * (src->width * pt.y + pt.x + i) + 0] = 255;
			src->data[3 * (src->width * pt.y + pt.x + i) + 1] = 0;
			src->data[3 * (src->width * pt.y + pt.x + i) + 2] = 0;
			src->data[3 * (src->width * (pt.y + template_height) + pt.x + i) + 0] = 255;
			src->data[3 * (src->width * (pt.y + template_height) + pt.x + i) + 1] = 0;
			src->data[3 * (src->width * (pt.y + template_height) + pt.x + i) + 2] = 0;
		}
		for (j = 0; j < template_height; j++)
		{
			src->data[3 * (src->width * (pt.y + j) + pt.x) + 0] = 255;
			src->data[3 * (src->width * (pt.y + j) + pt.x) + 1] = 0;
			src->data[3 * (src->width * (pt.y + j) + pt.x) + 2] = 0;
			src->data[3 * (src->width * (pt.y + j) + pt.x + template_width) + 0] = 255;
			src->data[3 * (src->width * (pt.y + j) + pt.x + template_width) + 1] = 0;
			src->data[3 * (src->width * (pt.y + j) + pt.x + template_width) + 2] = 0;
		}
	}
}

char *getDirAndBaseName(const char *name)
{
	int len = (int)strlen(name);
	char *ret = (char *)malloc(len);
	memcpy(ret, name, len);

	char *ppt;
	if ((ppt = strchr(ret, '.')) != NULL)
		*ppt = '\0';

	return ret;
}

char *getBaseName(const char *name)
{
	int len = (int)strlen(name);
	char *ret = (char *)malloc(len);
	memcpy(ret, name, len);
	int count = 0;
	int i;
	for (i = 0; i < len; i++)
	{
		if (ret[i] == '/')
			count++;
	}

	for (i = 0; i < count; i++)
	{
		ret = strchr(ret, '/');
	}

	char *ppt;
	if ((ppt = strchr(ret, '.')) != NULL)
		*ppt = '\0';

	return ret + 1;
}

void writeResult(const char *file_name, const char *template_name, const Point result, const int width, const int height, const int rotation, const double distance)
{
	FILE *fp = fopen(file_name, "a");
	if (fp == NULL)
	{
		fprintf(stderr, "file open error (result) %s\n", file_name);
		return;
	}
	fprintf(fp, "%s %d %d %d %d %d %f\n", template_name, result.x, result.y, width, height, rotation, distance);
	fclose(fp);
}

void clearResult(const char *file_name)
{
	FILE *fp = fopen(file_name, "w");
	if (fp == NULL)
	{
		return;
	}
	fclose(fp);
}
