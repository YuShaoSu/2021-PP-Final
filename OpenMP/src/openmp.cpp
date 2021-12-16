#include "image.hpp"
#include <iostream>

#include "../../common/CycleTimer.h"

using namespace std;

int main(int argc, char *argv[]) {

	// Original image file name
	string image_file_name= "../../image/big.jpg", result_file_name= "filter.jpg";
	int radius = 7;
	double sigma = 10;

	/*cout << "Original image name: " << endl;
	cin >> image_file_name;
	
	cout << "Result image name: " << endl;
	cin >> result_file_name;
    
	cout << "Radius: " << endl;  
	cin >> radius;
	
	cout << "Sigma:" << endl;
	cin >> sigma;*/

	GaussianKernel kernel(radius,sigma);
	double startTime = CycleTimer::currentSeconds();
	kernel.Calculation();
	double endTime = CycleTimer::currentSeconds();

	cout << "kernel calculation elapsed time: " << endTime - startTime << endl;

	cout << "Loading image..." << endl;
	
    Image img(image_file_name, result_file_name);

	img.image_load();

	//img.check();
	
	cout << "Appling filter..." << endl;

	startTime = CycleTimer::currentSeconds();
	img.GaussianFliterOpenMP(kernel);
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