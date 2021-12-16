#ifndef COMMANDS_H_
#define COMMANDS_H_

#include <iostream>
#include <string.h>
#include <fstream>
#include <vector>
#include <unistd.h>
#include "HybridAnomalyDetector.h"
#include <sstream>

using namespace std;

class DefaultIO {
public:
    virtual string read() = 0;

    virtual void write(string text) = 0;

    virtual void write(float f) = 0;

    virtual void read(float *f) = 0;

    virtual void upload(const char *CSVFileName) {
        string line;
        ofstream trainFile(CSVFileName);
        line = read();
        while (line != "done") {
            trainFile << line << endl;
            line = read();
        }
        trainFile.close();
    }

    virtual ~DefaultIO() {}

    // you may add additional methods here
};

class SocketIO : public DefaultIO {
    int _clientID;
public:
    SocketIO(int clientID) : _clientID(clientID) {};

    string read() override {
        string serverInput = "";
        char c = 0;
        ::read(_clientID, &c, sizeof(char));
        while (c != '\n') {
            serverInput += c;
            ::read(_clientID, &c, sizeof(char));
        }
        return serverInput;
    }

    void write(string text) override {
        ::write(_clientID, text.c_str(), text.length());
    }

    void write(float f) override {
        stringstream str_strm;
        str_strm << f;
        string fStr = str_strm.str();
        write(fStr);
    }

    void read(float *f) override {
        read();
    }
};

struct ClientData {
    const char *CSVTrainFile;
    const char *CSVTestFile;
    vector<AnomalyReport> anomalyReports;
};

class Command {
    friend class CLI;

protected:
    DefaultIO *dio;
    string description;
public:
    Command(DefaultIO *dio) : dio(dio) {};

    virtual void execute(ClientData &clientData, HybridAnomalyDetector &hybridAnomalyDetector) = 0;

    virtual ~Command() {};
};

class UploadCommand : public Command {
public:
    UploadCommand(DefaultIO *dio) : Command(dio) {
        description = "1.upload a time series csv file\n";
    };

    void execute(ClientData &clientData, HybridAnomalyDetector &hybridAnomalyDetector) override {
        dio->write("Please upload your local train CSV file.\n");
        dio->upload("anomalyTrain.csv");
        clientData.CSVTrainFile = "anomalyTrain.csv";
        dio->write("Upload complete.\n");

        dio->write("Please upload your local test CSV file.\n");
        dio->upload("anomalyTest.csv");
        clientData.CSVTestFile = "anomalyTest.csv";
        dio->write("Upload complete.\n");
    }
};

class AlgorithmSettingsCommand : public Command {
public:
    AlgorithmSettingsCommand(DefaultIO *dio) : Command(dio) {
        description = "2.algorithm settings\n";
    };

    void execute(ClientData &clientData, HybridAnomalyDetector &hybridAnomalyDetector) override {
        dio->write("The current correlation threshold is ");
        dio->write(hybridAnomalyDetector.getThreshold());
        dio->write("\nType a new threshold\n");
        float userInput = stof(dio->read());
        while (userInput < 0 || 1 < userInput) {
            dio->write("please choose a value between 0 and 1.\n");
            userInput = stof(dio->read());
        }
        hybridAnomalyDetector.setThreshold(userInput);
    }
};

class DetectAnomaliesCommand : public Command {
public:
    DetectAnomaliesCommand(DefaultIO *dio) : Command(dio) {
        description = "3.detect anomalies\n";
    };

    void execute(ClientData &clientData, HybridAnomalyDetector &hybridAnomalyDetector) override {
        hybridAnomalyDetector.learnNormal(TimeSeries(clientData.CSVTrainFile));
        clientData.anomalyReports = hybridAnomalyDetector.detect(TimeSeries(clientData.CSVTestFile));
        dio->write("anomaly detection complete.\n");
    }
};

class DisplayResultsCommand : public Command {
public:
    DisplayResultsCommand(DefaultIO *dio) : Command(dio) {
        description = "4.display results\n";
    };

    void execute(ClientData &clientData, HybridAnomalyDetector &hybridAnomalyDetector) override {
        auto anomalies = clientData.anomalyReports;
        for (auto anomaly : anomalies) {
            dio->write(anomaly.timeStep);
            dio->write("\t ");
            dio->write(anomaly.description);
            dio->write("\n");
        }
        dio->write("Done.\n");
    }
};

struct ClientRange {
    int begin;
    int end;
};

struct ClientAnomalies {
    string description;
    ClientRange range;
};

class UploadAnomaliesCommand : public Command {
public:
    UploadAnomaliesCommand(DefaultIO *dio) : Command(dio) {
        description = "5.upload anomalies and analyze results\n";
    };

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

    void execute(ClientData &clientData, HybridAnomalyDetector &hybridAnomalyDetector) override {
        dio->write("Please upload your local anomalies file.\n");
        vector<string> linesVector;
        string line;
        vector<ClientRange> clientRanges;
        line = dio->read();
        while (line != "done") {
            ClientRange cr;
            linesVector = splitLine(line);
            cr.begin = stoi(linesVector[0]);
            cr.end = stoi(linesVector[1]);
            clientRanges.push_back(cr);
            line = dio->read();
        }


        TimeSeries ts(clientData.CSVTestFile);
        map<string, vector<float>> tsMap = ts.getMap();
        vector<string> tsFeaturesVector = ts.getFeaturesVector();
        // n = timeseriesSize
        int n = tsMap[tsFeaturesVector[0]].size();
        bool consistency = true;
        int consistencyCounter = 0;
        auto anomalies = clientData.anomalyReports;
        int anomaliesLength = anomalies.size();
        vector<ClientAnomalies> clientAnomalies;

        int i = 0;
        for (i = 0; i < anomaliesLength - 1; ++i) {
            if (anomalies[i].description != anomalies[i + 1].description) {
                consistency = false;
            } else {
                consistencyCounter++;
            }
            if (!consistency && consistencyCounter != 0) {
                ClientAnomalies ca;
                ca.description = anomalies[i].description;
                ClientRange cr;
                cr.begin = (int) anomalies[i].timeStep - consistencyCounter;
                cr.end = (int) anomalies[i].timeStep;
                ca.range = cr;
                clientAnomalies.push_back(ca);
                consistency = true;
                consistencyCounter = 0;
            }
        }
        if (consistency) {
            ClientAnomalies ca;
            ca.description = anomalies[i].description;
            ClientRange cr;
            cr.begin = (int) anomalies[i].timeStep - consistencyCounter;
            cr.end = (int) anomalies[i].timeStep;
            ca.range = cr;
            clientAnomalies.push_back(ca);
            consistency = true;
            consistencyCounter = 0;
        }


        int counter = 0;
        for (ClientRange cr : clientRanges) {
            counter += cr.end - cr.begin - 1;
        }

        int N = n - counter;

        int FP = 0, TP = 0;
        int FPCounter = 0;
        for (ClientAnomalies ca : clientAnomalies) {
            for (ClientRange cr : clientRanges) {
                if (ca.range.end < cr.begin || ca.range.begin > cr.end) {
                    FPCounter++;
                } else {
                    TP++;
                }
            }
            if (FPCounter == clientRanges.size()) {
                FP++;
            }
            FPCounter = 0;
        }

        dio->write("Upload complete.\n");

        dio->write("True Positive Rate: ");
        float TPdivided =
                floor(((float) TP / (float) clientRanges.size()) * (float) pow(10.0, 3)) / (float) pow(10.0, 3);
        dio->write(TPdivided);

        dio->write("\n");
        dio->write("False Positive Rate: ");
        float FPdivided = floor(((float) FP / (float) N) * (float) pow(10.0, 3)) / (float) pow(10.0, 3);
        dio->write(FPdivided);
        dio->write("\n");

    }
};


#endif /* COMMANDS_H_ */
