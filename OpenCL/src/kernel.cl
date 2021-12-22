__kernel void CL_GaussianFliter(__global unsigned char* cl_input_r,
								__global unsigned char* cl_input_g, 
								__global unsigned char* cl_input_b,  
								__global unsigned char* cl_output_r,
								__global unsigned char* cl_output_g,
								__global unsigned char* cl_output_b,
								__global double* matrix,
								int radius,
								int width,
								int height) 
{
    
    int col = get_global_id(0);
    int row = get_global_id(1);
	unsigned char resR = 0;
	unsigned char resG = 0;
	unsigned char resB = 0;
	double fil;

	// if(col< width && row < height)
	// {	
		if (row > radius && col > radius && col < width - radius && row < height - radius)
		{

			for(int i = -radius; i <= radius; i++) 
				for(int j = -radius; j <= radius; j++) 
				{

					fil = matrix[(i + radius) * radius + j + radius];
					resR += fil * cl_input_r[(row + i) * width + col + j];
					resG += fil * cl_input_g[(row + i) * width + col + j];
					resB += fil * cl_input_b[(row + i) * width + col + j];
					
				}
		}

		cl_output_r[row * width + col] = resR;
		cl_output_g[row * width + col] = resG;
		cl_output_b[row * width + col] = resB;
	// }
	
}