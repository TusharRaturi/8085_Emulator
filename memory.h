#include <map>
#include "parser.h"

#ifndef MEMORY_H_
#define MEMORY_H_

bool loadInstruction(std::string op, TaggedSubtokens subTokens[10], int subTokenLength, int *loaderCounter, bool *hlt);

void setInitAddress(int address);

bool loadProgram(char* filename, int initAddress, int &addressSpace);
bool inputProgram(int initAddress, int &addressSpace);

void displayMem(std::string in);
void displayMem(std::string low, std::string high);
void displayUsedMem();

int getInitAddress();

#endif
