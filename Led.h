//
// Created by Quoc Minh Vu on 2017-06-06.
//

#ifndef ASSIGNMENT02_LED_H
#define ASSIGNMENT02_LED_H

#include <string>
#include <list>
#include "Command.h"

using std::string;

class Led {
private:
    string fileName;
    std::list<string> buffer;
    std::list<string>::iterator currentLine;
    std::list<string>::iterator lastLine;
    bool isSaved;
    void execute(const Command&);
    void print(std::list<string>::iterator, std::list<string>::iterator, bool);
	void printLineNumber(std::list<string>::iterator);
    void append(std::list<string>::iterator);
    void insert(std::list<string>::iterator);
    void remove(std::list<string>::iterator, std::list<string>::iterator);
    void change(std::list<string>::iterator, std::list<string>::iterator);
    void moveTo(std::list<string>::iterator);
    void writeToFile();
    void quitEditor();
    void addNewContent(const std::list<string>::iterator&, const std::list<string> &);
	void prepareFile();
public:
    Led();
    Led(const string&);
    void run();
    virtual ~Led();
};

#endif //ASSIGNMENT02_LED_H
