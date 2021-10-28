/*
 * anomaly_detection_util.cpp
 *
 * Author: Noam Basha 208847228
 */

#include <math.h>
#include "anomaly_detection_util.h"

// returns the average of X
float avg(float *x, int size) {
    // x = [a0, a1, a2, ... , an-1], size = n.
    float sum = 0;
    for (int i = 0; i < size; i++) {
        sum += x[i];
    }
    return sum / (float)size;
}

// returns the variance of X and Y
float var(float *x, int size) {
    float xSquared[size];
    // Initializing xSquared; xSquared[i] = (x[i])^2.
    for (int i = 0; i < size; i++) {
        xSquared[i] = pow(x[i], 2);
    }
    // var(x) = avg(xSquared) - miu^2.
    return (float)(avg(xSquared, size) - pow(avg(x, size), 2));
}

// returns the covariance of X and Y
float cov(float *x, float *y, int size) {
    // Initializing xy; xy[i] = x[i]*y[i];
    float xy[size];
    for (int i = 0; i < size; i++) {
        xy[i] = x[i] * y[i];
    }
    // cov(x,y) = avg(xy)-avg(x)*avg(y).
    return avg(xy, size) - avg(x, size) * avg(y, size);
}


// returns the Pearson correlation coefficient of X and Y
float pearson(float *x, float *y, int size) {
    // pearson = cov(x,y) / (sqrt(var(x)) * sqrt(var(y))).
    return cov(x, y, size) / (sqrt(var(x, size)) * sqrt(var(y, size)));
}

// performs a linear regression and returns the line equation
Line linear_reg(Point **points, int size) {
    float x[size], y[size];
    // Extracting all x's and all y's to separated arrays.
    for (int i = 0; i < size; i++) {
        x[i] = points[i]->x;
        y[i] = points[i]->y;
    }
    // a = cov(x,y) / var(x).
    float a = cov(x, y, size) / var(x, size);
    // b = avg(y) - a * avg(x).
    return Line(a, avg(y, size) - a * avg(x, size));
}

// returns the deviation between point p and the line equation of the points
float dev(Point p, Point **points, int size) {
    return dev(p, linear_reg(points, size));
}

// returns the deviation between point p and the line
float dev(Point p, Line l) {
    // Returning the distance from p to line by |f(x)-y|.
    return fabs(l.f(p.x) - p.y);
}




