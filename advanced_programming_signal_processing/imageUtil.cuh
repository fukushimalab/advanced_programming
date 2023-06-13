#pragma once
#include <vector>
#include <ctime>

typedef struct Image
{
	int width;
	int height;
	int channel;
	unsigned char *data;
} Image;

typedef struct Point
{
	int x;
	int y;
} Point;

struct CalcTime
{
	std::vector<double> que;
	timespec s;
	timespec e;
	void start();
	void end();
	void clear();

	double getAvgTime(const bool dropFirstMeasure = true, const bool isClear = true);
	double getLastTime();
};

int isMatchGray(Image *src, Image *templ, int x, int y);
int isMatchColor(Image *src, Image *templ, int x, int y);
void printImageInfo(Image *src);
void freeImage(Image *img);
Image *createImage(const int width, const int height, const int channels);
Image *cloneImage(const Image *src);
Image *readPXM(const char *name);
void writePXM(const char *name, Image *src);
void cvtColorGray(const Image *src, Image *dest);
void drawRectangle(Image *src, const Point pt, const int template_width, const int template_height);

int isMatchGray(Image *src, Image *templ, int x, int y);
int isMatchColor(Image *src, Image *templ, int x, int y);
char *getDirAndBaseName(const char *name);
char *getBaseName(const char *name);

char *getBaseName(const char *name);
char *getDirAndBaseName(const char *name);

void writeResult(const char *file_name, const char *template_name, const Point result, const int width, const int height, const int rotation, const double distance);
void clearResult(const char *file_name);