#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <time.h>
#include <CL/cl.h>

#include "display_image.h"

#define MAX_SOURCE_SIZE (10000)
#define TOTAL_OBJECT (3200)
#define TOTALFRAME (999999)

void assert(int value, int compare_value, const char message[])
{
    if (value != compare_value)
    {
        printf(message);
        printf("Error: %d\n", value);
        exit(1);
    }
}

int main(void) {
    int i;
    // Create the input values
    double* posX = (double*)malloc(sizeof(double) * TOTAL_OBJECT);
    double* posY = (double*)malloc(sizeof(double) * TOTAL_OBJECT);
    double* posZ = (double*)malloc(sizeof(double) * TOTAL_OBJECT);
    double* velX = (double*)malloc(sizeof(double) * TOTAL_OBJECT);
    double* velY = (double*)malloc(sizeof(double) * TOTAL_OBJECT);
    double* velZ = (double*)malloc(sizeof(double) * TOTAL_OBJECT);
    double* mass = (double*)malloc(sizeof(double) * TOTAL_OBJECT);
    bool*   alive = (bool*)malloc(sizeof(bool) * TOTAL_OBJECT);
    double* temperature = (double*)malloc(sizeof(double) * TOTAL_OBJECT);
    double* radius = (double*)malloc(sizeof(double) * TOTAL_OBJECT);

    // Init with random values
    srand((unsigned int)time(0));
    for (i = 0; i < TOTAL_OBJECT; i++)
    {
        posX[i] = (float)(rand()- 16384) / 1.0;
        posY[i] = (float)(rand()- 16384) / 1.0;
        posZ[i] = (float)(rand()- 16384) / 1.0;
        velX[i] = (float)(rand() - 16384) / 30.0;
        velY[i] = (float)(rand() - 16384) / 30.0;
        velZ[i] = (float)(rand() - 16384) / 30.0;
        mass[i] = (float)rand() * 1000.0;
        alive[i] = true;
        temperature[i] = (double)rand() / 32767.0;
        radius[i] = (double)rand() / 3276.7;
    }

    // A custom size for a single object
    //mass[0] = (float)rand() * 270000.0;
    //velX[0] = 0.0;
    //velY[0] = 0.0;
    //velZ[0] = 0.0;

    // Load the kernel source code into the array source_str
    FILE* fp;
    char* source_str;
    size_t source_size;

    fopen_s(&fp, "E:/Work/OpenCL/Learn/HelloWorld/SpaceSimulation/simkernel.cl.c", "r");
    if (!fp) {
        printf("Failed to load kernel.\n");
        exit(1);
    }
    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose(fp);

    // Get platform and device information
    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);
    assert(ret, CL_SUCCESS, "Cannot get device ID\n");

    // Create an OpenCL context
    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

    // Create a command queue
    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

    // Create memory buffers on the device for each vector input, output value
    cl_mem dposX    = clCreateBuffer(context, CL_MEM_READ_WRITE, TOTAL_OBJECT * sizeof(double), NULL, &ret);
    cl_mem dposY    = clCreateBuffer(context, CL_MEM_READ_WRITE, TOTAL_OBJECT * sizeof(double), NULL, &ret);
    cl_mem dposZ    = clCreateBuffer(context, CL_MEM_READ_WRITE, TOTAL_OBJECT * sizeof(double), NULL, &ret);
    cl_mem dvelX    = clCreateBuffer(context, CL_MEM_READ_WRITE, TOTAL_OBJECT * sizeof(double), NULL, &ret);
    cl_mem dvelY    = clCreateBuffer(context, CL_MEM_READ_WRITE, TOTAL_OBJECT * sizeof(double), NULL, &ret);
    cl_mem dvelZ    = clCreateBuffer(context, CL_MEM_READ_WRITE, TOTAL_OBJECT * sizeof(double), NULL, &ret);
    cl_mem dmassin  = clCreateBuffer(context, CL_MEM_READ_WRITE, TOTAL_OBJECT * sizeof(double), NULL, &ret);
    cl_mem dmassout = clCreateBuffer(context, CL_MEM_READ_WRITE, TOTAL_OBJECT * sizeof(double), NULL, &ret);
    cl_mem dposXout = clCreateBuffer(context, CL_MEM_READ_WRITE, TOTAL_OBJECT * sizeof(double), NULL, &ret);
    cl_mem dposYout = clCreateBuffer(context, CL_MEM_READ_WRITE, TOTAL_OBJECT * sizeof(double), NULL, &ret);
    cl_mem dposZout = clCreateBuffer(context, CL_MEM_READ_WRITE, TOTAL_OBJECT * sizeof(double), NULL, &ret);
    cl_mem dvelXout = clCreateBuffer(context, CL_MEM_READ_WRITE, TOTAL_OBJECT * sizeof(double), NULL, &ret);
    cl_mem dvelYout = clCreateBuffer(context, CL_MEM_READ_WRITE, TOTAL_OBJECT * sizeof(double), NULL, &ret);
    cl_mem dvelZout = clCreateBuffer(context, CL_MEM_READ_WRITE, TOTAL_OBJECT * sizeof(double), NULL, &ret);
    cl_mem dalive   = clCreateBuffer(context, CL_MEM_READ_WRITE, TOTAL_OBJECT * sizeof(bool), NULL, &ret);
    cl_mem dtemp    = clCreateBuffer(context, CL_MEM_READ_WRITE, TOTAL_OBJECT * sizeof(double), NULL, &ret);

    // Create a program from the kernel source
    cl_program program = clCreateProgramWithSource(context, 1, (const char**)&source_str, (const size_t*)&source_size, &ret);

    // Build the program
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    if (ret == CL_BUILD_PROGRAM_FAILURE) {
        // Determine the size of the log
        size_t log_size;
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

        // Allocate memory for the log
        char* log = (char*)malloc(log_size);

        // Get the log
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);

        // Print the log
        printf("%s\n", log);
        free(log);
    }
    assert(ret, CL_SUCCESS, "Failed to build kernel program\n");

    // Create the OpenCL kernel
    cl_kernel kernel = clCreateKernel(program, "calc_sim", &ret);

    // Set the arguments of the kernel program
    ret  = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&dposX);
    ret |= clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&dposY);
    ret |= clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&dposZ);
    ret |= clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*)&dvelX);
    ret |= clSetKernelArg(kernel, 4, sizeof(cl_mem), (void*)&dvelY);
    ret |= clSetKernelArg(kernel, 5, sizeof(cl_mem), (void*)&dvelZ);
    ret |= clSetKernelArg(kernel, 6, sizeof(cl_mem), (void*)&dmassin);
    ret |= clSetKernelArg(kernel, 7, sizeof(cl_mem), (void*)&dmassout);
    ret |= clSetKernelArg(kernel, 8, sizeof(cl_mem), (void*)&dposXout);
    ret |= clSetKernelArg(kernel, 9, sizeof(cl_mem), (void*)&dposYout);
    ret |= clSetKernelArg(kernel, 10, sizeof(cl_mem), (void*)&dposZout);
    ret |= clSetKernelArg(kernel, 11, sizeof(cl_mem), (void*)&dvelXout);
    ret |= clSetKernelArg(kernel, 12, sizeof(cl_mem), (void*)&dvelYout);
    ret |= clSetKernelArg(kernel, 13, sizeof(cl_mem), (void*)&dvelZout);
    ret |= clSetKernelArg(kernel, 14, sizeof(cl_mem), (void*)&dalive);
    ret |= clSetKernelArg(kernel, 15, sizeof(cl_mem), (void*)&dtemp);
    assert(ret, CL_SUCCESS, "Failed to set kernel arg\n");

    // Init display image
    initDisplayer();

    // Main program loop
    for (int frame = 0; frame < TOTALFRAME; frame++)
    {
        // Copy the input lists their respective memory buffers
        ret  = clEnqueueWriteBuffer(command_queue, dposX, CL_TRUE, 0, TOTAL_OBJECT * sizeof(double), posX, 0, NULL, NULL);
        ret |= clEnqueueWriteBuffer(command_queue, dposY, CL_TRUE, 0, TOTAL_OBJECT * sizeof(double), posY, 0, NULL, NULL);
        ret |= clEnqueueWriteBuffer(command_queue, dposZ, CL_TRUE, 0, TOTAL_OBJECT * sizeof(double), posZ, 0, NULL, NULL);
        ret |= clEnqueueWriteBuffer(command_queue, dvelX, CL_TRUE, 0, TOTAL_OBJECT * sizeof(double), velX, 0, NULL, NULL);
        ret |= clEnqueueWriteBuffer(command_queue, dvelY, CL_TRUE, 0, TOTAL_OBJECT * sizeof(double), velY, 0, NULL, NULL);
        ret |= clEnqueueWriteBuffer(command_queue, dvelZ, CL_TRUE, 0, TOTAL_OBJECT * sizeof(double), velZ, 0, NULL, NULL);
        ret |= clEnqueueWriteBuffer(command_queue, dmassin, CL_TRUE, 0, TOTAL_OBJECT * sizeof(double), mass, 0, NULL, NULL);
        ret |= clEnqueueWriteBuffer(command_queue, dmassout, CL_TRUE, 0, TOTAL_OBJECT * sizeof(double), mass, 0, NULL, NULL);
        ret |= clEnqueueWriteBuffer(command_queue, dalive, CL_TRUE, 0, TOTAL_OBJECT * sizeof(bool), alive, 0, NULL, NULL);
        ret |= clEnqueueWriteBuffer(command_queue, dtemp, CL_TRUE, 0, TOTAL_OBJECT * sizeof(double), temperature, 0, NULL, NULL);
        assert(ret, CL_SUCCESS, "Failed to copy input buffer\n");

        // Execute the OpenCL kernel on the list
        size_t global_item_size = TOTAL_OBJECT; // Process the entire lists
        size_t local_item_size = 64; // Divide work items into groups of 64
        printf("Start calculating..... ");
        ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);
        assert(ret, CL_SUCCESS, "Failed to execute kernel program\n");

        // Read the memory buffer on the device back to input buffer on host
        ret  = clEnqueueReadBuffer(command_queue, dposXout, CL_TRUE, 0, TOTAL_OBJECT * sizeof(double), posX, 0, NULL, NULL);
        ret |= clEnqueueReadBuffer(command_queue, dposYout, CL_TRUE, 0, TOTAL_OBJECT * sizeof(double), posY, 0, NULL, NULL);
        ret |= clEnqueueReadBuffer(command_queue, dposZout, CL_TRUE, 0, TOTAL_OBJECT * sizeof(double), posZ, 0, NULL, NULL);
        ret |= clEnqueueReadBuffer(command_queue, dvelXout, CL_TRUE, 0, TOTAL_OBJECT * sizeof(double), velX, 0, NULL, NULL);
        ret |= clEnqueueReadBuffer(command_queue, dvelYout, CL_TRUE, 0, TOTAL_OBJECT * sizeof(double), velY, 0, NULL, NULL);
        ret |= clEnqueueReadBuffer(command_queue, dvelZout, CL_TRUE, 0, TOTAL_OBJECT * sizeof(double), velZ, 0, NULL, NULL);
        ret |= clEnqueueReadBuffer(command_queue, dmassout, CL_TRUE, 0, TOTAL_OBJECT * sizeof(double), mass, 0, NULL, NULL);
        ret |= clEnqueueReadBuffer(command_queue, dalive, CL_TRUE, 0, TOTAL_OBJECT * sizeof(bool), alive, 0, NULL, NULL);
        ret |= clEnqueueReadBuffer(command_queue, dtemp, CL_TRUE, 0, TOTAL_OBJECT * sizeof(double), temperature, 0, NULL, NULL);
        assert(ret, CL_SUCCESS, "Failed to read back output buffer\n");
        printf("done\n");

        // Copy data to image displayer
        writeImage(alive, posX, posY, posZ, mass, temperature, TOTAL_OBJECT);
        showImage(frame);
    }


    // Clean up
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(dposX);
    ret = clReleaseMemObject(dposY);
    ret = clReleaseMemObject(dposZ);
    ret = clReleaseMemObject(dvelX);
    ret = clReleaseMemObject(dvelY);
    ret = clReleaseMemObject(dvelZ);
    ret = clReleaseMemObject(dposXout);
    ret = clReleaseMemObject(dposYout);
    ret = clReleaseMemObject(dposZout);
    ret = clReleaseMemObject(dvelXout);
    ret = clReleaseMemObject(dvelYout);
    ret = clReleaseMemObject(dvelZout);
    ret = clReleaseMemObject(dmassin);
    ret = clReleaseMemObject(dmassout);
    ret = clReleaseMemObject(dalive);
    ret = clReleaseMemObject(dtemp);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);
    if (ret != CL_SUCCESS)
    {
        printf("Failed to release some object\n");
    }
    free(posX);
    free(posY);
    free(posZ);
    free(velX);
    free(velY);
    free(velZ);
    free(mass);
    free(alive);
    free(temperature);
    free(source_str);

    return 0;
}