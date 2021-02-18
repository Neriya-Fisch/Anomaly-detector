// 315558692	312502602

#ifndef MINCIRCLE_H_
#define MINCIRCLE_H_

#include "anomaly_detection_util.h"
#include <iostream>
#include <vector>
#include <stdlib.h> /* srand, rand */
#include <time.h>	/* time */
#include <math.h>

using namespace std;

class Circle
{
public:
	Point center;
	float radius;
	Circle(Point c, float r) : center(c), radius(r) {}
};

float dist(Point a, Point b);

Circle from2points(Point a, Point b);
Circle from3Points(Point a, Point b, Point c);

Circle trivial(vector<Point> &P);
Circle welzl(Point **P, vector<Point> R, size_t n);
Circle findMinCircle(Point **points, size_t size);

#endif /* MINCIRCLE_H_ */
