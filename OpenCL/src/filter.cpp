#include "filter.hpp"

void GaussianKernel::Calculation()
{
	// Calculate
	double sum = 0.0;
	for(int i = -radius; i <= radius; i++)
		for(int j = -radius; j <= radius; j++) {
			int idx = (i+radius) * radius + j + radius;
			matrix[idx] = gauss_2d(j, i, sigma);
			sum += matrix[idx];
		}

	// normalization
	for(int i = 0; i < dim; i++)
		for(int j = 0; j < dim; j++)
		{
			matrix[i * dim + j] /= sum;
		}
			
}

double gauss_2d(const int x,  const int y,  const double sigma) {
	double result = 1.0 / (2 * PI * sigma * sigma);
	result *= exp(-(x*x+y*y)/(2 * sigma * sigma));
	return result;
}




