__kernel void sobel_kernel(__global ushort3 *img, __global ushort3 *output_image, __global int *weights_x,
    __global int *weights_y, int width, int number_of_pixels)
    {
      float3 pixel_x = (float3) 0;
      float3 pixel_y = (float3) 0;
      ushort3 pixel;

      int x = get_global_id(0);
      int y = get_global_id(1);

      for(int xl = -1; xl <= 1; xl++){
        for(int yl = -1; yl <= 1; yl++){
          int new_i = (x + xl) + (yl+y) * width;
          if(new_i < 0 || new_i >= number_of_pixels){
            continue;
          }

          int w_i = (xl+1)+(yl+1)*3;
          float3 w_x = (float3) weights_x[w_i];
          float3 w_y = (float3) weights_y[w_i];
          float3 cur_p = convert_float3(img[new_i]);

          pixel_x = pixel_x + (cur_p * w_x);
          pixel_y = pixel_y + (cur_p * w_y);
        }
      }

      float3 brightness = (float3) 0.80;
      pixel = convert_ushort3(brightness * (fabs(pixel_x) + fabs(pixel_y)));

      output_image[x + y*width] = min(pixel, (ushort3) 0xFF);
    }

__kernel void grayscale_kernel(__global ushort3 *img, int width)
    {
      int x = get_global_id(0);
      int y = get_global_id(1);
      float3 currentPixel = convert_float3(img[y * width + x]);

      float gray = (currentPixel.x + currentPixel.y + currentPixel.z) / 3.0f;
      float3 gray_v = (float3) gray;

      currentPixel = gray_v;

      img[y*width+x] = convert_ushort3(currentPixel);
    }
