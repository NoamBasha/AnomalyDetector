

#ifndef SIMPLEANOMALYDETECTOR_H_
#define SIMPLEANOMALYDETECTOR_H_

#include "anomaly_detection_util.h"
#include "AnomalyDetector.h"
#include <vector>
#include <algorithm>
#include <string.h>
#include <math.h>

struct correlatedFeatures {
    string feature1, feature2;  // names of the correlated features
    float corrlation;
    Line lin_reg;
    float threshold;
    Point center;
};


class SimpleAnomalyDetector : public TimeSeriesAnomalyDetector {
public:
    SimpleAnomalyDetector();

    virtual ~SimpleAnomalyDetector();

    virtual void learnNormal(const TimeSeries &ts);

    virtual vector<AnomalyReport> detect(const TimeSeries &ts);

    vector<correlatedFeatures> getNormalModel() {
        return cf;
    }

    virtual void learnNormalHelper(const TimeSeries &ts, float maxPearson, string feature1Str, string feature2Str);

    virtual bool isAnomaly(Point p, correlatedFeatures c);

protected:
    vector<correlatedFeatures> cf;

};


#endif /* SIMPLEANOMALYDETECTOR_H_ */
