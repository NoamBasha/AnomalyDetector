#include "SimpleAnomalyDetector.h"

using namespace std;

SimpleAnomalyDetector::SimpleAnomalyDetector() {
    threshold = 0.9;
}

SimpleAnomalyDetector::~SimpleAnomalyDetector() {
    // TODO Auto-generated destructor stub
}

void SimpleAnomalyDetector::learnNormal(const TimeSeries &ts) {
    map<string, vector<float>> tsMap = ts.getMap();
    vector<string> tsFeaturesVector = ts.getFeaturesVector();
    int featuresVectorSize = tsFeaturesVector.size();
    int valueVectorSize = tsMap[tsFeaturesVector[0]].size();
    string feature1Str;
    string feature2Str;
    for (int i = 0; i < featuresVectorSize; ++i) {
        string fiName = tsFeaturesVector[i];
        auto fiData = tsMap[fiName].data();
        float maxPearson = 0;
        feature1Str = fiName;
        for (int j = i + 1; j < featuresVectorSize; ++j) {
            string fjName = tsFeaturesVector[j];
            auto fjData = tsMap[fjName].data();
            float currentPearson = fabs(pearson(fiData, fjData, valueVectorSize));
            if (currentPearson > maxPearson) {
                maxPearson = currentPearson;
                feature2Str = fjName;
            }
        }

        learnNormalHelper(ts, maxPearson, feature1Str, feature2Str);
    }
}

vector<AnomalyReport> SimpleAnomalyDetector::detect(const TimeSeries &ts) {
    map<string, vector<float>> tsMap = ts.getMap();
    vector<string> tsFeaturesVector = ts.getFeaturesVector();
    int valueVectorSize = tsMap[tsFeaturesVector[0]].size();
    vector<AnomalyReport> anomalyReportsVector;
    Point *points[valueVectorSize];
    for (int i = 0; i < cf.size(); ++i) {
        correlatedFeatures currentCF = cf[i];
        for (int j = 0; j < valueVectorSize; ++j) {
            points[j] = new Point(tsMap[currentCF.feature1][j], tsMap[currentCF.feature2][j]);
        }
        for (int j = 0; j < valueVectorSize; ++j) {
            if (isAnomaly(*points[j], currentCF)) {
                string anomalyDescription = currentCF.feature1 + "-" + currentCF.feature2;
                AnomalyReport *anomalyReport = new AnomalyReport(anomalyDescription, j + 1);
                anomalyReportsVector.push_back(*anomalyReport);
                delete anomalyReport;
            }
            delete points[j];
        }
    }
    return anomalyReportsVector;
}

void SimpleAnomalyDetector::learnNormalHelper(const TimeSeries &ts, float maxPearson, string feature1Str,
                                              string feature2Str) {

    map<string, vector<float>> tsMap = ts.getMap();
    vector<string> tsFeaturesVector = ts.getFeaturesVector();
    int valueVectorSize = tsMap[tsFeaturesVector[0]].size();

    correlatedFeatures currentCF;

    if (maxPearson > threshold) {
        currentCF.corrlation = maxPearson;
        Point *points[valueVectorSize];
        for (int j = 0; j < valueVectorSize; ++j) {
            points[j] = new Point(tsMap[feature1Str][j], tsMap[feature2Str][j]);
        }
        currentCF.feature1 = feature1Str;
        currentCF.feature2 = feature2Str;
        currentCF.lin_reg = linear_reg(points, valueVectorSize);
        currentCF.threshold = 0;
        for (int j = 0; j < valueVectorSize; ++j) {
            currentCF.threshold = max(currentCF.threshold, fabs(dev(*points[j], currentCF.lin_reg)));
            delete points[j];
        }
        currentCF.threshold *= 1.1;
        cf.push_back(currentCF);
    }
}

bool SimpleAnomalyDetector::isAnomaly(Point p, correlatedFeatures c) {
    return fabs(dev(p, c.lin_reg)) > c.threshold;
}
