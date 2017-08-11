// Driver program to test the
// Led class impliemented in assignment 2
#include<iostream>
#include<string>
using std::cout;
using std::endl;
using std::string;
#include "Led.h"

int main(int argc, char * argv[])
{
	string filename; 
	switch (argc)
	{
	case 1:  // no file name
		break;
	case 2:  // read from argument string
		filename = argv[1]; // initialize filename
		break;
	default:
		cout << "too many arguments - all discarded" << endl;
		break;
	}
	Led led(filename);
	led.run();
	return 0; // done
}
