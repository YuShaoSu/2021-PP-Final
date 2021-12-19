#include "image.hpp"
#include "../../common/CycleTimer.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {

	// Original image file name
	string image_file_name= "../../image/Lenna.png", result_file_name= "filter.png";
	int radius = 5;
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
	kernel.Calculation();
	cout << "Loading image..." << endl;
	
    Image img(image_file_name, result_file_name);

	img.image_load();

	//img.check();
	
	cout << "Appling filter..." << endl;
	double startTime = CycleTimer::currentSeconds();
	img.GaussianFliter(kernel);
    double endTime = CycleTimer::currentSeconds();
    double minSerial = endTime - startTime;

	cout << "GaussianFliter cuda: "<<  minSerial * 1000 << " ms" << endl;

	cout << "Writing image to disk..." << endl;
	img.image_write();

	cout << "DONE!"<< endl;

	return 0;
}