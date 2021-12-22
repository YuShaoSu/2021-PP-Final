#include "image.hpp"

void Image::image_load() 
{
	img = imread(input_file);
	if(img.empty())
	{
		cout << "file open error " << endl;
	}
    height = img.rows;
	width = img.cols;
	cout << "width: " << width << ", height: "<< height << endl;
	
	pixels = new Pixel[width * height];

	// Read pixels
	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			Vec3b &color = img.at<Vec3b>(i,j);//BGR
			pixels[i*width+j].R = color[2];
			pixels[i*width+j].G = color[1];
			pixels[i*width+j].B = color[0];
		}	
	}
}


void Image::check()
{
	for(int i = 0; i < height; i++)
		for(int j = 0; j < width; j++)
			cout << "r: "<< pixels[i * width + j].R << " g: "<< pixels[i * width + j].G << " b: " << pixels[i * width + j].B;
}



__global__ void cudaGaussianFliter(Pixel* input_cuda, Pixel* output_cuda, double* matrix, const int radius, const int width, const int height) 
{
    
    int col = blockIdx.x * blockDim.x + threadIdx.x; //thread block
    int row = blockIdx.y * blockDim.y + threadIdx.y; //thread block

	if(col< width && row < height)
	{	
		Pixel res;
		res.R = res.G = res.B = 0;
		double fil;
		if (row >= radius && col >= radius && col < width - radius && row < height - radius)
		{

			for(int i = -radius; i <= radius; i++) 
				for(int j = -radius; j <= radius; j++) 
				{
					fil = matrix[(i + radius) * radius + j + radius];
					res.R += fil * input_cuda[(row + i) * width + col + j].R;
					res.G += fil * input_cuda[(row + i) * width + col + j].G;
					res.B += fil * input_cuda[(row + i) * width + col + j].B;
					
				}
		}

		output_cuda[row * width + col] = res;
	}
}


void Image::GaussianFliter(const GaussianKernel& kernel)
{

    Pixel *input_cuda, *output_cuda;
	Pixel *output = new Pixel[width * height];
	double * matrix;

    int size = width * height * sizeof(Pixel); // image size byte
	int kernel_size = kernel.dim * kernel.dim * sizeof(double);

    cudaMalloc(&input_cuda, size); 
	cudaMalloc(&output_cuda, size); 
	cudaMalloc(&matrix, kernel_size);

	cudaMemcpy(input_cuda, pixels, size, cudaMemcpyHostToDevice);
	cudaMemcpy(output_cuda, output, size, cudaMemcpyHostToDevice);
	cudaMemcpy(matrix, kernel.matrix, kernel_size, cudaMemcpyHostToDevice);
	
	int block = (int) sqrt(THREAD);
    int blocksNumX = (width + block - 1) /  block;
    int blocksNumY = (height + block - 1) / block;

    dim3 threadPerBlock(block, block);
    dim3 numBlocks(blocksNumX, blocksNumY);
    cudaGaussianFliter <<< numBlocks, threadPerBlock >>> (input_cuda, output_cuda, matrix, kernel.radius, width, height);

    cudaMemcpy(output, output_cuda, size, cudaMemcpyDeviceToHost);
	
	memcpy(pixels, output, size);

	delete [] output;
    cudaFree(input_cuda);
	cudaFree(output_cuda);
	cudaFree(matrix);
}



void Image::image_write()
{

	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			Vec3b &color = img.at<Vec3b>(i,j);//BGR
			color[0] = pixels[i*width+j].B;
			color[1] = pixels[i*width+j].G;
			color[2] = pixels[i*width+j].R;
		}	
	}
    
	imwrite(output_file, img);

}
	



