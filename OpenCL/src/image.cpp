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
	char dname[500];
	cl_int err_platform, err_device, err_context, err_cmdq;
	err_platform = clGetPlatformIDs(1, &pid, &num_platform);
	err_device = clGetDeviceIDs(pid, CL_DEVICE_TYPE_GPU, 1, &did, &num_device);
	clGetDeviceInfo(did,CL_DEVICE_NAME,500,dname,NULL);
	// cout<<dname<<endl;
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

	Pixel *output = new Pixel[width * height];
	double ** matrix;

    int size = width * height * sizeof(Pixel); // image size byte
	int kernel_size = kernel.dim * kernel.dim * sizeof(double);

    // opencl memory allocation
    cl_int err_mem;
    cl_mem in_img_mem = clCreateBuffer(ctx, CL_MEM_READ_ONLY, size, NULL, &err_mem);
    cl_mem out_img_mem = clCreateBuffer(ctx, CL_MEM_WRITE_ONLY, size, NULL, &err_mem);
	cl_mem kernel_mem = clCreateBuffer(ctx, CL_MEM_READ_ONLY, kernel_size, NULL, &err_mem);


    // transfer input data to the device

	err_cmdq = clEnqueueWriteBuffer(cmd_queue, kernel_mem, CL_TRUE, 0, kernel_size, kernel.matrix, 0, NULL, NULL);
	cout<<err_cmdq<<endl;
	err_cmdq = clEnqueueWriteBuffer(cmd_queue, in_img_mem, CL_TRUE, 0, size, pixels, 0, NULL, NULL);

	// create the OpenCL kernel
	// Load cl program from file to string
	cl_program program_cl,program_head,program;
	cl_int err_program, err_build, err_kernel, err_args;
	std::ifstream srcFile("kernel.cl");
  	string srcProg(istreambuf_iterator<char>(srcFile),(istreambuf_iterator<char>()));
  	const char *src=srcProg.c_str();
  	size_t src_length=srcProg.length();
  	program_cl=clCreateProgramWithSource(ctx,1,&src,&src_length,&err_program);

	std::ifstream srcFile_head("image.hpp");
	string srcProg_head(istreambuf_iterator<char>(srcFile_head),(istreambuf_iterator<char>()));
    const char *src_head=srcProg_head.c_str();
    size_t src_length_head=srcProg_head.length();
    program_head=clCreateProgramWithSource(ctx,1,&src_head,&src_length_head,&err_program);
	cout<<src_head<<endl;

	const char *input_head_names[1]={"image.hpp"};
	cl_program input_head[1]={program_head};
	err_build=clCompileProgram(program_cl,0,NULL,0,1,input_head,input_head_names,NULL,NULL);
	cout<<err_build<<endl;
	
	if (err_build == CL_BUILD_PROGRAM_FAILURE)
	{
		// Determine the size of the log
		size_t log_size;
		clGetProgramBuildInfo(program_cl, did, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

		// Allocate memory for the log
		char *log = (char *) malloc(log_size);

		// Get the log
		clGetProgramBuildInfo(program_cl, did, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);

		// Print the log
		cout<<log<<endl;
	}
	program=clLinkProgram(ctx,num_device,&did,NULL,1,&program_cl,NULL,NULL,&err_build);
	cout<<err_build<<endl;

    cl_kernel kernelt = clCreateKernel(program, "CL_GaussianFliter", &err_kernel);
	cout<<err_kernel<<endl;

    // set arguments of the kernel
    clSetKernelArg(kernelt, 0, sizeof(cl_mem), (void *)&in_img_mem);
	clSetKernelArg(kernelt, 1, sizeof(cl_mem), (void *)&out_img_mem);
	clSetKernelArg(kernelt, 2, sizeof(cl_mem), (void *)&kernel_mem);
	clSetKernelArg(kernelt, 3, sizeof(int), (void *)&kernel.radius);
	clSetKernelArg(kernelt, 4, sizeof(int), (void *)&width);
	clSetKernelArg(kernelt, 5, sizeof(int), (void *)&height);

	size_t globalws[2] = {width, height};


	// execute kernel
    clEnqueueNDRangeKernel(cmd_queue, kernelt, 2, 0, globalws, 0, 0, 0, 0);

    // copy results from device back to host
    clEnqueueReadBuffer(cmd_queue, out_img_mem, CL_TRUE, 0, size, output, 0, NULL, NULL); // CL_TRUE: blocking read back

    // free opencl object
    clReleaseMemObject(in_img_mem);
    clReleaseMemObject(out_img_mem);
    clReleaseMemObject(kernel_mem);
    clReleaseCommandQueue(cmd_queue);
    clReleaseContext(ctx);
	system("pause");
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
	



