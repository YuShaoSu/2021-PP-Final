#ifndef FILTER_HPP
#define FILTER_HPP

#include <iostream>
#include <cmath>
#define PI 3.14159265

using namespace std;

class GaussianKernel
{
public:
	int radius;
	int dim;
	double **matrix;
	double sigma;
	double sum = 0.0;
	// thread variable
	int t_num, per_t, t_remain;
	GaussianKernel(const int r, const double s, const int t_num):radius(r), sigma(s), t_num(t_num)
	{
		dim = 2 * radius + 1;
		matrix = new double* [dim];
		for(int i = 0; i < dim; i++)
			matrix[i] = new double [dim];
		per_t = radius*2 / t_num;
		t_remain = (radius*2) % t_num;
	}
	~GaussianKernel()
	{
		// cout << "call of GaussianKernel destructor" << endl;
		for(int i = 0; i < dim ; i++)
			delete [] matrix[i];
		delete [] matrix;
	}
	void Calculation();
	void CalculationWorker(int idx);
};

double gauss_2d(const int x,  const int y,  const double sigma);

#endif //FILTER_H