__kernel void gaussian_kernel(__global ushort3 *img, __global ushort3 *output_image, __global float *weight,
    int width, int number_of_pixels)
    {
      ushort3 pixel;
      int x = get_global_id(0);
      int y = get_global_id(1);

      for(int xl = -2; xl <= 2; xl++){
        for(int yl = -2; yl <= 2; yl++){
          int new_i = (x + xl) + (yl+y) * width;
          if(new_i < 0 || new_i >= number_of_pixels){
            continue;
          }
          float c_w = weight[(yl+2) * (xl+2)];

          pixel.x += img[new_i].x * c_w;
          pixel.y += img[new_i].y * c_w;
          pixel.z += img[new_i].z * c_w;
        }
      }

      output_image[x + y*width] = pixel;
    }
