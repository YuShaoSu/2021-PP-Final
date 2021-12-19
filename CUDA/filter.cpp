#include "filter.hpp"

void GaussianKernel::Calculation()
{
	// Calculate
	double sum = 0.0;
	for(int i = -radius; i <= radius; i++)
		for(int j = -radius; j <= radius; j++) {
			matrix[i+radius][j+radius] = gauss_2d(j, i, sigma);
			sum += matrix[i+radius][j+radius];
		}

	// normalization
	for(int i = 0; i < dim; i++)
		for(int j = 0; j < dim; j++)
		{
			matrix[i][j] /= sum;
		}
			
}

double gauss_2d(const int x,  const int y,  const double sigma) {
	double result = 1.0 / (2 * PI * sigma * sigma);
	result *= exp(-(x*x+y*y)/(2 * sigma * sigma));
	return result;
}




