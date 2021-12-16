#ifndef CLI_H_
#define CLI_H_

#include <string.h>
#include "commands.h"

using namespace std;

class CLI {
    DefaultIO *dio;
    // you can add data members
public:
    CLI(DefaultIO *dio) : dio(dio) {};

    void start();

    virtual ~CLI();

private:
    void printMenu(const vector<Command *> &commands);
};

#endif /* CLI_H_ */