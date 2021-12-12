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
	cout << "width: " << width << "height: "<< height << endl;
	pixels = new Pixel*[height];
	for(int i = 0; i < height; i++)
		pixels[i] = new Pixel[width];

	// Read pixels
	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			Vec3b &color = img.at<Vec3b>(i,j);//BGR
			pixels[i][j].R = color[2];
			pixels[i][j].G = color[1];
			pixels[i][j].B = color[0];
		}	
	}
}


void Image::check()
{
	for(int i = 0; i < height; i++)
		for(int j = 0; j < width; j++)
			cout << "r: "<< pixels[i][j].R << " g: "<< pixels[i][j].G << " b: " << pixels[i][j].B;
}


//Apply a filter to the image
void Image::apply_to_pixel(const int x, const int y, const GaussianKernel& kernel)
{
	if(x < kernel.radius || y < kernel.radius || x >= width - kernel.radius || y >= height - kernel.radius) return;

	Pixel res;
	res.R = res.G = res.B = 0;
	double fil;

	for(int i = -kernel.radius; i <= kernel.radius; i++) 
		for(int j = -kernel.radius; j <= kernel.radius; j++) 
		{
			fil = kernel.matrix[i + kernel.radius][j + kernel.radius];
			res.R += fil * pixels[y + i][x + j].R;
			res.G += fil * pixels[y + i][x + j].G;
			res.B += fil * pixels[y + i][x + j].B;
		}
	
	pixels[y][x].R = res.R;
	pixels[y][x].G = res.G;
	pixels[y][x].B = res.B;
}	


void Image::GaussianFliter(const GaussianKernel& kernel)
{
	for(int y = 0; y < height; y++)
		for(int x = 0; x < width; x++)
			apply_to_pixel(x, y, kernel);
}


void Image::image_write()
{

	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			Vec3b &color = img.at<Vec3b>(i,j);//BGR
			color[0] = pixels[i][j].B;
			color[1] = pixels[i][j].G;
			color[2] = pixels[i][j].R;
		}	
	}
    
	imwrite(output_file, img);

}
	



