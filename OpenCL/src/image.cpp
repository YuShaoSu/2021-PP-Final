#include "image.hpp"

void Image::image_load() 
{

	img = imread(input_file);
	if(img.empty())
	{
		cout << "file open error " << endl;
	}
    height = img.rows;
	width = img.cols;
	cout << "width: " << width << ", height: "<< height << endl;
	pixels = new Pixel[width * height];

	// Read pixels
	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			Vec3b &color = img.at<Vec3b>(i,j);//BGR
			pixels[i*width+j].R = color[2];
			pixels[i*width+j].G = color[1];
			pixels[i*width+j].B = color[0];
		}	
	}
}


void Image::check()
{
	for(int i = 0; i < height; i++)
		for(int j = 0; j < width; j++)
			cout << "r: "<< pixels[i * width + j].R << " g: "<< pixels[i * width + j].G << " b: " << pixels[i * width + j].B;
}



void Image::GaussianFliter(const GaussianKernel& kernel)
{
	// OpenCL initialization
	cl_uint num_platform, num_device;
	cl_platform_id pid;
	cl_device_id did;
//	char dname[500];
	cl_int err_platform, err_device, err_context, err_cmdq;
	err_platform = clGetPlatformIDs(1, &pid, &num_platform);
	err_device = clGetDeviceIDs(pid, CL_DEVICE_TYPE_GPU, 1, &did, &num_device);
	// clGetDeviceInfo(did,CL_DEVICE_NAME,500,dname,NULL);
	// cout<<dname<<endl;
	cl_context ctx = clCreateContext(NULL, 1, &did, 0, 0, &err_context);

	cl_command_queue cmd_queue = clCreateCommandQueueWithProperties(ctx, did, 0, &err_cmdq);  
	if(err_platform == CL_SUCCESS &&
			err_device == CL_SUCCESS &&
			err_context == CL_SUCCESS &&
			err_cmdq == CL_SUCCESS){
		// std::cout << "Initialize without error!" << std::endl;
	}
	else{
		std::cerr << "[Error] Initialize fail!" << std::endl;
		return ;
	}


	double * matrix;
    int size = width * height * sizeof(unsigned char); // image size byte
	int kernel_size = kernel.dim * kernel.dim * sizeof(double);
	// Pixel *output = pixels;
	unsigned char *r = new unsigned char [width * height];
	unsigned char *g = new unsigned char [width * height];
	unsigned char *b = new unsigned char [width * height];

	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			r[i*width+j] = pixels[i*width+j].R;
			g[i*width+j] = pixels[i*width+j].G;
			b[i*width+j] = pixels[i*width+j].B;
		}	
	}

    // opencl memory allocation
    cl_int err_mem;
	cl_mem in_img_mem_r = clCreateBuffer(ctx, CL_MEM_READ_ONLY, size, NULL, &err_mem);
	cl_mem in_img_mem_g = clCreateBuffer(ctx, CL_MEM_READ_ONLY, size, NULL, &err_mem);
	cl_mem in_img_mem_b = clCreateBuffer(ctx, CL_MEM_READ_ONLY, size, NULL, &err_mem);
    cl_mem out_img_mem_r = clCreateBuffer(ctx, CL_MEM_WRITE_ONLY, size, NULL, &err_mem);
	cl_mem out_img_mem_g = clCreateBuffer(ctx, CL_MEM_WRITE_ONLY, size, NULL, &err_mem);
	cl_mem out_img_mem_b = clCreateBuffer(ctx, CL_MEM_WRITE_ONLY, size, NULL, &err_mem);
	cl_mem kernel_mem = clCreateBuffer(ctx, CL_MEM_READ_ONLY, kernel_size, NULL, &err_mem);

    // transfer input data to the device
	err_cmdq = clEnqueueWriteBuffer(cmd_queue, in_img_mem_r, CL_TRUE, 0, size, r, 0, NULL, NULL);
	err_cmdq = clEnqueueWriteBuffer(cmd_queue, in_img_mem_g, CL_TRUE, 0, size, g, 0, NULL, NULL);
	err_cmdq = clEnqueueWriteBuffer(cmd_queue, in_img_mem_b, CL_TRUE, 0, size, b, 0, NULL, NULL);
	err_cmdq = clEnqueueWriteBuffer(cmd_queue, kernel_mem, CL_TRUE, 0, kernel_size, kernel.matrix, 0, NULL, NULL);

	// create the OpenCL kernel
	// Load cl program from file to string
	cl_program program_cl,program;
	cl_int err_program, err_build, err_kernel, err_args;
	std::ifstream srcFile("kernel.cl");
  	string srcProg(istreambuf_iterator<char>(srcFile),(istreambuf_iterator<char>()));
  	const char *src=srcProg.c_str();
  	size_t src_length=srcProg.length();
  	program_cl=clCreateProgramWithSource(ctx,1,&src,&src_length,&err_program);
	err_build = clBuildProgram(program_cl, 1, &did, "-Werror", 0, 0);

    cl_kernel kernelt = clCreateKernel(program_cl, "CL_GaussianFliter", &err_kernel);

    // set arguments of the kernel
    clSetKernelArg(kernelt, 0, sizeof(cl_mem), (void *)&in_img_mem_r);
	clSetKernelArg(kernelt, 1, sizeof(cl_mem), (void *)&in_img_mem_g);
	clSetKernelArg(kernelt, 2, sizeof(cl_mem), (void *)&in_img_mem_b);
	clSetKernelArg(kernelt, 3, sizeof(cl_mem), (void *)&out_img_mem_r);
	clSetKernelArg(kernelt, 4, sizeof(cl_mem), (void *)&out_img_mem_g);
	clSetKernelArg(kernelt, 5, sizeof(cl_mem), (void *)&out_img_mem_b);
	clSetKernelArg(kernelt, 6, sizeof(cl_mem), (void *)&kernel_mem);
	clSetKernelArg(kernelt, 7, sizeof(int), (void *)&kernel.radius);
	clSetKernelArg(kernelt, 8, sizeof(int), (void *)&width);
	clSetKernelArg(kernelt, 9, sizeof(int), (void *)&height);

	size_t globalws[2] = {(size_t) width, (size_t) height};
	// execute kernel
    clEnqueueNDRangeKernel(cmd_queue, kernelt, 2, 0, globalws, 0, 0, 0, 0);
    // copy results from device back to host
    clEnqueueReadBuffer(cmd_queue, out_img_mem_r, CL_TRUE, 0, size, r, 0, NULL, NULL);
	clEnqueueReadBuffer(cmd_queue, out_img_mem_g, CL_TRUE, 0, size, g, 0, NULL, NULL); 
    clEnqueueReadBuffer(cmd_queue, out_img_mem_b, CL_TRUE, 0, size, b, 0, NULL, NULL);

	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			pixels[i*width+j].R = r[i*width+j];
			pixels[i*width+j].G = g[i*width+j];
			pixels[i*width+j].B = b[i*width+j];
		}	
	}

//	memcpy(pixels,output,size);
    // free opencl object
	clFlush(cmd_queue);
	clFinish(cmd_queue);
    clReleaseMemObject(in_img_mem_r);
	clReleaseMemObject(in_img_mem_g);
	clReleaseMemObject(in_img_mem_b);
    clReleaseMemObject(out_img_mem_r);
	clReleaseMemObject(out_img_mem_g);
	clReleaseMemObject(out_img_mem_b);
    clReleaseMemObject(kernel_mem);
    clReleaseCommandQueue(cmd_queue);
    clReleaseContext(ctx);

	delete []r;
	delete []g;
	delete []b;
}

void Image::image_write()
{
	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			Vec3b &color = img.at<Vec3b>(i,j);//BGR
			color[0] = pixels[i*width+j].B;
			color[1] = pixels[i*width+j].G;
			color[2] = pixels[i*width+j].R;
		}	
	}
    
	imwrite(output_file, img);

}
	



