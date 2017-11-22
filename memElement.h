#include "parser.h"

#ifndef MEMELE_H_
#define MEMELE_H_

typedef struct mem_element
{
    bool reserved = false;
    std::string sdata = "";

    std::string extData[2] = {"", ""};
    int extDataLength = 0;

    int lineNo = -1;

    int pcInc = 0;
    bool brkPt = false;

    bool used = false;
}MemElement;

extern MemElement mainMem[65536];

#endif // MEMELE_H_
