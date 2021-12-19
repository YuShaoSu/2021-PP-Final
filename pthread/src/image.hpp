#ifndef IMAGE_HPP
#define IMAGE_HPP
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "pixel.hpp"
#include "filter.hpp"
#include <opencv2/opencv.hpp>
#include <string>
#include "thread"
using namespace std;
using namespace cv;
class Image
{

public:

	string input_file, output_file;
	int width, height, color_depth, t_num, per_t, t_remain;
	Pixel** pixels;
	Mat img;
	GaussianKernel& kernel;
	Image(string input_file, string output_file, int t_num, GaussianKernel& kernel)
	: input_file(input_file), output_file(output_file), t_num(t_num), kernel(kernel) {
	};
	
	~Image()
	{
		for(int i = 0; i < height; i++)
			delete [] pixels[i];
		delete [] pixels;
	}
	Image(Image const &) = default;
	Image(Image      &&) = default;
    Image & operator= (Image const &) = default;
	Image & operator= (Image      &&) = default;

	void image_load();
	
	void check();

	// void apply_to_pixel(const int x, const int y, const GaussianKernel& kernel);
	void apply_to_pixel(const int x, const int y);
	
	// void GaussianFilter(const GaussianKernel& kernel);
	// void GaussianFilterWorker(const GaussianKernel& kernel, int idx);
	void GaussianFilter();
	void GaussianFilterWorker(int idx);


	void image_write();
	
};

#endif //IMAGE_H
