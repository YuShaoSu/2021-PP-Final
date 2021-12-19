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
	cout << "width: " << width << "height: "<< height << endl;
	pixels = new Pixel*[height];
	for(int i = 0; i < height; i++)
		pixels[i] = new Pixel[width];

	// Read pixels
	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			Vec3b &color = img.at<Vec3b>(i,j);//BGR
			pixels[i][j].R = color[2];
			pixels[i][j].G = color[1];
			pixels[i][j].B = color[0];
		}	
	}
}


void Image::check()
{
	for(int i = 0; i < height; i++)
		for(int j = 0; j < width; j++)
			cout << "r: "<< pixels[i][j].R << " g: "<< pixels[i][j].G << " b: " << pixels[i][j].B;
}



void Image::GaussianFliter(const GaussianKernel& kernel)
{
	// OpenCL initialization
	cl_uint num_platform, num_device;
	cl_platform_id pid;
	cl_device_id did;
	cl_int err_platform, err_device, err_context, err_cmdq;
	err_platform = clGetPlatformIDs(1, &pid, &num_platform);
	err_device = clGetDeviceIDs(pid, CL_DEVICE_TYPE_GPU, 1, &did, &num_device);
	cl_context ctx = clCreateContext(NULL, 1, &did, 0, 0, &err_context);
	cl_command_queue cmd_queue = clCreateCommandQueueWithProperties(ctx, did, 0, &err_cmdq);  
	if(err_platform == CL_SUCCESS &&
			err_device == CL_SUCCESS &&
			err_context == CL_SUCCESS &&
			err_cmdq == CL_SUCCESS){
		std::cout << "Initialize without error!" << std::endl;
	}
	else{
		std::cerr << "[Error] Initialize fail!" << std::endl;
		return ;
	}

    Pixel *cl_input, *cl_output;
	Pixel *output = new Pixel[width * height];
	double ** matrix;

    int size = width * height * sizeof(Pixel); // image size byte
	int kernel_size = kernel.dim * kernel.dim * sizeof(double);

    // opencl memory allocation
    cl_int err_mem;
    cl_mem in_img_mem = clCreateBuffer(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, size, &cl_input, &err_mem);
    if(err_mem != CL_SUCCESS) std::cerr << "[Error] clCreateBuffer" << std::endl;
    cl_mem out_img_mem = clCreateBuffer(ctx, CL_MEM_WRITE_ONLY, size, &cl_output, &err_mem);
    if(err_mem != CL_SUCCESS) std::cerr << "[Error] clCreateBuffer" << std::endl;
    cl_mem kernel_mem = clCreateBuffer(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, kernel_size, &matrix, &err_mem);
    if(err_mem != CL_SUCCESS) std::cerr << "[Error] clCreateBuffer" << std::endl;
    cl_mem width_mem = clCreateBuffer(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int), &width, &err_mem);
    if(err_mem != CL_SUCCESS) std::cerr << "[Error] clCreateBuffer" << std::endl;
    cl_mem height_mem = clCreateBuffer(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int), &height, &err_mem);	
    if(err_mem != CL_SUCCESS) std::cerr << "[Error] clCreateBuffer" << std::endl;

    // transfer input data to the device
	err_cmdq = clEnqueueWriteBuffer(cmd_queue, in_img_mem, CL_TRUE, 0, size, cl_input, 0, NULL, NULL);
	err_cmdq = clEnqueueWriteBuffer(cmd_queue, kernel_mem, CL_TRUE, 0, kernel_size, matrix, 0, NULL, NULL);

	// create the OpenCL kernel
	// Load cl program from file to string
	fstream fs("./kernel.cl", std::ios_base::in);
	stringstream ss;
	string gf_str;
	ss << fs.rdbuf();
	gf_str = ss.str();
	const char* gf_cstr = gf_str.c_str();
	size_t gf_len = gf_str.size();
	// opencl create and build program
	cl_int err_program, err_build, err_kernel, err_args;
	cl_program program = clCreateProgramWithSource(ctx, 1, &gf_cstr, &gf_len, &err_program);
    cl_kernel kernelt = clCreateKernel(program, "CL_GaussianFliter", &err_program);

    // set arguments of the kernel
    clSetKernelArg(kernelt, 0, sizeof(cl_mem), (void *)&in_img_mem);
	clSetKernelArg(kernelt, 1, sizeof(cl_mem), (void *)&out_img_mem);
	clSetKernelArg(kernelt, 2, sizeof(cl_mem), (void *)&kernel_mem);
	clSetKernelArg(kernelt, 3, sizeof(int), (void *)&kernel.radius);
	clSetKernelArg(kernelt, 4, sizeof(cl_mem), (void *)&width_mem);
	clSetKernelArg(kernelt, 5, sizeof(cl_mem), (void *)&height_mem);

	size_t globalws[2] = {width, height};

	// execute kernel
    clEnqueueNDRangeKernel(cmd_queue, kernelt, 2, 0, globalws, 0, 0, 0, 0);

    // copy results from device back to host
    clEnqueueReadBuffer(cmd_queue, out_img_mem, CL_TRUE, 0, size, cl_output, 0, NULL, NULL); // CL_TRUE: blocking read back

    // free opencl object
    clReleaseMemObject(in_img_mem);
    clReleaseMemObject(out_img_mem);
    clReleaseMemObject(kernel_mem);
    clReleaseMemObject(width_mem);
    clReleaseMemObject(height_mem);
    clReleaseCommandQueue(cmd_queue);
    clReleaseContext(ctx);
}

void Image::image_write()
{
	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			Vec3b &color = img.at<Vec3b>(i,j);//BGR
			color[0] = pixels[i][j].B;
			color[1] = pixels[i][j].G;
			color[2] = pixels[i][j].R;
		}	
	}
    
	imwrite(output_file, img);

}
	



