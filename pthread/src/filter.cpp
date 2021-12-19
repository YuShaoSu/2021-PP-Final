#include "filter.hpp"
#include <thread>
#include <vector>
#include <mutex>

mutex mu;

void GaussianKernel::CalculationWorker(int idx)
{
	double local = 0.0;
	int start = -radius + idx * per_t;
	int end = idx == t_num - 1 ? radius : -radius + (idx+1) * per_t;
	for(int i = start; i <= end; i++) {
		for(int j = -radius; j <= radius; j++) {
			matrix[i+radius][j+radius] = gauss_2d(j, i, sigma);
			local += matrix[i+radius][j+radius];
		}
	}

	mu.lock();
	cout << "worker " << idx << endl;
	sum += local;
	mu.unlock();
}

void GaussianKernel::Calculation()
{
	// std::vector<std::thread> threads;
	// for(int i = 0; i < t_num; ++i) {
	// 	threads.push_back(thread(&GaussianKernel::CalculationWorker, this, i));
	// }

	// for(int i = 0; i < t_num; ++i) {
	// 	threads[i].join();
	// }

	// cout << "sum: " << sum << endl;

	// thread t(&GaussianKernel::CalculationWorker, this, 1);
	// t.join();

	// Calculate
	// parallelize this
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




