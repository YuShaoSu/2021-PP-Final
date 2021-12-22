#include "image.hpp"
#include <iostream>

#include "../../common/CycleTimer.h"

using namespace std;

int main(int argc, char *argv[]) {

	if(argc != 2) {
		cout << "Please input thread number and image file name!" << endl;
		return 0;
	}
	// Original image file name
	string image_file_name= "../../image/";
	image_file_name.append(argv[1]);
	string result_file_name = argv[1];
	result_file_name.append("_filter.jpg");

	int radius = 3;
	double sigma = 10;

	double startTime = CycleTimer::currentSeconds();
	GaussianKernel kernel(radius,sigma);
	kernel.Calculation();
	
    Image img(image_file_name, result_file_name);

	img.image_load();
	img.GaussianFliter(kernel);
	img.image_write();

	double endTime = CycleTimer::currentSeconds();

	cout << "OpenCL done, elapsed time: " << endTime - startTime << endl;

	return 0;
}