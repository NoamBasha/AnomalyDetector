
#ifndef MINCIRCLE_H_
#define MINCIRCLE_H_

#include <iostream>
#include <vector>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <math.h>
#include "anomaly_detection_util.h"

using namespace std;


// ------------ DO NOT CHANGE -----------

class Circle{
public:
	Point center;
	float radius;
	Circle(Point c,float r):center(c),radius(r){}
};
// --------------------------------------


float distanceBetweenPoints(const Point &a, const Point &b);

Circle circleFromTwoPoints(const Point &a, const Point &b);

Circle circleFromThreePoints(const Point &a, const Point &b, const Point &c);

bool isInsideCircle(Point p, Circle c);

bool isCircleValid(Circle c, const vector<Point>& points);

Circle findMinCircleHelper(Point **points, size_t pointsSize, vector<Point> boundaryPoints);

Circle findMinCircle(Point **points, size_t pointsSize);


#endif /* MINCIRCLE_H_ */
