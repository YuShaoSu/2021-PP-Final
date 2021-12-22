#include "image.hpp"
#include <iostream>

#include "../../common/CycleTimer.h"

using namespace std;

int main(int argc, char *argv[]) {

	if(argc != 3) {
		cout << "Please input thread number and image file name!" << endl;
		return 0;
	}
	// Original image file name
	string image_file_name= "../../image/";
	image_file_name.append(argv[2]);
	string result_file_name = argv[2];
	result_file_name.append("_filter.jpg");
	int radius = 3, t_num = stoi(argv[1]);
	double sigma = 10;

	GaussianKernel kernel(radius,sigma);
	double startTime = CycleTimer::currentSeconds();
	kernel.Calculation();

	
    Image img(image_file_name, result_file_name, t_num);

	img.image_load();

	img.GaussianFliterOpenMP(kernel);

	img.image_write();
	double endTime = CycleTimer::currentSeconds();

	cout << "OpenMP done, elapsed time: " << endTime - startTime << endl;

	return 0;
}