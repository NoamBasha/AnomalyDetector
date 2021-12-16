
#include "HybridAnomalyDetector.h"

HybridAnomalyDetector::HybridAnomalyDetector() {
    // TODO Auto-generated constructor stub

}

HybridAnomalyDetector::~HybridAnomalyDetector() {
    // TODO Auto-generated destructor stub
}

void HybridAnomalyDetector::learnNormalHelper(const TimeSeries &ts, float maxPearson, string feature1Str,
                                              string feature2Str) {
    if (maxPearson > 0.9) {
        SimpleAnomalyDetector::learnNormalHelper(ts, maxPearson, feature1Str, feature2Str);
    } else if (maxPearson > 0.5) {
        map<string, vector<float>> tsMap = ts.getMap();
        vector<string> tsFeaturesVector = ts.getFeaturesVector();
        int valueVectorSize = tsMap[tsFeaturesVector[0]].size();
        Point *points[valueVectorSize];
        for (int k = 0; k < valueVectorSize; ++k) {
            points[k] = new Point(tsMap[feature1Str][k], tsMap[feature2Str][k]);
        }
        Circle minCircle = findMinCircle(points, valueVectorSize);
        correlatedFeatures currentCF;
        currentCF.feature1 = feature1Str;
        currentCF.feature2 = feature2Str;
        currentCF.corrlation = maxPearson;
        currentCF.lin_reg = linear_reg(points, valueVectorSize);
        currentCF.center = minCircle.center;
        currentCF.threshold = minCircle.radius * 1.1;
        cf.push_back(currentCF);
        for (int j = 0; j < valueVectorSize; ++j) {
            delete points[j];
        }
    }
}

bool HybridAnomalyDetector::isAnomaly(Point p, correlatedFeatures c) {
    if (c.corrlation >= 0.9) {
        return SimpleAnomalyDetector::isAnomaly(p, c);
    } else if (c.corrlation >= 0.5) {
        return (distanceBetweenPoints(p, c.center) > c.threshold);
    }
}
