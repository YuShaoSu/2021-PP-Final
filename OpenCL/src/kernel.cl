__kernel void CL_GaussianFliter(__global Pixel* cl_input, __global Pixel* cl_output,__global double** matrix,__global const int radius,__global const int width,__global const int height) 
{
    
    int col = get_global_id(0);
    int row = get_global_id(1);

	if(col< width && row < height)
	{	
		Pixel res;
		res.R = res.G = res.B = 0;
		double fil;
		if (row > radius && col > radius && col < width - radius && row < height - radius)
		{

			for(int i = -radius; i <= radius; i++) 
				for(int j = -radius; j <= radius; j++) 
				{
					fil = matrix[i + radius][j + radius];
					res.R += fil * cl_input[(row + i) * width + col + j].R;
					res.G += fil * cl_input[(row + i) * width + col + j].G;
					res.B += fil * cl_input[(row + i) * width + col + j].B;
					
				}
		}

		cl_output[row * width + col] = res;
	}
	
}