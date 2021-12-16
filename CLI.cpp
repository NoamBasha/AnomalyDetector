#include "CLI.h"
#include <map>
#include <vector>

void CLI::printMenu(const vector<Command *> &commands) {
    dio->write("Welcome to the Anomaly Detection Server.\n");
    dio->write("Please choose an option:\n");
    for (auto &command : commands) {
        dio->write(command->description);
    }
    dio->write("6.exit\n");
}

void CLI::start() {
    ClientData clientData;
    HybridAnomalyDetector hybridAnomalyDetector;

    vector<Command *> commands;

    UploadCommand uploadCmd = UploadCommand(dio);
    AlgorithmSettingsCommand algorithmSettingsCmd = AlgorithmSettingsCommand(dio);
    DetectAnomaliesCommand detectAnomaliesCmd = DetectAnomaliesCommand(dio);
    DisplayResultsCommand displayResultsCmd = DisplayResultsCommand(dio);
    UploadAnomaliesCommand uploadAnomaliesCmd = UploadAnomaliesCommand(dio);
    commands.push_back(&uploadCmd);
    commands.push_back(&algorithmSettingsCmd);
    commands.push_back(&detectAnomaliesCmd);
    commands.push_back(&displayResultsCmd);
    commands.push_back(&uploadAnomaliesCmd);

    printMenu(commands);
    int choice = stoi(dio->read());

    while (choice != 6) {
        commands[choice - 1]->execute(clientData, hybridAnomalyDetector);
        printMenu(commands);
        choice = stoi(dio->read());
    }
}

CLI::~CLI() {
}