#include "imageUtil.cuh"

#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <glob.h>
// #include <omp.h>

void templateMatchingGray(Image *src, Image *templ, Point *position, double *distance)
{
	if (src->channel != 1 || templ->channel != 1)
	{
		fprintf(stderr, "src and/or templeta image is not a gray image.\n");
		return;
	}

	for (int y = 0; y < src->height - templ->height; y++)
	{
		for (int x = 0; x < src->height - templ->height; x++)
		{
			if (isMatchGray(src, templ, x, y))
			{
				position->x = x;
				position->y = y;
				*distance = 0;
				return;
			}
		}
	}

	position->x = 0;
	position->y = 0;
	*distance = INT_MAX;
	return;
}

int isMatchGray(Image *src, Image *templ, int x, int y)
{
	for (int j = 0; j < templ->height; j++)
	{
		for (int i = 0; i < templ->width; i++)
		{
			int pt = (y + j) * src->width + (x + i);
			int pt2 = j * templ->width + i;
			if (src->data[pt] != templ->data[pt2])
			{
				return 0;
			}
		}
	}
	return 1;
}

void templateMatchingColor(Image *src, Image *templ, Point *position, double *distance)
{
	if (src->channel != 3 || templ->channel != 3)
	{
		fprintf(stderr, "src and/or template image is not a color image.\n");
		return;
	}

	for (int y = 0; y < src->height - templ->height; y++)
	{
		for (int x = 0; x < src->width - templ->width; x++)
		{
			if (isMatchColor(src, templ, x, y))
			{
				position->x = x;
				position->y = y;
				*distance = 0;
				return;
			}
		}
	}

	position->x = 0;
	position->y = 0;
	*distance = INT_MAX;
	return;
}

int isMatchColor(Image *src, Image *templ, int x, int y)
{
	for (int j = 0; j < templ->height; j++)
	{
		for (int i = 0; i < templ->width; i++)
		{
			int pt = 3 * ((y + j) * src->width + (x + i));
			int pt2 = 3 * (j * templ->width + i);
			if (src->data[pt + 0] != templ->data[pt2 + 0] ||
					src->data[pt + 1] != templ->data[pt2 + 1] ||
					src->data[pt + 2] != templ->data[pt2 + 2])
			{
				return 0;
			}
		}
	}
	return 1;
}

// test/beach3.ppm template /airgun_women_syufu.ppm 0 0.5 cwp
int level1(char *input_file, char *templ_file, int rotation, double threshold, char *options)
{
	CalcTime t;
	// 初期化
	t.start();

	char output_name_base[256];
	char output_name_txt[256];
	char output_name_img[256];
	strcpy(output_name_base, "result/");
	strcat(output_name_base, getBaseName(input_file));
	strcpy(output_name_txt, output_name_base);
	strcat(output_name_txt, ".txt");
	strcpy(output_name_img, output_name_base);

	int isWriteImageResult = 0;
	int isPrintResult = 0;
	int isGray = 0;

	if (options != NULL)
	{
		char *p = NULL;
		if ((p = strchr(options, 'c')) != NULL)
			clearResult(output_name_txt);
		if ((p = strchr(options, 'w')) != NULL)
			isWriteImageResult = 1;
		if ((p = strchr(options, 'p')) != NULL)
			isPrintResult = 1;
		if ((p = strchr(options, 'g')) != NULL)
			isGray = 1;
	}

	Image *img = readPXM(input_file);
	Image *templ = readPXM(templ_file);

	Point result;
	double distance = 0.0;
	// 初期化終了
	t.end();
	printf("画像：%s\n", input_file);
	printf("テンプレートファイル：%s\n", templ_file);
	printf("初期化. %5.2lf[ms]\n", t.getAvgTime(false));

	// テンプレートマッチング開始
	t.start();
	if (isGray && img->channel == 3)
	{
		Image *img_gray = createImage(img->width, img->height, 1);
		Image *templ_gray = createImage(templ->width, templ->height, 1);
		cvtColorGray(img, img_gray);
		cvtColorGray(templ, templ_gray);

		templateMatchingGray(img_gray, templ_gray, &result, &distance);
	}
	else
	{
		templateMatchingColor(img, templ, &result, &distance);
	}
	// テンプレートマッチング終了
	t.end();
	printf("メイン. %5.2lf[ms]\n", t.getAvgTime(false));

	// 後処理開始
	t.start();
	if (distance < threshold)
	{
		writeResult(output_name_txt, getBaseName(templ_file), result, templ->width, templ->height, rotation, distance);
		if (isPrintResult)
		{
			// printf("[Found    ] %s %d %d %d %d %d %f\n", getBaseName(templ_file), result.x, result.y, templ->width, templ->height, rotation, distance);
		}
		if (isWriteImageResult)
		{
			drawRectangle(img, result, templ->width, templ->height);

			if (img->channel == 3)
				strcat(output_name_img, ".ppm");
			else if (img->channel == 1)
				strcat(output_name_img, ".pgm");
			// printf("out: %s", output_name_img);
			writePXM(output_name_img, img);
		}
	}
	else
	{
		if (isPrintResult)
		{
			// printf("[Not found] %s %d %d %d %d %d %f\n", getBaseName(templ_file), result.x, result.y, templ->width, templ->height, rotation, distance);
		}
	}

	freeImage(img);
	freeImage(templ);

	// 後処理終了
	t.end();
	printf("後処理. %5.2lf[ms]\n", t.getAvgTime(false));

	printf("\n");

	return 0;
}

void process_image(char *image, char *level)
{
	char *bname = getBaseName(image);
	strcat(bname, ".ppm");
	char *name = (char *)malloc(256);
	strcpy(name, "imgproc/");
	strcat(name, bname);
	printf("name: %s\n", name);

	int rotation = 0;

	glob_t glob_result;
	char *search_path = (char *)malloc(256);
	strcpy(search_path, level);
	strcat(search_path, "/*.ppm");
	glob(search_path, GLOB_TILDE, NULL, &glob_result);

	int x = 0;
	for (unsigned int i = 0; i < glob_result.gl_pathc; i++)
	{
		char *templ = glob_result.gl_pathv[i];
		if (x == 0)
		{
			level1(name, templ, rotation, 0.5, "cwp");
			x = 1;
		}
		else
		{
			level1(name, templ, rotation, 0.5, "wp");
		}
	}
	globfree(&glob_result);
}

int main(int argc, char *argv[])
{
	char *level = argv[1];
	glob_t glob_result;

	char input_path[256];
	strcpy(input_path, level);
	strcat(input_path, "/final/*.ppm");
	printf("test file path: %s\n", input_path);

	glob(input_path, GLOB_TILDE, NULL, &glob_result);
	for (unsigned int i = 0; i < glob_result.gl_pathc; ++i)
	{
		char *image = glob_result.gl_pathv[i];
		printf("image: %s\n", image);
		process_image(image, level);
	}
	globfree(&glob_result);
	return 0;
}