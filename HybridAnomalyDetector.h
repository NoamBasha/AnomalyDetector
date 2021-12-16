

#ifndef HYBRIDANOMALYDETECTOR_H_
#define HYBRIDANOMALYDETECTOR_H_

#include "SimpleAnomalyDetector.h"
#include "minCircle.h"

class HybridAnomalyDetector : public SimpleAnomalyDetector {
public:
    HybridAnomalyDetector();

    virtual ~HybridAnomalyDetector();

    virtual void
    learnNormalHelper(const TimeSeries &ts, float maxPearson, string feature1Str, string feature2Str) override;

    virtual bool isAnomaly(Point p, correlatedFeatures c) override;


};

#endif /* HYBRIDANOMALYDETECTOR_H_ */
