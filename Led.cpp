//
// Created by Quoc Minh Vu on 2017-06-06.
//

#include <iostream>
#include <fstream>
#include <algorithm>
#include "Led.h"

using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;

Led::Led() : currentLine(buffer.begin()), lastLine(buffer.begin()), fileName("") {};

Led::Led(const string& fileName) : currentLine(buffer.begin()), lastLine(buffer.begin()), fileName(fileName) {}

void Led::prepareFile() {
    if (!fileName.empty()) {
        ifstream fin(fileName.c_str());
        if (fin) { // The file exists
            int lineCount = 0;
            while (!fin.eof()) {
                string tmp;
                getline(fin, tmp);
                buffer.push_back(tmp);
				lineCount++;
            }
			currentLine = lastLine = --buffer.end();
            cout << "\"" << fileName << "\" ";
            cout << lineCount << " lines" << endl;
        } else { // The file doesn't exist
            cout << "Unable to open file " << fileName << endl;
            cout << "\"" << fileName << "\" " << "[New File]" << endl;
        }
    } else
        cout << "\"?\" " << "[New file]" << endl;
	isSaved = true;
}

void Led::run() {
	prepareFile();
	cout << "Entering command mode." << endl;
    Command cmd;
    string cmdLine;
    do {
        cout << ": ";
        getline(cin, cmdLine);
        cmd.parse(cmdLine, (unsigned int) std::distance(buffer.begin(), currentLine) + 1, (unsigned int) std::distance(buffer.begin(), lastLine) + 1);
        if (cmd.isIsValidCmd())
            execute(cmd);
        else
            cout << cmd.getRelevantInfo() << endl;
    } while (cmd.getCmdSymbol() != 'q');
    quitEditor();
}

void Led::execute(const Command& cmd) {
    switch (cmd.getCmdSymbol()) {
        case 'p':
            print(std::next(buffer.begin(), cmd.getFromLine() - 1), std::next(buffer.begin(), cmd.getToLine() - 1), false);
            break;
        case 'a':
            append(std::next(buffer.begin(), cmd.getFromLine() - 1));
            break;
        case 'i':
            insert(std::next(buffer.begin(), cmd.getToLine() - 1));
            break;
        case 'r':
            remove(std::next(buffer.begin(), cmd.getFromLine() - 1), std::next(buffer.begin(), cmd.getToLine() - 1));
            break;
        case 'n':
            print(std::next(buffer.begin(), cmd.getFromLine() - 1), std::next(buffer.begin(), cmd.getToLine() - 1), true);
            break;
        case 'c':
            change(std::next(buffer.begin(), cmd.getFromLine() - 1), std::next(buffer.begin(), cmd.getToLine() - 1));
            break;
        case 'w':
            writeToFile();
            break;
        case '=':
            printLineNumber(currentLine);
            break;
		case 'u': {
			int moveToLine = cmd.getToLine() - 1;
			if (moveToLine < 0 || buffer.empty()) {
				cout << "BOF reached" << endl;
				moveToLine = 0;
			}
			moveTo(std::next(buffer.begin(), moveToLine));
			break;
		}
		case 'd': {
			int moveToLine = cmd.getToLine() - 1;
			if (buffer.empty()) {
				cout << "EOF reached" << endl;
				moveToLine = buffer.size();
			} else if (moveToLine >= buffer.size()) {
				cout << "EOF reached" << endl;
				moveToLine = buffer.size() - 1;
			}
			moveTo(std::next(buffer.begin(), moveToLine));
			break;
		}
        case 'q':
            break;
        default:
            break;
    }
}

Led::~Led() = default;

void Led::print(std::list<string>::iterator fromLine, std::list<string>::iterator toLine, bool withNumbers) {
	if (!buffer.empty()) {
		if (withNumbers) {
			auto it = fromLine;
			toLine++;
			while (it != toLine) {
				cout << std::distance(buffer.begin(), it) + 1 << "  " << *it << endl;
				it++;
			}
		}
		else {
			auto it = fromLine;
			toLine++;
			while (it != toLine) {
				cout << *it << endl;
				it++;
			}
		}
		currentLine = --toLine;
	}
	else {
		if (withNumbers)
			cout << "0" << endl;
		else
			cout << endl;
	}
}

void Led::printLineNumber(std::list<string>::iterator currentLine) {
	if (buffer.empty())
		cout << 0 << endl;
	else
		cout << std::distance(buffer.begin(), currentLine) + 1 << endl;
}

void Led::append(std::list<string>::iterator fromLine) {
    if (buffer.size() == 0) {
        insert(fromLine);
    } else {
        std::list<string> newContent;
        string line;
        while (true) {
            getline(cin, line);
            if (line.compare(".") == 0)
                break;
            newContent.push_back(line);
        }
        auto toLine = std::next(buffer.begin(), std::distance(buffer.begin(), fromLine) + 1);
        addNewContent(toLine, newContent);
    }
}

void Led::insert(std::list<string>::iterator toLine) {
    std::list<string> newContent;
    string line;
    while (true) {
        getline(cin, line);
        if (line.compare(".") == 0)
            break;
        newContent.push_back(line);
    }
    addNewContent(toLine, newContent);
}

void Led::remove(std::list<string>::iterator fromLine, std::list<string>::iterator toLine) {
	if (!buffer.empty()) {
		if (std::distance(toLine, buffer.end()) > 1) // toLine is not the last line
			currentLine = std::next(buffer.begin(), std::distance(buffer.begin(), toLine) + 1);
		else if (std::distance(buffer.begin(), fromLine) >= 1) // fromLine is not the first line
			currentLine = std::next(buffer.begin(), std::distance(buffer.begin(), fromLine) - 1);
		buffer.erase(fromLine, ++toLine);
		if (buffer.empty()) {
			lastLine = buffer.end();
			currentLine = buffer.end();
		}
		else
			lastLine = std::prev(buffer.end(), 1);
		isSaved = false;
	}
}

void Led::change(std::list<string>::iterator fromLine, std::list<string>::iterator toLine) {
	bool isChanged = false;
    string changeWhat, toWhat;
    cout << "change what? ";
    getline(cin, changeWhat);
    unsigned long changeLen = changeWhat.length();
    cout << "    to what? ";
    getline(cin, toWhat);
	if (!buffer.empty()) {
		unsigned long toLen = toWhat.length();
		toLine++;
		for (auto iter = fromLine; iter != toLine; iter++) {
			string currentStr = *iter;

			for (int i = 0; i < currentStr.length(); i++) {
				// Try to find the substring
				unsigned long pos = currentStr.find(changeWhat, i);

				// If don't find anything, move to the next line
				if (pos == string::npos)
					break;

				// Find something
				// Replace the substring
				currentStr.replace(pos, changeLen, toWhat);
				*iter = currentStr;
				// Move the index to the rest of the original string to continue searching
				//i += pos + toLen - 1;
				i = pos + toLen - 1;
				isChanged = true;
			}
		}
		
		if (isChanged) {
			currentLine = --toLine;
			isSaved = false;
		}
		else
			cout << "No change is made!" << endl;
	}
}

void Led::moveTo(std::list<string>::iterator toLine) {
    currentLine = toLine;
    print(currentLine, currentLine, false);
}

void Led::writeToFile() {
	while (fileName.empty()) {
		cout << "Enter the file name: ";
		getline(cin, fileName);
	}

    ofstream fout(fileName.c_str());
	int lineCount = 0;
	if (buffer.empty()) {
		fout << "";
	}
	else {
		for (auto iter = buffer.begin(); iter != buffer.end(); iter++) {
			fout << *iter << endl;
			lineCount++;
		}
	}
    fout.close();
	cout << "\"" << fileName << "\" " << lineCount << " lines written" << endl;
    isSaved = true;
}

void Led::quitEditor() {
    if (!isSaved) {
		int invalidAnsCount = 0;
        
        bool validAns = false;
        do {
			if (invalidAnsCount == 0)
				cout << "Save changes to \"" << fileName << "\" (y/n)?  ";
			else if (invalidAnsCount == 1)
				cout << "invalid answer: ";
			else if (invalidAnsCount == 2) {
				cout << "enter y for yes and n for no." << endl;
				cout << "Save changes to \"" << fileName << "\" (y/n)?  ";
				invalidAnsCount = 0;
			}

            string answer;
			getline(cin, answer);
            std::transform(answer.begin(), answer.end(), answer.begin(), ::tolower);
			if (answer.compare("y") == 0) {
				validAns = true;
				writeToFile();
			} else if (answer.compare("n") == 0)
				validAns = true;
			else {
				invalidAnsCount++;
				validAns = false;
			}
				
        } while (!validAns);
    }
	cout << "quitting led . . . bye." << endl;
}

void Led::addNewContent(const std::list<string>::iterator& toLine, const std::list<string> &newContent) {
	if (!newContent.empty()) {
		buffer.insert(toLine, newContent.begin(), newContent.end());
		currentLine = std::next(buffer.begin(), std::distance(buffer.begin(), toLine) - 1);
		lastLine = std::prev(buffer.end(), 1);
		isSaved = false;
	}
}
