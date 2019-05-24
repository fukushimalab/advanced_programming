#pragma once

typedef struct Image
{
	int width;
	int height;
	int channel;
	unsigned char* data;
}Image;

typedef struct Point
{
	int x;
	int y;
}Point;

void freeImage(Image* img);
Image* createImage(const int width, const int height, const int channels);
Image* cloneImage(const Image* src);
Image* readPXM(const char* name);
void writePXM(const char* name, Image* src);
void cvtColorGray(const Image* src, Image* dest);
void drawRectangle(Image* src, const Point pt, const int template_width, const int template_height);

char* getBaseName(const char* name);
char* getDirAndBaseName(const char* name);


void writeResult(const char* file_name, const char* template_name, const Point result, const int width, const int height, const int rotation, const double distance);
void clearResult(const char* file_name);