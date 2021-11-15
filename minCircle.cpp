#include "minCircle.h"

using namespace std;

/*
 * We will implement welzl's algorithm to find the MEC of the given array of points.
 */

float distanceBetweenPoints(const Point &a, const Point &b) {
    return (float)sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

Circle circleFromTwoPoints(const Point &a, const Point &b) {
    auto centerX = (float)((a.x + b.x) / 2.0);
    auto centerY = (float)((a.y + b.y) / 2.0);
    Point center = {centerX, centerY};
    auto radius = (float)((distanceBetweenPoints(a, b)) / 2.0);
    return {center, radius};
}

Circle circleFromThreePoints(const Point &a, const Point &b, const Point &c) {
    // According to a formula from the internet:
    float x1 = b.x - a.x;
    float y1 = b.y - a.y;
    float x2 = c.x - a.x;
    float y2 = c.y - a.y;
    float b1 = x1 * x1 + y1 * y1;
    float c1 = x2 * x2 + y2 * y2;
    float d1 = x1 * y2 - y1 * x2;
    Point i = {(y2 * b1 - y1 * c1) / (2 * d1), (x1 * c1 - x2 * b1) / (2 * d1)};
    i.x += a.x;
    i.y += a.y;
    return {i, distanceBetweenPoints(i, a)};
}

bool isInsideCircle(Point p, Circle c) {
    return distanceBetweenPoints(p, c.center) <= c.radius;
}

/*
 * Checks if all of the points in the points vector are inside the circle.
 */
bool isCircleValid(Circle c, const vector<Point>& points) {
    for (Point p : points)
        if (!isInsideCircle(p, c))
            return false;
    return true;
}

Circle findMinCircleHelper(Point **points, size_t pointsSize, vector<Point> boundaryPoints) {
    // If P is empty (meaning all the points have been processed)
    // or |R| = 3 then we already found three points on the boundary so we can return the trivial circle
    // uniquely determined by them.
    if (pointsSize == 0 || boundaryPoints.size() == 3) {
        if (boundaryPoints.empty()) {
            return {{0, 0}, 0};
        } else if (boundaryPoints.size() == 1) {
            return {boundaryPoints[0], 0};
        } else if (boundaryPoints.size() == 2) {
            return circleFromTwoPoints(boundaryPoints[0], boundaryPoints[1]);
        }
        // Checking if the MEC can be determined by two points only from the boundary points found.
        for (int i = 0; i < 3; i++) {
            for (int j = i + 1; j < 3; j++) {
                Circle c = circleFromTwoPoints(boundaryPoints[i], boundaryPoints[j]);
                if (isCircleValid(c, boundaryPoints))
                    return c;
            }
        }
        return circleFromThreePoints(boundaryPoints[0], boundaryPoints[1], boundaryPoints[2]);
    }
    // 'Removing' the last point from points and Calling the recursive algorithm on points and boundaryPoints
    Point removedPoint = {points[pointsSize - 1]->x, points[pointsSize - 1]->y};
    --pointsSize;
    Circle possibleMEC = findMinCircleHelper(points, pointsSize, boundaryPoints);
    // If the removed point is enclosed by the possible MEC:
    if (isInsideCircle(removedPoint, possibleMEC)) {
        return possibleMEC;
    }
    // the removed point is on the perimeter of the MEC therefore we'll add it to the boundary points vector.
    boundaryPoints.push_back(removedPoint);
    return findMinCircleHelper(points, pointsSize, boundaryPoints);
}

Circle findMinCircle(Point **points, size_t pointsSize) {
    // Initializing a vector of points that will contain the boundary points of the MEC.
    vector<Point> boundaryPoints;
    return findMinCircleHelper(points, pointsSize, boundaryPoints);
}