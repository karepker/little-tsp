#ifndef __INTERACTION__
#define __INTERACTION__

// INCLUDES
#include "common.hpp"

enum programmode_t { NOT_SET = 0, OPTTSP, NAIVETSP };

programmode_t parseArgs(int argc, char* argv[]);
programmode_t checkMode(const char* optarg);
	
#endif // __INTERACTION__
