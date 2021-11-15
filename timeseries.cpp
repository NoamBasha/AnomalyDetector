#include "timeseries.h"
#include <fstream>
#include <string>
#include <vector>

// Splitting a line with commas.
vector<string> splitLine(string line) {
    vector<string> linesVector;
    int currentPosition;
    line += ",";
    string delimiter = ",";
    while ((currentPosition = line.find(delimiter)) != string::npos) {
        linesVector.push_back(line.substr(0, currentPosition));
        line.erase(0, currentPosition + delimiter.length());
    }
    return linesVector;
}

// The CTOR takes a CSV file and insert its data to a map.
TimeSeries::TimeSeries(const char *CSVfileName) {
    // Creating an input file stream object called CSVfile to read the file.
    ifstream CSVfile(CSVfileName);

    string line;
    // Getting the first line of the file which contains the features names
    // and inserting them to a vector.
    getline(CSVfile, line);
    _featuresVector = splitLine(line);

    int vectorLength = _featuresVector.size();
    while (getline(CSVfile, line)) {
        vector<string> lineVector = splitLine(line);
        for (int i = 0; i < vectorLength; ++i) {
            // Inserting the data from the file to the corresponding vector.
            _dataMap[_featuresVector[i]].push_back(std::stof(lineVector[i]));
        }
    }
    CSVfile.close();
}