#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video/tracking.hpp>
#include <iostream>

#define CAPTURE_INTERVAL_TIME 50 // Millisecond

using namespace std;
using namespace cv;

uint32_t image_width = 960;
uint32_t image_height = 768;

/* Usage:
    main.exe <video_file> */

const float focal_length = 300;

struct
{
    float x;
    float y;
    float z;
} translation_vector = {.x = 0.0, .y = 0.0, .z = 3.0};

struct
{
    int32_t x;
    int32_t y;
} principal_point = {.x = image_width / 2, .y = image_height / 2};

void flow2chToYUV(const Mat src, Mat &dst)
{
    if (src.type() != CV_32FC2)
    {
        printf("Invalid flow source %d with %d\n", src.type(), CV_32FC2);
    }

    for (int y = 0; y < src.size().height; y++)
    {
        for (int x = 0; x < src.size().width; x++)
        {
            float uf = src.at<Vec2f>(y, x)[0];
            float vf = src.at<Vec2f>(y, x)[1];
            uint8_t u = (uint8_t)round(uf * 127.0 + 128.0);
            uint8_t v = (uint8_t)round(vf * 127.0 + 128.0);
            float lengthf = sqrt(uf * uf + vf * vf) / 1.41 * 255;
            uint8_t length = lengthf > 255 ? 255 : (uint8_t)round(lengthf);
            dst.at<Vec3b>(y, x) = Vec3b(length, u, v);
        }
    }
}

void flow2chToStruct(const Mat flowField, Mat &dst)
{
    if (flowField.type() != CV_32FC2)
    {
        printf("Invalid flow source %d with %d\n", flowField.type(), CV_32FC2);
    }
    if (dst.type() != CV_32FC3)
    {
        printf("Invalid dst type %d with %d\n", flowField.type(), CV_32FC3);
    }

    // Calculate depth
    for (int pixY = 0; pixY < flowField.rows; pixY++)
    {
        for (int pixX = 0; pixX < flowField.cols; pixX++)
        {
            float x = 0;
            float y = 0;
            float z = 0;
            uint32_t channelNum = 0;
            float flowU = flowField.at<Vec2f>(pixY, pixX)[0];
            float flowV = flowField.at<Vec2f>(pixY, pixX)[1];
            if (flowU != 0)
            {
                channelNum += 1;
                z += (translation_vector.x * focal_length - translation_vector.z * (principal_point.x - (pixX + flowU))) / flowU;
            }
            if (flowV != 0)
            {
                channelNum += 1;
                z += (translation_vector.y * focal_length - translation_vector.z * (principal_point.y - (pixY + flowV))) / flowV;
            }
            if(channelNum > 0)
            {
                z = z / channelNum;
                z = z > 4096 ? 4096 : z;
                z = z < 0 ? 4096 : z;
            }
            // x = (pixX - principal_point.x) * z / focal_length;
            // y = (pixY - principal_point.y) * z / focal_length;
            x = pixX;
            y = pixY;

            dst.at<Vec3f>(pixY, pixX) = {x, y, z};
        }
    }
}

void combineStructColor(Mat &dst, const Mat structure, const Mat color)
{
    if(dst.depth() != CV_32F || dst.channels() != 6 || structure.type() != CV_32FC3 || color.type() != CV_8UC3)
    {
        printf("Input type is not correctn");
    }
    for(int x = 0; x < dst.cols; x++)
    {
        for(int y = 0; y < dst.rows; y++)
        {
            dst.at<Vec6f>(y, x) = Vec6f(
                structure.at<Vec3f>(y, x)[0],
                structure.at<Vec3f>(y, x)[1],
                structure.at<Vec3f>(y, x)[2],
                (float)color.at<Vec3b>(y, x)[2] / (float) 255.0,
                (float)color.at<Vec3b>(y, x)[1] / (float) 255.0,
                (float)color.at<Vec3b>(y, x)[0] / (float) 255.0
            );
        }
    }
}

// Read two sequence images and output depth map
// argv[1]: prev image
// argv[2]: cur image
int main(int argc, char **argv)
{
    if(argc != 3)
    {
        printf("Invalid arguments\n");
        return 1;
    }

    Mat prev_image = imread(argv[1], IMREAD_GRAYSCALE);
    Mat cur_image = imread(argv[2], IMREAD_GRAYSCALE);
    
    return 0;
}