#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#define THREAD 16

__device__ int mandel(float c_re, float c_im, int maxIteration)
{
  float z_re = c_re, z_im = c_im;
  int i;
  for (i = 0; i < maxIteration; ++i)
  {

    if (z_re * z_re + z_im * z_im > 4.f)break;

    float new_re = z_re * z_re - z_im * z_im;
    float new_im = 2.f * z_re * z_im;
    z_re = c_re + new_re;
    z_im = c_im + new_im;
  }

  return i;
}


__global__ void mandelKernel(float lowerX, float lowerY, int* Pd, float stepX, float stepY, int width, int height) {
    // To avoid error caused by the floating number, use the following pseudo code
    //
    // float x = lowerX + thisX * stepX;
    // float y = lowerY + thisY * stepY;
    
    // calculate thisX, thisY
    int idx, idy;
    idx = blockIdx.x * blockDim.x + threadIdx.x; //thread block
    idy = blockIdx.y * blockDim.y + threadIdx.y; //thread block
    
    if(idx >= resX || idy >= resY)return;
    float x = lowerX + idx * stepX;
    float y = lowerY + idy * stepY;

    int index = idy * resX + idx;
    Pd[index] = mandel(x, y);
   
}

// Host front-end function that allocates the memory and launches the GPU kernel

//x1, y1, x0, y0, output, width, height, maxIterations

void hostFE (float upperX, float upperY, float lowerX, float lowerY, int* img, int width, int height)
{
    float stepX = (upperX - lowerX) / width;
    float stepY = (upperY - lowerY) / height;

    int blocksNumX = (int) ceil(width / (float) THREAD);
    int blocksNumY = (int) ceil(height / (float) THREAD);

    int *Pd;
    int size = width * height * sizeof(int); // image size

    cudaMalloc(&Pd, size); 
    dim3 threadPerBlock(THREAD, THREAD);
    dim3 numBlocks(blocksNumX, blocksNumY);
    mandelKernel <<< numBlocks, threadPerBlock >>> (lowerX, lowerY, Pd, stepX, stepY, width, height);
    
    cudaMemcpy(img, Pd, size, cudaMemcpyDeviceToHost);
    
    cudaFree(Pd);
}
