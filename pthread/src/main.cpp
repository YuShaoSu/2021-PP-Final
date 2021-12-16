#include "image.hpp"
#include <iostream>
#include <string>
#include "../../common/CycleTimer.h"

using namespace std;

int main(int argc, char *argv[]) {

	if (argc != 2) {
		printf("Please give the number of threads!\n");
		return 0;
	}

	// Original image file name
	string image_file_name= "../../image/big.jpg", result_file_name= "filter.jpg";
	int radius = 7;
	double sigma = 10;
	int t_num = stoi(argv[1]);


    // Image img(image_file_name, result_file_name, t_num);
	// img.image_load();


	GaussianKernel kernel(radius,sigma,t_num);
	double startTime = CycleTimer::currentSeconds();
	kernel.Calculation();
	double endTime = CycleTimer::currentSeconds();

	cout << "kernel calculation elapsed time: " << endTime - startTime << endl;

	cout << "Loading image..." << endl;
	
    Image img(image_file_name, result_file_name,t_num,kernel);

	img.image_load();

	//img.check();
	
	cout << "Appling filter..." << endl;

	startTime = CycleTimer::currentSeconds();
	// img.GaussianFilter(kernel);
	img.GaussianFilter();
	endTime = CycleTimer::currentSeconds();
	cout << "Apply filter elapsed time: " << endTime - startTime << endl;


	cout << "Writing image to disk..." << endl;
	startTime = CycleTimer::currentSeconds();
	img.image_write();
	endTime = CycleTimer::currentSeconds();
	cout << "image write elapsed time: " << endTime - startTime << endl;


	cout << "DONE!"<< endl;

	return 0;
}
