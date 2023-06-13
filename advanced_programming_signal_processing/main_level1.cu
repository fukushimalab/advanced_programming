#include "imageUtil.cuh"

#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <glob.h>

__global__ void kernelGray(Image src, Image templ, Point *position, double *distance, int *is_found)
{
	// Calculate the unique x, y coordinates for this thread
	int x = blockIdx.x * blockDim.x + threadIdx.x;
	int y = blockIdx.y * blockDim.y + threadIdx.y;

	// Check if the coordinates are within the valid range
	if (x < src.width - templ.width && y < src.height - templ.height)
	{
		if (isMatchGray(&src, &templ, x, y))
		{
			position->x = x;
			position->y = y;
			*distance = 0;
			*is_found = 1;
			return;
		}
	}
}

void templateMatchingGray(Image *src, Image *templ, Point *position, double *distance, int *is_found)
{
	Image d_img;
	d_img.channel = src->channel;
	d_img.height = src->height;
	d_img.width = src->width;
	size_t size = src->height * src->width * src->channel * sizeof(unsigned char);
	cudaMalloc((void **)&d_img.data, size);
	cudaMemcpy(d_img.data, src->data, size, cudaMemcpyHostToDevice);

	Image d_templ;
	d_templ.channel = templ->channel;
	d_templ.height = templ->height;
	d_templ.width = templ->width;
	size = templ->width * templ->height * templ->channel * sizeof(unsigned char);
	cudaMalloc((void **)&d_templ.data, size);
	cudaMemcpy(d_templ.data, templ->data, size, cudaMemcpyHostToDevice);

	Point *d_position;
	cudaMalloc((void **)&d_position, sizeof(Point));

	double *d_distance;
	cudaMalloc((void **)&d_distance, sizeof(double));

	int *d_is_found;
	cudaMalloc((void **)&d_is_found, sizeof(int));

	if (src->channel != 3 || templ->channel != 3)
	{
		// デバイス関数では printf は可能だが fprintf は不可能
		printf("src and/or template image is not a color image.\n");
		return;
	}

	int BLOCK_SIZE = 16;
	dim3 dimBlock(BLOCK_SIZE, BLOCK_SIZE);
	dim3 dimGrid((src->width - templ->width + dimBlock.x - 1) / dimBlock.x, (src->height - templ->width + dimBlock.y - 1) / dimBlock.y);

	kernelGray<<<dimGrid, dimBlock>>>(d_img, d_templ, d_position, d_distance, d_is_found);

	cudaDeviceSynchronize();

	cudaError_t error = cudaGetLastError();
	if (error != cudaSuccess)
	{
		fprintf(stderr, "%s: %s\n", __func__, cudaGetErrorString(error));
	}
	// GPUメモリをCPUメモリにコピー
	cudaMemcpy(distance, d_distance, sizeof(double), cudaMemcpyDeviceToHost);
	cudaMemcpy(position, d_position, sizeof(Point), cudaMemcpyDeviceToHost);
	cudaMemcpy(is_found, d_is_found, sizeof(int), cudaMemcpyDeviceToHost);

	// GPUメモリの解放
	cudaFree(d_img.data);
	cudaFree(d_templ.data);
	cudaFree(d_position);
	cudaFree(d_distance);
	cudaFree(d_is_found);
}

__device__ int isMatchGray(Image *src, Image *templ, int x, int y)
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

__global__ void kernelColor(Image src, Image templ, Point *position, double *distance, int *is_found)
{
	// Calculate the unique x, y coordinates for this thread
	int x = blockIdx.x * blockDim.x + threadIdx.x;
	int y = blockIdx.y * blockDim.y + threadIdx.y;

	// Check if the coordinates are within the valid range
	if (x < src.width - templ.width && y < src.height - templ.height)
	{
		if (isMatchColor(&src, &templ, x, y))
		{
			position->x = x;
			position->y = y;
			*distance = 0;
			*is_found = 1;
			return;
		}
	}
}

void templateMatchingColor(Image *src, Image *templ, Point *position, double *distance, int *is_found)
{
	Image d_img;
	d_img.channel = src->channel;
	d_img.height = src->height;
	d_img.width = src->width;
	size_t size = src->height * src->width * src->channel * sizeof(unsigned char);
	cudaMalloc((void **)&d_img.data, size);
	cudaMemcpy(d_img.data, src->data, size, cudaMemcpyHostToDevice);

	Image d_templ;
	d_templ.channel = templ->channel;
	d_templ.height = templ->height;
	d_templ.width = templ->width;
	size = templ->width * templ->height * templ->channel * sizeof(unsigned char);
	cudaMalloc((void **)&d_templ.data, size);
	cudaMemcpy(d_templ.data, templ->data, size, cudaMemcpyHostToDevice);

	Point *d_position;
	cudaMalloc((void **)&d_position, sizeof(Point));

	double *d_distance;
	cudaMalloc((void **)&d_distance, sizeof(double));

	int *d_is_found;
	cudaMalloc((void **)&d_is_found, sizeof(int));

	if (src->channel != 3 || templ->channel != 3)
	{
		// デバイス関数では printf は可能だが fprintf は不可能
		printf("src and/or template image is not a color image.\n");
		return;
	}

	int BLOCK_SIZE = 32;
	dim3 dimBlock(BLOCK_SIZE, BLOCK_SIZE);
	dim3 dimGrid((src->width - templ->width + dimBlock.x - 1) / dimBlock.x, (src->height - templ->width + dimBlock.y - 1) / dimBlock.y);

	kernelColor<<<dimGrid, dimBlock>>>(d_img, d_templ, d_position, d_distance, d_is_found);

	cudaDeviceSynchronize();

	cudaError_t error = cudaGetLastError();
	if (error != cudaSuccess)
	{
		fprintf(stderr, "%s: %s\n", __func__, cudaGetErrorString(error));
	}
	// GPUメモリをCPUメモリにコピー
	cudaMemcpy(distance, d_distance, sizeof(double), cudaMemcpyDeviceToHost);
	cudaMemcpy(position, d_position, sizeof(Point), cudaMemcpyDeviceToHost);
	cudaMemcpy(is_found, d_is_found, sizeof(int), cudaMemcpyDeviceToHost);

	// GPUメモリの解放
	cudaFree(d_img.data);
	cudaFree(d_templ.data);
	cudaFree(d_position);
	cudaFree(d_distance);
	cudaFree(d_is_found);
}

__device__ int isMatchColor(Image *src, Image *templ, int x, int y)
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
	double distance = INT_MAX;
	int is_found = 0;
	// 初期化終了

	// テンプレートマッチング開始

	// cudaMalloc((void **)&d_img, sizeof(Image));
	// cudaMalloc((void **)&d_templ, sizeof(Image));

	if (isGray && img->channel == 3)
	{
		Image *img_gray = createImage(img->width, img->height, 1);
		Image *templ_gray = createImage(templ->width, templ->height, 1);
		cvtColorGray(img, img_gray);
		cvtColorGray(templ, templ_gray);

		// テンプレートマッチング
		templateMatchingGray(img, templ, &result, &distance, &is_found);

		freeImage(img_gray);
		freeImage(templ_gray);
	}
	else
	{
		// テンプレートマッチング
		templateMatchingColor(img, templ, &result, &distance, &is_found);
	}

	// 後処理開始
	if (is_found)
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

	return 0;
}

void process_image(char *image)
{
	char *level = "level1";
	char *bname = getBaseName(image);
	strcat(bname, ".ppm");
	// 例：name = imgproc/level1_000.ppm, imgproc/level1_001.ppm
	char *name = (char *)malloc(256);
	strcpy(name, "imgproc/");
	strcat(name, bname);

	int rotation = 0;

	glob_t glob_result;
	char *search_path = (char *)malloc(256);
	strcpy(search_path, level);
	strcat(search_path, "/*.ppm");
	// level1/*.ppm （テンプレート画像）で正規表現マッチング
	glob(search_path, GLOB_TILDE, NULL, &glob_result);

	int x = 0;
	for (unsigned int i = 0; i < glob_result.gl_pathc; i++)
	{
		char *templ = glob_result.gl_pathv[i];
		if (x == 0)
		{
			// 問題の画像とテンプレート画像でテンプレートマッチング
			level1(name, templ, rotation, 0.5, "cwp");
			x = 1;
		}
		else
		{
			// 問題の画像とテンプレート画像でテンプレートマッチング
			level1(name, templ, rotation, 0.5, "wp");
		}
	}
	globfree(&glob_result);
}

int main(int argc, char *argv[])
{
	// 問題の画像のパスの正規表現
	glob_t glob_result;

	// level1/final/*.ppm にある画像の取得
	char input_path[256];
	strcat(input_path, "level1/final/*.ppm");

	// level1/final/*.ppm でマッチング
	glob(input_path, GLOB_TILDE, NULL, &glob_result);
	for (unsigned int i = 0; i < glob_result.gl_pathc; ++i)
	{
		char *image = glob_result.gl_pathv[i];
		process_image(image);
	}
	globfree(&glob_result);
	return 0;
}
