//
// Created by Quoc Minh Vu on 2017-06-06.
//

#include <iostream>
#include "Command.h"

using std::cout;
using std::endl;

Command::Command() = default;

void Command::parse(const string& cmdLine, int currentLine, int lastLine) {
    resetDefaultValues();
    string tmpCmdLine = cmdLine;
    removeSpaces(tmpCmdLine);

    if (tmpCmdLine.length() == 0) // Special case: the command line is empty (user just press enter), same as '1d' - go down 1 line
        tmpCmdLine = "1d";

    /**
     * Check the last char
     * If it is a valid command symbol, set the corresponding command
     * and remove it from the command line
     * Posibility: any cmdLine ending with a valid command symbol: 'p', 'a', 'i', 'r', 'n', 'c', 'w', '=', 'u', 'd'
     */
    char lastChar = (char) tolower(tmpCmdLine.at(tmpCmdLine.length() - 1));
    if (validCommandSymbol(lastChar)) {
        cmdSymbol = lastChar;
        tmpCmdLine.pop_back();
    }

    /**
     * If the last char is a valid command symbol, it is already be removed
     * From this point, any command line should be empty or end with ',' or '.' or '$' or a digit to be valid
     */
    if (tmpCmdLine.length() == 0) { // Empty command
        fromLine = toLine = currentLine;
    } else { // Not empty command
        lastChar = (char) tolower(tmpCmdLine.at(tmpCmdLine.length() - 1));
        if ((!isdigit(lastChar)) && (lastChar != '.') && (lastChar != '$') && (lastChar != ',')) {
            cmdSymbol = lastChar;
            isValidCmd = false;
            relevantInfo = "Invalid command symbol";
            return;
        } else { /** Special case: valid cmdLine but no command symbol means 'print' */
            if (cmdSymbol == '#') // Don't set value if cmdSymbol was already be set before
                cmdSymbol = 'p';
        }
    }

    /**
     * From this point, there are two posibilities
     * 1. There is no comma
     * 2. There is one or some commas
     */
    bool is1NumProvided, is2NumProvided;
    size_t commaPosition = tmpCmdLine.find(',');
    if (commaPosition == string::npos) { // 1. There is no comma
        /**
         * Some posibilities are
         * 1.1. The cmdLine is empty (original cmdLine is 'p' or 'd' or alike)
         * 1.2. The cmdLine is '.' or '$' (original cmdLine is '.p' or '$p' or alike)
         * 1.3. The cmdLine is a number (original cmdLine is '1p' or '15d' or alike)
         * 1.4. Otherwise, the cmdLine is invalid
         */
        is2NumProvided = false;
        if (tmpCmdLine.length() == 0) { // 1.1. The cmdLine is empty (original cmdLine is 'p' or 'd' or alike)
            fromLine = toLine = currentLine;
            is1NumProvided = false;
        } else { // The cmdLine is NOT empty
            is1NumProvided = true;
            if (tmpCmdLine.length() == 1 && (tmpCmdLine.at(0) == '.')) { // 1.2. The cmdLine is '.' (original cmdLine is '.p' or '.')
                fromLine = toLine = currentLine;
            } else if (tmpCmdLine.length() == 1 && (tmpCmdLine.at(0) == '$')) { // 1.2. The cmdLine is '$' (original cmdLine is '$p' or '$')
                fromLine = toLine = lastLine;
            } else if (isPositiveNumber(tmpCmdLine)) { // 1.3. The cmdLine is a number (original cmdLine is '1p' or '15d' or alike)
                fromLine = toLine = std::stoi(tmpCmdLine);
            } else { // 1.4. Otherwise, the cmdLine is invalid
                isValidCmd = false;
                relevantInfo = "There is no comma and it's not just a number, a dot or a dollar sign";
            }
        }
    } else { // 2. There is one or some commas
        // Split the cmdLine into two parts by the first comma
        string beforeComma = tmpCmdLine.substr(0, commaPosition);
        string afterComma = tmpCmdLine.substr(commaPosition + 1, tmpCmdLine.length());

        /**
         * Some posibilities for both the strings before and after the comma
         * 2.1. The string is empty or '.' means the current line
         * 2.2. The string is '$' means the last line
         * 2.3. The string is a positive number
         * 2.4. Otherwise, the cmdLine is invalid
         */
        is1NumProvided = parseLineNumber(fromLine, beforeComma, currentLine, lastLine);
        is2NumProvided = parseLineNumber(toLine, afterComma, currentLine, lastLine);
    }

    /**
     * Different commands take numbers as input differently
     * 1. 'u' and 'd' only take one integer (the number of lines to move) as input
     * 2. Other commands require fromLine to be smaller or equal to toLine
     * 3. 'w' doesn't take any number as input
     * 4. 'a' and 'i' only take one integer (the fromLine) as input
     */
    if (isValidCmd) {
        switch (cmdSymbol) {
            case 'u': {
                unsigned int movedSteps;
                if (is2NumProvided) {
                    /**
                     * If the second number is provided (whether or not the first one is provided)
                     * Use the second one
                     */
                    movedSteps = toLine;
                    fromLine = currentLine;
					toLine = fromLine - movedSteps;
                } else if (is1NumProvided) {
                    /**
                     * If only the first number or no number is provided, use it
                     */
                    movedSteps = fromLine;
                    fromLine = currentLine;
					toLine = fromLine - movedSteps;
                } else {
                    /**
                     * If no number is provided
                     */
                    movedSteps = 1;
                    fromLine = currentLine;
					toLine = fromLine - movedSteps;
                }
                break;
            }
            case 'd': {
                unsigned int movedSteps;
                if (is2NumProvided) {
                    /**
                     * If the second number is provided (whether or not the first one is provided)
                     * Use the second one
                     */
                    movedSteps = toLine;
                    fromLine = currentLine;
					toLine = fromLine + movedSteps;
                } else if (is1NumProvided) {
                    /**
                     * If only the first number is provided, use it
                     */
                    movedSteps = fromLine;
                    fromLine = currentLine;
					toLine = fromLine + movedSteps;
                } else {
                    /**
                     * If no number is provided
                     */
                    movedSteps = 1;
                    fromLine = currentLine;
					toLine = fromLine + movedSteps;
                }
                break;
            }
            case 'w':
                fromLine = 1;
                toLine = lastLine;
                break;
            case 'a': // New content between input line and the line after it
                if (is2NumProvided && validateLineNumbers(toLine, lastLine)) {
                    fromLine = toLine;
                    toLine = fromLine + 1;
                } else if (validateLineNumbers(fromLine, lastLine)) {
                    toLine = fromLine + 1;
                }
                break;
            case 'i': // New content between input line and the line before it
                if (is2NumProvided && validateLineNumbers(toLine, lastLine)) {
                    fromLine = toLine - 1;
                } else if (validateLineNumbers(fromLine, lastLine)) {
                    toLine = fromLine;
                    fromLine = toLine - 1;
                }
                break;
            default: // Other commands require fromLine to be smaller or equal to toLine
                if (validateLineNumbers(fromLine, lastLine) && validateLineNumbers(toLine, lastLine) && (toLine < fromLine)) {
                    relevantInfo = "fromLine must be smaller or equal to toLine";
                    isValidCmd = false;
                }
                break;
        }
    }
}

char Command::getCmdSymbol() const {
    return cmdSymbol;
}

int Command::getFromLine() const {
    return fromLine;
}

int Command::getToLine() const {
    return toLine;
}

bool Command::isIsValidCmd() const {
    return isValidCmd;
}

const string &Command::getRelevantInfo() const {
    return relevantInfo;
}

bool Command::isPositiveNumber(const string &str) {
    for (auto it = str.begin(); it != str.end(); it++)
        if (!isdigit(*it))
            return false;
    return true;
}

bool Command::parseLineNumber(int& targetLine, const string& str, int currentLine, int lastLine) {
    /**
     * Some posibilities for both the strings before and after the comma
     * 2.1. The string is empty or '.' means the current line
     * 2.2. The string is '$' means the last line
     * 2.3. The string is a positive number
     * 2.4. Otherwise, the cmdLine is invalid
     *
     * The returned value tells if the line number is explicitly provided or not
     */
    if (str.length() == 0) { // 2.1. The string is empty means the current line
        targetLine = currentLine;
    } else if ((str.length() == 1) && (str.at(0) == '.')) { // 2.1. The string is '.' means the current line
        targetLine = currentLine;
        return true;
    } else if ((str.length() == 1) && (str.at(0) == '$')) { // 2.2. The string is '$' means the last line
        targetLine = lastLine;
        return true;
    } else if (isPositiveNumber(str)) { // 2.3. The string is a positive number
        targetLine = std::stoi(str);
        return true;
    } else { // 2.4. Otherwise, the cmdLine is invalid
        isValidCmd = false;
        relevantInfo = "invalid line number format";
    }
    return false;
}

void Command::removeSpaces(string& str) {
    auto it = str.begin();
    while (it != str.end()) {
        if ((*it == ' ') || (*it == '\t'))
            str.erase(it);
        else
            it++;
    }
}

bool Command::validCommandSymbol(char cmd) {
    for (int i = 0; i < (sizeof(validCmds)/sizeof(char)); i++)
        if (cmd == validCmds[i])
            return true;
    return false;
}

void Command::resetDefaultValues()	 {
    isValidCmd = true;
    fromLine = 0;
    toLine = 0;
    cmdSymbol = '#';
    relevantInfo = "";
}

bool Command::validateLineNumbers(int targetLine, int lastLine) {
    if ((targetLine > lastLine) || (targetLine < 1)) {
		relevantInfo = "line numbers range from 1 to " + std::to_string(lastLine);
        return (isValidCmd = false);
    }
    return true;
}
