#include "imageUtil.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
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

void templateMatchingColor(Image *src, Image *templ, Point *position, double *distance, int *rotation)
{
    if (src->channel != 3 || templ->channel != 3)
    {
        fprintf(stderr, "src and/or template image is not a color image.\n");
        return;
    }

    Image *rotated;
    int rotations[] = {0, 90, 180, 270};
    for (int i = 0; i < 4; i++)
    {
        rotated = rotateImage(templ, rotations[i]);
        for (int y = 0; y < src->height - rotated->height; y++)
        {
            for (int x = 0; x < src->width - rotated->width; x++)
            {
                if (isMatchColor(src, rotated, x, y))
                {
                    position->x = x;
                    position->y = y;
                    templ->height = rotated->height;
                    templ->width = rotated->width;
                    *rotation = rotations[i];
                    *distance = 0;
                    return;
                }
            }
        }
    }

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

            int src_r = src->data[pt + 0];
            int src_g = src->data[pt + 1];
            int src_b = src->data[pt + 2];

            int templ_r = templ->data[pt2 + 0];
            int templ_g = templ->data[pt2 + 1];
            int templ_b = templ->data[pt2 + 2];

            if (src_r != templ_r ||
                src_g != templ_g ||
                src_b != templ_b)
            {
                return 0;
            }
        }
    }
    return 1;
}

// test/beach3.ppm template /airgun_women_syufu.ppm 0 0.5 cwp
int main(int argc, char **argv)
{
    if (argc < 5)
    {
        fprintf(stderr, "Usage: templateMatching src_image temlate_image rotation threshold option(c,w,p,g)\n");
        fprintf(stderr, "Option:\nc) clear a txt result. \nw) write result a image with rectangle.\np) print results.\n");
        fprintf(stderr, "ex: templateMatching src_image.ppm temlate_image.ppm 0 1.0  \n");
        fprintf(stderr, "ex: templateMatching src_image.ppm temlate_image.ppm 0 1.0 c\n");
        fprintf(stderr, "ex: templateMatching src_image.ppm temlate_image.ppm 0 1.0 w\n");
        fprintf(stderr, "ex: templateMatching src_image.ppm temlate_image.ppm 0 1.0 p\n");
        fprintf(stderr, "ex: templateMatching src_image.ppm temlate_image.ppm 0 1.0 g\n");
        fprintf(stderr, "ex: templateMatching src_image.ppm temlate_image.ppm 0 1.0 cw\n");
        fprintf(stderr, "ex: templateMatching src_image.ppm temlate_image.ppm 0 1.0 cwp\n");
        fprintf(stderr, "ex: templateMatching src_image.ppm temlate_image.ppm 0 1.0 cwpg\n");
        return -1;
    }

    char *input_file = argv[1];
    char *template_file = argv[2];
    int rotation = atoi(argv[3]);
    double threshold = atof(argv[4]);

    printf("rotation -> %d\n", rotation);

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

    if (argc == 6)
    {
        char *p = NULL;
        if (p = strchr(argv[5], 'c') != NULL)
            clearResult(output_name_txt);
        if (p = strchr(argv[5], 'w') != NULL)
            isWriteImageResult = 1;
        if (p = strchr(argv[5], 'p') != NULL)
            isPrintResult = 1;
        if (p = strchr(argv[5], 'g') != NULL)
            isGray = 1;
    }

    Image *img = readPXM(input_file);
    Image *template = readPXM(template_file);

    Point result;
    double distance = 0.0;

    if (isGray && img->channel == 3)
    {
        Image *img_gray = createImage(img->width, img->height, 1);
        Image *template_gray = createImage(template->width, template->height, 1);
        cvtColorGray(img, img_gray);
        cvtColorGray(template, template_gray);

        templateMatchingGray(img_gray, template_gray, &result, &distance);

        freeImage(img_gray);
        freeImage(template_gray);
    }
    else
    {
        templateMatchingColor(img, template, &result, &distance, &rotation);
    }

    if (distance < threshold)
    {
        writeResult(output_name_txt, getBaseName(template_file), result, template->width, template->height, rotation, distance);
        if (isPrintResult)
        {
            printf("[Found    ] %s %d %d %d %d %d %f\n", getBaseName(template_file), result.x, result.y, template->width, template->height, rotation, distance);
        }
        if (isWriteImageResult)
        {
            drawRectangle(img, result, template->width, template->height);

            if (img->channel == 3)
                strcat(output_name_img, ".ppm");
            else if (img->channel == 1)
                strcat(output_name_img, ".pgm");
            printf("out: %s", output_name_img);
            writePXM(output_name_img, img);
        }
    }
    else
    {
        if (isPrintResult)
        {
            printf("[Not found] %s %d %d %d %d %d %f\n", getBaseName(template_file), result.x, result.y, template->width, template->height, rotation, distance);
        }
    }

    freeImage(img);
    freeImage(template);

    return 0;
}
