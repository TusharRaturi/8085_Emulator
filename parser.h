#include <iomanip>
#include <sstream>
#include "error.h"

#ifndef PARSER_H_
#define PARSER_H_

enum TypeData
{
    TD_NONE, TD_NUM, TD_BIG_NUM, TD_STR
};

typedef struct ts
{
    std::string value = "";
    TypeData td = TD_NONE;
}TaggedSubtokens;

std::string outPlaceAppend(std::string a, std::string b);

void extractDelimited(TaggedSubtokens token[10], std::string s, std::string delimiter, int *out_length);

int strToInt(std::string n);

int makeSignedx8(int val);
int makeSignedx16(int val);

int strHexToDec(std::string in);

int strHexToDecx8(std::string in);
int strHexToDecx16(std::string in);

std::string intToHex(int x, int digits1, int y, int digits2);
std::string intToHex(int x);
std::string intToHex(int x, int digits);

bool isHexDigit(char ch);

bool isXBitHex(std::string &in, int bits, std::string command);

bool parse(std::string in, std::string &in_op, TaggedSubtokens (&in_subTokens)[10], int &in_subTokenLength);
bool isValidOperation(std::string op, TaggedSubtokens (&subTokens)[10], int subTokenLength);

bool isRegisterOrMemory(std::string in);
bool isRegPair(std::string in);

template <typename T> std::string to_string(const T& n);

std::string trim(char in[100]);

#endif // PARSER_H_
