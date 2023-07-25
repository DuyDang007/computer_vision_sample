#include<iostream>
#include<opencv2/core.hpp>
#include<opencv2/imgcodecs.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/highgui.hpp>

#include "display_image.h"

using namespace std;
using namespace cv;

Mat image, blurredImage;
int verticalShift = 0, horizontalShift = 0;

void initDisplayer()
{
	image = Mat::zeros(720, 1280, CV_8UC3);
	namedWindow("Image", WINDOW_AUTOSIZE);
	return;
}

// Map value between [posMin ; posMax] to [0 ; pixMax]
unsigned int mapPixel(const double value, const double posMin, const double posMax, const unsigned int pixMin, const unsigned int pixMax)
{
	double result;
	result = (value - posMin) / (posMax - posMin) * (1000) + pixMin;
	if (result > pixMax) result = pixMax;
	else if (result < 0.0) result = 0.0;
	return (unsigned int) result;
}

void writeImage(const bool* isAlive, const double* posX, const double* posY, const double* posZ, const double* mass, const double* temperature, const size_t objNum)
{
	image = Scalar(0, 0, 0);
	unsigned int x, y;
	for (int i = 0; i < objNum; i++)
	{
		if (i == 0)
		{
			printf("%f  %f  %f  %f \n", mass[0], mass[1], mass[2], mass[3]);
		}
		if (mass[i] == 0.0 || isAlive[i] == 0) continue;
		x = mapPixel(posX[i], -20000.0 + horizontalShift, 25000.0 + horizontalShift,  0, 1280);
		y = mapPixel(posY[i], -20000.0 + verticalShift, 25000.0 + verticalShift, 0, 720);

		// Calculate obj radius on image, just a silly value that works :D
		int rad = int(mass[i] / 30000000);
		if (rad > 5) rad = 5;

		// Obj temperature
		unsigned int tmp = temperature[i];
		if (tmp > 255) tmp = 255;

		circle(image, Point(x, y), rad, Scalar(tmp/1.5, tmp, 255-20), FILLED, 0);

	}
	return;
}

void showImage(int i)
{
	imshow("Image", image);
	int keyCode = waitKeyEx(1);
	switch (keyCode)
	{
		case 2490368: // Up
			verticalShift -= 1000;
			break;
		case 2621440: // down
			verticalShift += 1000;
			break;
		case 2424832: // Left
			horizontalShift -= 1000;
			break;
		case 2555904: // Right
			horizontalShift += 1000;
			break;
		default:
			break;
	}

	return;
}

