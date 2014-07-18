#include "interaction.hpp"

#include <getopt.h>
#include <cstring>

#include <iostream>
#include <fstream>
#include <sstream>

#include "util.hpp"

using std::cout;
using std::string;
using std::endl;
using std::ifstream;

const unsigned int NUM_MODES{NAIVETSP + 1};
const char* MODES[NUM_MODES] = { "OPTTSP", "NAIVETSP" };
const programmode_t DEFAULT_MODE{OPTTSP};

programmode_t CheckMode(const char* optarg) {
	for (unsigned int i = 0; i < NUM_MODES; ++i) {
		if (strcmp(optarg, MODES[i]) == 0) {
			// the optarg is one of the allowed algorithms
			return (programmode_t) (i + 1);
		}
	}
	throw Error{"Mode did not match one of the specified modes!"};
}

programmode_t ParseArgs(int argc, char* argv[]) {
	programmode_t mode{NOT_SET};
	string line;
	ifstream help{"help.txt", ifstream::in};
	while (true) {
		static struct option longOptions[] =
		{
			{"help", no_argument, 0, 'h'},
			{"mode", required_argument, 0, 'm'},
			{0, 0, 0, 0} // keeps getopt_long from segfaulting on abbreviated long options
		};
		int optionIndex{0};
		int c{getopt_long(argc, argv, "hm:", longOptions, &optionIndex)};

		// detect the end of the options
		if (c == -1) { break; }

		switch(c) {
			case '0':
				if (longOptions[optionIndex].flag != 0) { break; }
				cout << "option " << longOptions[optionIndex].name;
				if (optarg) { cout << " with arg " << optarg; }
				cout << endl;
				break;

			case 'h':
				while (getline(help, line)) { cout << line << endl; }
				exit(0);
				break;

			case 'm':
				mode = CheckMode(optarg);
				break;

			case '?':
				// error message already printed
				throw Error{"No argument received!"};
				break;

			default:
				throw Error{"Weird getopt issues."};
		}
	}

	// set mode to default if flag no given
	if (mode == NOT_SET) { mode = DEFAULT_MODE; }
	return mode;
}


