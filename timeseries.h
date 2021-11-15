#ifndef TIMESERIES_H_
#define TIMESERIES_H_

#include <string>
#include <vector>
#include <map>

using namespace std;

class TimeSeries {

public:

    // CTOR
    explicit TimeSeries(const char *CSVfileName);

    // Returns the map containing the data.
    vector<string> getFeaturesVector() const {
        return _featuresVector;
    }

    // Returns a vector that contains the features names.
    map<string, vector<float>> getMap() const {
        return _dataMap;
    }

private:
    // Map that contains all the file's data
    map<string, vector<float>> _dataMap;

    // Vector that contains the features names.
    vector<string> _featuresVector;
};

#endif /* TIMESERIES_H_ */