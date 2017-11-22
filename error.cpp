#include "error.h"

void error(std::string s)
{
    std::cout << "Error: " << s << std::endl;
}

void expected(std::string s)
{
    error(s.append(" expected."));
}
