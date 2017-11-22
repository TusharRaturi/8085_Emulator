#include <iostream>
#include <string>
#include <algorithm>
#include <bitset>

#ifndef INSTRUCTIONS_H_
#define INSTRUCTIONS_H_

typedef std::bitset<1> Flag;
extern int A, B, C, D, E, H, L;
extern Flag fS, fZ, fAC, fP, fC;
extern bool debug;
extern bool step;

void mov(std::string oper1, std::string oper2);
int staxGetLoc(int x, int y);
void swap(int *x, int *y);
void setFlags(int A, bool carryAffected);
Flag getParity(unsigned int x);
void dad(int tmp1, int tmp2);

#endif // INSTRUCTIONS_H_
