#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "../misc/ppm_file.c"
#include <CL/opencl.h>
#include "gfx_misc.c"

void gaussFilter(cl_ushort3* pixels, int width, int height, float weight[25]){
  int number_of_pixels = width * height;

  for(int x = 0; x < width; x++){
    for(int y = 0; y < height; y++){
      cl_ushort3* pixel = (cl_ushort3*) malloc(sizeof(cl_ushort3));

      for(int xl = -2; xl <= 2; xl++){
        for(int yl = -2; yl <= 2; yl++){
          int new_i = (x + xl) + (yl+y) * width;
          if(new_i < 0 || new_i >= number_of_pixels){
            continue;
          }
          float c_w = weight[(yl+2) * (xl+2)];

          pixel->x += pixels[new_i].x * c_w;
          pixel->y += pixels[new_i].y * c_w;
          pixel->z += pixels[new_i].z * c_w;
        }
      }

      pixels[x + y*width] = *pixel;
    }
  }
}

int main(int argc, char** argv){
  int i_w = 512;
  int i_h = 512;

  cl_ushort3* pixels = readPPM("lena.ppm", &i_w, &i_h);
  printf("Loaded image successfully \n");

  float weights[25];
  calculateWeights(weights);
  gaussFilter(pixels, i_w, i_h, weights);

  writePPM("lena_gaussian_seq.ppm", pixels, i_w, i_h);

  free(pixels);
}
