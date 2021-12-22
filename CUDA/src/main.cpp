#include "image.hpp"
#include "../../common/CycleTimer.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {

	// Original image file name
	if(argc != 2) {
		cout << "Please input one parameter for image file name!" << endl;
		return 0;
	}
	// Original image file name
	string image_file_name= "../../image/";
	image_file_name.append(argv[1]);
	string result_file_name = argv[1];
	result_file_name.append("_filter.jpg");
	
	int radius = 5;
	double sigma = 10;

	double startTime = CycleTimer::currentSeconds();
	GaussianKernel kernel(radius,sigma);
	kernel.Calculation();
	
    Image img(image_file_name, result_file_name);

	img.image_load();

	img.GaussianFliter(kernel);
    // double minSerial = endTime - startTime;
	// cout << "GaussianFliter cuda: "<<  minSerial * 1000 << " ms" << endl;

	img.image_write();
    double endTime = CycleTimer::currentSeconds();

	cout << "CUDA done, elapsed time: " << endTime - startTime << endl;

	return 0;
}
