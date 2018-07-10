#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "../misc/ppm_file.c"
#include <CL/opencl.h>
#include "gfx_misc.c"

int min(int a, int b){
  return a < b ? a : b;
}

void sobelFilter(cl_ushort3* pixels, cl_ushort3* output, int width, int height, int weights_x[9], int weights_y[9]){
  int number_of_pixels = width * height;

  for(int x = 0; x < width; x++){
    for(int y = 0; y < height; y++){
      cl_ushort3 pixel_x;
      cl_ushort3 pixel_y;

      for(int xl = -1; xl <= 1; xl++){
        for(int yl = -1; yl <= 1; yl++){
          int new_i = (x + xl) + (yl+y) * width;
          if(new_i < 0 || new_i >= number_of_pixels){
            continue;
          }

          int w_i = (xl+1)+(yl+1)*3;

          pixel_x.x += pixels[new_i].x * weights_x[w_i];
          pixel_x.y += pixels[new_i].y * weights_x[w_i];
          pixel_x.z += pixels[new_i].z * weights_x[w_i];

          pixel_y.x += pixels[new_i].x * weights_y[w_i];
          pixel_y.y += pixels[new_i].y * weights_y[w_i];
          pixel_y.z += pixels[new_i].z * weights_y[w_i];
        }
      }

      pixel_x.x = min(abs(pixel_x.x), 255);
      pixel_x.y = min(abs(pixel_x.y), 255);
      pixel_x.z = min(abs(pixel_x.z), 255);

      pixel_y.x = min(abs(pixel_y.x), 255);
      pixel_y.y = min(abs(pixel_y.y), 255);
      pixel_y.z = min(abs(pixel_y.z), 255);

      cl_ushort3 pixel;

      pixel.x = min((pixel_x.x+pixel_y.x)* .6, 255);
      pixel.y = min((pixel_x.y+pixel_y.y)* .6, 255);
      pixel.z = min((pixel_x.z+pixel_y.z)* .6, 255);

      output[x + y*width] = pixel;
    }
  }
}

int main(int argc, char** argv){
  int i_w = 512;
  int i_h = 512;

  int weights_x[9] = {1, 0, -1, 2, 0, -2, 1, 0, -1};
  int weights_y[9] = {1, 2, 1, 0, 0, 0, -1, -2, -1};

  cl_ushort3* pixels = readPPM("lena.ppm", &i_w, &i_h);
  printf("Loaded image successfully \n");

  grayScale(pixels, i_w, i_h);

  cl_ushort3* output = (cl_ushort3*) malloc(sizeof(cl_ushort3) * i_w * i_h);
  sobelFilter(pixels, output, i_w, i_h, weights_x, weights_y);

  writePPM("lena_sobel_seq.ppm", output, i_w, i_h);

  free(pixels);
  free(output);
}
