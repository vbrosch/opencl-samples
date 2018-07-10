#include <stdio.h>
#include <stdlib.h>
#include <CL/opencl.h>

#define RGB_COMPONENT_COLOR 255
#define CREATOR "VB"

static cl_ushort3 *readPPM(const char *filename, int *height, int *width)
{
    char buff[16];
    FILE *fp;
    int c, rgb_comp_color;
    //open PPM file for reading
    fp = fopen(filename, "rb");

    if (!fp)
    {
        fprintf(stderr, "Unable to open file '%s'\n", filename);
        exit(1);
    }

    //read image format
    if (!fgets(buff, sizeof(buff), fp))
    {
        perror(filename);
        exit(1);
    }

    //check the image format
    if (buff[0] != 'P' || buff[1] != '6')
    {
        fprintf(stderr, "Invalid image format (must be 'P6')\n");
        exit(1);
    }

    //check for comments
    c = getc(fp);
    while (c == '#')
    {
        while (getc(fp) != '\n')
            ;
        c = getc(fp);
    }

    ungetc(c, fp);
    //read image size information
    if (fscanf(fp, "%d %d", width, height) != 2)
    {
        fprintf(stderr, "Invalid image size (error loading '%s')\n", filename);
        exit(1);
    }

    //read rgb component
    if (fscanf(fp, "%d", &rgb_comp_color) != 1)
    {
        fprintf(stderr, "Invalid rgb component (error loading '%s')\n", filename);
        exit(1);
    }

    //check rgb component depth
    if (rgb_comp_color != RGB_COMPONENT_COLOR)
    {
        fprintf(stderr, "'%s' does not have 8-bits components\n", filename);
        exit(1);
    }

    while (fgetc(fp) != '\n')
        ;

    int number_of_pixels = *width * *height;
    int number_of_rgb_values = number_of_pixels * 3;

    //memory allocation for pixel data
    unsigned char *rgb_pixels = (unsigned char*)malloc(number_of_rgb_values);

    if (!rgb_pixels)
    {
        fprintf(stderr, "Unable to allocate memory\n");
        exit(1);
    }

    //read pixel data from file
    if (fread(rgb_pixels, 3 * *width, *height, fp) != *height)
    {
        fprintf(stderr, "Error loading image '%s'\n", filename);
        exit(1);
    }

    cl_ushort3 *pixels = (cl_ushort3*) malloc(number_of_pixels * sizeof(cl_ushort3));

    for(int i = 0; i < number_of_pixels; i++){
        pixels[i].x = rgb_pixels[3*i];
        pixels[i].y = rgb_pixels[3*i+1];
        pixels[i].z = rgb_pixels[3*i+2];
    }

    free(rgb_pixels);

    fclose(fp);
    return pixels;
}
void writePPM(const char *filename, cl_ushort3 *pixels, int width, int height)
{
    FILE *fp;
    //open file for output
    fp = fopen(filename, "wb");
    if (!fp)
    {
        fprintf(stderr, "Unable to open file '%s'\n", filename);
        exit(1);
    }

    //write the header file
    //image format
    fprintf(fp, "P6\n");

    //comments
    fprintf(fp, "# Created by %s\n", CREATOR);

    //image size
    fprintf(fp, "%d %d\n", width, height);

    // rgb component depth
    fprintf(fp, "%d\n", RGB_COMPONENT_COLOR);

    int number_of_pixels = width * height;
    int number_of_rgb_values = number_of_pixels * 3;
    unsigned char* rgb_pixels = (unsigned char*) malloc(sizeof(unsigned char) * number_of_rgb_values);

    for(int i=0; i < number_of_pixels; i++){
      rgb_pixels[3*i] = pixels[i].x;
      rgb_pixels[3*i+1] = pixels[i].y;
      rgb_pixels[3*i+2] = pixels[i].z;
    }

    // pixel data
    fwrite(rgb_pixels, 3 * width, height, fp);
    fclose(fp);
}
