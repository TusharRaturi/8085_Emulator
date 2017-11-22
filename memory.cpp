#include "memory.h"
#include "memElement.h"

int initAddress;

int loadLine = 1;

std::map<std::string,int> instructionSize;

MemElement mainMem[65536];

int getInitAddress()
{
    return initAddress;
}

bool loadInstruction(std::string op, TaggedSubtokens subTokens[10], int subTokenLength, int *loaderCounter, bool *hlt)
{
    int address = *loaderCounter;
    int tAdd = *loaderCounter;

    mainMem[address].reserved = true;
    mainMem[address].sdata = op;
    mainMem[address].lineNo = loadLine++;

    for(int i = 0; i < subTokenLength; i++)
    {
        switch(subTokens[i].td)
        {
            case TD_STR:
                mainMem[address].extData[mainMem[address].extDataLength++] = subTokens[i].value;
                break;

            case TD_NUM:
                mainMem[++address].reserved = true;
                mainMem[address].sdata = subTokens[i].value;
                break;

            case TD_BIG_NUM:
                mainMem[++address].reserved = true;
                mainMem[address].sdata = subTokens[i].value.substr(0, 2);

                mainMem[++address].reserved = true;
                mainMem[address].sdata = subTokens[i].value.substr(2);
                break;

            case TD_NONE:

            default:
                error("There was an error while parsing. This may be a bug. Please contact the developer of this software.");
                return false;
        }
    }

    if(op == "hlt")
        *hlt = true;

    *loaderCounter = address + 1;
    mainMem[tAdd].pcInc = address - tAdd + 1;

    return true;
}

void setInitAddress(int address)
{
    initAddress = address;
}

bool loadProgram(char* filename, int initAddress, int &addressSpace)
{
    FILE *f = fopen(filename, "r");

    if(!f)
    {
        std::cout << "Cannot find the specified file." << std::endl;
        return false;
    }

    setInitAddress(initAddress);

    int loaderCounter = initAddress;

    char buffer[100];

    bool hlt = false;

    while(!feof(f))
    {
        if(fgets(buffer, 100, f) == NULL)
            break;

        std::string tmpop;
        TaggedSubtokens subTokens[10];
        int subTokenLength;

        if(parse(trim(buffer), tmpop, subTokens, subTokenLength))
        {
            if(hlt)
            {
                error("there are unreachable instructions after HLT. Those instructions were not loaded in memory.");
                break;
            }
            else if(!loadInstruction(tmpop, subTokens, subTokenLength, &loaderCounter, &hlt))
                    error("while loading in memory. Check if the instruction is valid");
        }
    }
    fclose(f);

    if(!hlt)
    {
        error("HLT was not found in the program. ADDED HLT at the end of program.");
        while(!hlt)
            loadInstruction("hlt", 0x0, 0, &loaderCounter, &hlt);
    }

    addressSpace = loaderCounter - initAddress - 1;

    return true;
}

bool inputProgram(int initAddress, int &addressSpace)
{
    std::cout << "Enter the program manually: " << std::endl;

    setInitAddress(initAddress);

    int loaderCounter = initAddress;

    char buffer[100];
    std::string line;

    bool hlt = false;

    while(true)
    {
        if(hlt)
            break;

        std::cin.getline(buffer, 100);

        if(buffer == "")
            break;

        std::string tmpop;
        TaggedSubtokens subTokens[10];
        int subTokenLength;

        if(parse(trim(buffer), tmpop, subTokens, subTokenLength))
            if(!loadInstruction(tmpop, subTokens, subTokenLength, &loaderCounter, &hlt))
                    error("while loading in memory. Check if the instruction is valid");
    }

    if(!hlt)
    {
        error("HLT was not found in the program. ADDED HLT at the end of program.");
        while(!hlt)
            loadInstruction("hlt", 0x0, 0, &loaderCounter, &hlt);
    }

    addressSpace = loaderCounter - initAddress - 1;

    return true;
}

void displayMem(std::string in)
{
    int i = strHexToDec(in);
    std::cout << intToHex(i, 4) << "\t" << mainMem[i].sdata << " " << mainMem[i].extData[0] << " " << mainMem[i].extData[1] << std::endl;
}

void displayMem(std::string low, std::string high)
{
    for(int i = strHexToDec(low); i <= strHexToDec(high); i++)
        displayMem(intToHex(i));
}

void displayUsedMem()
{
    for(int i = 0; i < 65536; i++)
        if(mainMem[i].used)
            displayMem(intToHex(i));

    std::cout << std::endl;
}
