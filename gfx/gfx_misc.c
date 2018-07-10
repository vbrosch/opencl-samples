#include <math.h>

void calculateWeights(float oneDimensionalWeights[25])
{
    float weights[5][5];
    float sigma = 5.0;
    float r, s = 2.0 * sigma * sigma;

    // sum is for normalization
    float sum = 0.0;

    // generate weights for 5x5 kernel
    for (int x = -2; x <= 2; x++)
    {
        for (int y = -2; y <= 2; y++)
        {
            r = x * x + y * y;
            weights[x + 2][y + 2] = exp(-(r / s)) / (M_PI * s);
            sum += weights[x + 2][y + 2];
        }
    }

    // normalize the weights
    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < 5; ++j)
        {
            weights[i][j] /= sum;
            oneDimensionalWeights[i * j] = weights[i][j];
        }
    }
}

void grayScale(cl_ushort3* pixels, int width, int height){
  for(int x = 0; x < width; x++){
    for(int y = 0; y < height; y++){
      cl_ushort3 pixel = pixels[x + y*width];

      unsigned char gray = (pixel.x + pixel.y + pixel.z)/3;

      pixels[x + y*width].x = gray;
      pixels[x + y*width].y = gray;
      pixels[x + y*width].z = gray;
    }
  }
}
