//
// Created by Quoc Minh Vu on 2017-06-06.
//

#ifndef ASSIGNMENT02_COMMAND_H
#define ASSIGNMENT02_COMMAND_H

#include <string>
using std::string;

class Command {
private:
    char validCmds[11] = {'p', 'a', 'i', 'r', 'n', 'c', 'w', '=', 'u', 'd', 'q'};
    int fromLine;
    int toLine;
    char cmdSymbol;
    bool isValidCmd;
    string relevantInfo;

    bool parseLineNumber(int&, const string&, int, int);
    bool isPositiveNumber(const string&);
    void removeSpaces(string&);
    bool validCommandSymbol(char);
    void resetDefaultValues();
    bool validateLineNumbers(int, int);
public:
    Command();
    void parse(const string &, int, int);
    char getCmdSymbol() const;
    int getFromLine() const;
	int getToLine() const;
    bool isIsValidCmd() const;
    const string &getRelevantInfo() const;
};


#endif //ASSIGNMENT02_COMMAND_H
