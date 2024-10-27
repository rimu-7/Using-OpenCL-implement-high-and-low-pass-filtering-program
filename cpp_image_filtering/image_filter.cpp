#include <CL/cl.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>

void readKernel(const char* filename, std::string& kernelCode) {
    std::ifstream kernelFile(filename);
    if (!kernelFile.is_open()) {
        std::cerr << "Failed to open kernel file." << std::endl;
        exit(1);
    }
    kernelCode.assign((std::istreambuf_iterator<char>(kernelFile)), std::istreambuf_iterator<char>());
}

void processImage(const char* inputFilePath, const char* outputLowPass, const char* outputHighPass) {
    cv::Mat img = cv::imread(inputFilePath, cv::IMREAD_GRAYSCALE);
    int width = img.cols;
    int height = img.rows;

    // OpenCL setup
    cl_platform_id platform;
    clGetPlatformIDs(1, &platform, nullptr);

    cl_device_id device;
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, nullptr);

    cl_context context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, nullptr);
    cl_command_queue queue = clCreateCommandQueue(context, device, 0, nullptr);

    // Create buffers
    cl_mem d_input = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, width * height, img.data, nullptr);
    cl_mem d_outputLow = clCreateBuffer(context, CL_MEM_WRITE_ONLY, width * height, nullptr, nullptr);
    cl_mem d_outputHigh = clCreateBuffer(context, CL_MEM_WRITE_ONLY, width * height, nullptr, nullptr);

    // Read and compile kernels
    std::string kernelCode;
    readKernel("filter.cl", kernelCode);
    const char* source = kernelCode.c_str();

    cl_program program = clCreateProgramWithSource(context, 1, &source, nullptr, nullptr);
    clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);

    // Execute low-pass filter
    cl_kernel kernelLow = clCreateKernel(program, "low_pass_filter", nullptr);
    clSetKernelArg(kernelLow, 0, sizeof(cl_mem), &d_input);
    clSetKernelArg(kernelLow, 1, sizeof(cl_mem), &d_outputLow);
    clSetKernelArg(kernelLow, 2, sizeof(int), &width);
    clSetKernelArg(kernelLow, 3, sizeof(int), &height);

    size_t globalWorkSize[] = { (size_t)width, (size_t)height };
    clEnqueueNDRangeKernel(queue, kernelLow, 2, nullptr, globalWorkSize, nullptr, 0, nullptr, nullptr);

    // Execute high-pass filter
    cl_kernel kernelHigh = clCreateKernel(program, "high_pass_filter", nullptr);
    clSetKernelArg(kernelHigh, 0, sizeof(cl_mem), &d_input);
    clSetKernelArg(kernelHigh, 1, sizeof(cl_mem), &d_outputHigh);
    clSetKernelArg(kernelHigh, 2, sizeof(int), &width);
    clSetKernelArg(kernelHigh, 3, sizeof(int), &height);

    clEnqueueNDRangeKernel(queue, kernelHigh, 2, nullptr, globalWorkSize, nullptr, 0, nullptr, nullptr);

    // Read back results
    unsigned char* h_outputLow = new unsigned char[width * height];
    unsigned char* h_outputHigh = new unsigned char[width * height];

    clEnqueueReadBuffer(queue, d_outputLow, CL_TRUE, 0, width * height, h_outputLow, 0, nullptr, nullptr);
    clEnqueueReadBuffer(queue, d_outputHigh, CL_TRUE, 0, width * height, h_outputHigh, 0, nullptr, nullptr);

    // Save results
    cv::Mat lowPassImg(height, width, CV_8UC1, h_outputLow);
    cv::Mat highPassImg(height, width, CV_8UC1, h_outputHigh);

    cv::imwrite(outputLowPass, lowPassImg);
    cv::imwrite(outputHighPass, highPassImg);

    // Cleanup
    delete[] h_outputLow;
    delete[] h_outputHigh;
    clReleaseMemObject(d_input);
    clReleaseMemObject(d_outputLow);
    clReleaseMemObject(d_outputHigh);
    clReleaseKernel(kernelLow);
    clReleaseKernel(kernelHigh);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
}

int main(int argc, char** argv) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <input image> <output low pass> <output high pass>" << std::endl;
        return -1;
    }

    processImage(argv[1], argv[2], argv[3]);
    return 0;
}
