#include "SimpleAnomalyDetector.h"

using namespace std;

SimpleAnomalyDetector::SimpleAnomalyDetector() {
    // TODO Auto-generated constructor stub

}

SimpleAnomalyDetector::~SimpleAnomalyDetector() {
    // TODO Auto-generated destructor stub
}

void SimpleAnomalyDetector::learnNormal(const TimeSeries &ts) {
    map<string, vector<float>> tsMap = ts.getMap();
    vector<string> tsFeaturesVector = ts.getFeaturesVector();
    int featuresVectorSize = tsFeaturesVector.size();
    int valueVectorSize = tsMap[tsFeaturesVector[0]].size();
    for (int i = 0; i < featuresVectorSize; ++i) {
        string fiName = tsFeaturesVector[i];
        auto fiData = tsMap[fiName].data();
        float maxPearson = 0;
        correlatedFeatures currentCF;
        currentCF.feature1 = fiName;
        for (int j = i + 1; j < featuresVectorSize; ++j) {
            string fjName = tsFeaturesVector[j];
            auto fjData = tsMap[fjName].data();
            float currentPearson = fabs(pearson(fiData, fjData, valueVectorSize));
            if (currentPearson > maxPearson) {
                maxPearson = currentPearson;
                currentCF.feature2 = fjName;
            }
        }
        if (maxPearson > 0.9) {
            currentCF.corrlation = maxPearson;
            Point *points[valueVectorSize];
            for (int j = 0; j < valueVectorSize; ++j) {
                points[j] = new Point(tsMap[currentCF.feature1][j], tsMap[currentCF.feature2][j]);
            }
            currentCF.lin_reg = linear_reg(points, valueVectorSize);
            currentCF.threshold = 0;
            for (int j = 0; j < valueVectorSize; ++j) {
                currentCF.threshold = max(currentCF.threshold,
                                          fabs(dev(*points[j], currentCF.lin_reg)));
                delete points[j];
            }
            currentCF.threshold *= 1.1;
            cf.push_back(currentCF);
        }
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
            if (fabs(dev(*points[j], currentCF.lin_reg)) > currentCF.threshold) {
                string anomalyDescription = currentCF.feature1 + "-" + currentCF.feature2;
                AnomalyReport *anomalyReport = new AnomalyReport(anomalyDescription, j+1);
                anomalyReportsVector.push_back(*anomalyReport);
                delete anomalyReport;
            }
            delete points[j];
        }
    }
    return anomalyReportsVector;
}