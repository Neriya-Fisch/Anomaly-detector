/*
 * animaly_detection_util.cpp
 *
 * Author: 315558692 Neriya Fisch
 */

#include <math.h>
#include "anomaly_detection_util.h"

float avg(float *x, int size)
{
	float sum = 0;
	for (int i = 0; i < size; i++)
	{
		sum = sum + x[i];
	}
	float avg = sum / size;
	return avg;
}

// returns the variance of X and Y

float var(float *x, int size)
{
	float sum1 = 0, sum2 = 0;
	for (int i = 0; i < size; i++)
	{
		sum1 = sum1 + (x[i] * x[i]);
	}
	float u = avg(x, size);
	sum1 = sum1 / size;
	return sum1 - (u * u);
}

// returns the covariance of X and Y
float cov(float *x, float *y, int size)
{
	float average1 = avg(x, size);
	float average2 = avg(y, size);
	float sum = 0;
	for (int i = 0; i < size; i++)
	{
		sum = sum + ((x[i] - average1) * (y[i] - average2));
	}
	return sum / size;
}

// returns the Pearson correlation coefficient of X and Y
float pearson(float *x, float *y, int size)
{
	float cov1 = cov(x, y, size);
	float varX = var(x, size);
	float varY = var(y, size);
	return cov1 / sqrt(varX * varY);
}

// performs a linear regression and returns the line equation
Line linear_reg(Point **points, int size)
{
	float arrX[size];
	float arrY[size];
	for (int i = 0; i < size; i++)
	{
		arrX[i] = points[i]->x;
		arrY[i] = points[i]->y;
	}
	float a = cov(arrX, arrY, size) / var(arrX, size);
	float avgY = avg(arrY, size);
	float avgX = avg(arrX, size);
	float b = avgY - avgX * a;

	return Line(a, b);
}

// returns the deviation between point p and the line equation of the points
float dev(Point p, Point **points, int size)
{
	Line line = linear_reg(points, size);
	if (line.f(p.x) - p.y > 0)
	{
		return line.f(p.x) - p.y;
	}
	else
	{
		return p.y - line.f(p.x);
	}
}

// returns the deviation between point p and the line
float dev(Point p, Line l)
{
	if (l.f(p.x) - p.y > 0)
	{
		return l.f(p.x) - p.y;
	}
	else
	{
		return p.y - l.f(p.x);
	}
}