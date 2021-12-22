#include "image.hpp"
#include <iostream>
#include <string>
#include "../../common/CycleTimer.h"

using namespace std;

int main(int argc, char *argv[]) {

	if (argc != 3) {
		printf("Please give the number of threads and input file name!\n");
		return 0;
	}

	// Original image file name
	string image_file_name= "../../image/";
	image_file_name.append(argv[2]);
	string result_file_name = argv[2];
	result_file_name.append("_filter.jpg");	
	int radius = 5;
	double sigma = 10;
	int t_num = stoi(argv[1]);


    // Image img(image_file_name, result_file_name, t_num);
	// img.image_load();


	GaussianKernel kernel(radius,sigma,t_num);
	double startTime = CycleTimer::currentSeconds();
	kernel.Calculation();
    Image img(image_file_name, result_file_name,t_num,kernel);

	img.image_load();
	img.GaussianFilter();
	img.image_write();

	double endTime = CycleTimer::currentSeconds();


	cout << "pthread done, elapsed time: " << endTime - startTime << endl;
	return 0;
}
