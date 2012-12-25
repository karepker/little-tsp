// INCLUDES
// Built in
#include <getopt.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
// Project
#include "interaction.hpp"

using std::cout;
using std::string;
using std::endl;
using std::ostringstream;
using std::ifstream;

namespace 
{
	const unsigned int NUM_MODES = FASTTSP + 1;
	const char* MODES[NUM_MODES] = { "OPTTSP", "NAIVETSP" };
}

programmode_t checkMode(const char* optarg)
{
	for(unsigned int i = 0; i < NUM_MODES; ++i)
	{
		if(strcmp(optarg, MODES[i]) == 0)
		{
			// the optarg is one of the allowed algorithms
			return (programmode_t) (i + 1);
		}
	}

	ostringstream message;
	message << "Mode did not match one of the specified modes. " <<
		"Given \"" << optarg << "\"" << endl;
	throw MessageError(message.str());
}

programmode_t parseArgs(int argc, char* argv[])
{
	programmode_t mode = NOT_SET;
	int c;
	string line;
	ifstream help("help.txt", ifstream::in);
	ostringstream message;
	while(true)
	{
		static struct option longOptions[] =
		{
			{"help", no_argument, 0, 'h'},
			{"mode", required_argument, 0, 'm'},
			{0, 0, 0, 0} // keeps getopt_long from segfaulting on abbreviated long options
		};
		int optionIndex = 0;
		c = getopt_long(argc, argv, "hm:", longOptions, &optionIndex);
		
		// detect the end of the options
		if(c == -1)
		{
			break;
		}

		switch(c)
		{
			case '0':
				if(longOptions[optionIndex].flag != 0)
				{
					break;
				}
				printf("option %s", longOptions[optionIndex].name);
				if(optarg)
				{
					printf(" with arg %s", optarg);
				}
				printf("\n");
				break;

			case 'h':
				while(getline(help, line))
				{
					cout << line << endl;
				}
				exit(0);
				break; 

			case 'm':
				mode = checkMode(optarg);
				break;

			case '?':
				// error message already printed
				message << "No argument received" << endl;
				throw MessageError(message.str());
				break;

			default:
				abort();

		}
	}

	// ensure that mode was set
	if(mode == NOT_SET)
	{
		throw MessageError("Mode flag not specified, -m/--mode is required\n");
	}
	return mode;
}


