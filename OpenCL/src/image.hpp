#ifndef IMAGE_HPP
#define IMAGE_HPP
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>
#include "pixel.hpp"
#include "filter.hpp"
#include <opencv2/opencv.hpp>
#include <string>
using namespace std;

class Image
{

public:

	string input_file, output_file;
	int width, height, color_depth;
	Pixel* pixels;
	cv::Mat img;
	Image(string input_file, string output_file):input_file(input_file), output_file(output_file){};
	
	~Image()
	{
		delete [] pixels;
	}
	Image(Image const &) = default;
	Image(Image      &&) = default;
    Image & operator= (Image const &) = default;
	Image & operator= (Image      &&) = default;

	void image_load();
	
	void check();
	
	void GaussianFliter(const GaussianKernel& kernel);

	void image_write();
	
};

#endif //IMAGE_H
