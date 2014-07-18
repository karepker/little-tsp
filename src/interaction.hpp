#ifndef INTERACTION_H
#define INTERACTION_H

enum programmode_t { NOT_SET, OPTTSP, NAIVETSP };

programmode_t ParseArgs(int argc, char* argv[]);
programmode_t CheckMode(const char* optarg);

#endif  // INTERACTION_H
