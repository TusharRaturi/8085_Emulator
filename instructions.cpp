#include "instructions.h"
#include "memElement.h"
#include "Main.h"

int A, B, C, D, E, H, L;
Flag fS, fZ, fAC, fP, fC;
bool debug = false;
bool step = false;

void mov2(std::string x, int *reg, std::string *memLoc, bool mem)
{
    if(x == "a")
        if(mem)
            *memLoc = intToHex(A);
        else
            *reg = A;
    else if(x == "b")
        if(mem)
            *memLoc = intToHex(B);
        else
            *reg = B;
    else if(x == "c")
        if(mem)
            *memLoc = intToHex(C);
        else
            *reg = C;
    else if(x == "d")
        if(mem)
            *memLoc = intToHex(D);
        else
            *reg = D;
    else if(x == "e")
        if(mem)
            *memLoc = intToHex(E);
        else
            *reg = E;
    else if(x == "h")
        if(mem)
            *memLoc = intToHex(H);
        else
            *reg = H;
    else if(x == "l")
        if(mem)
            *memLoc = intToHex(L);
        else
            *reg = L;
    else if(x == "m")
        *reg = strHexToDec(mainMem[strHexToDec(intToHex(H, 2, L, 2))].sdata);
}

void mov(std::string oper1, std::string oper2)
{
    if(oper1 == "a")
        mov2(oper2, &A, 0x0, false);
    else if(oper1 == "b")
        mov2(oper2, &B, 0x0, false);
    else if(oper1 == "c")
        mov2(oper2, &C, 0x0, false);
    else if(oper1 == "d")
        mov2(oper2, &D, 0x0, false);
    else if(oper1 == "e")
        mov2(oper2, &E ,0x0, false);
    else if(oper1 == "h")
        mov2(oper2, &H, 0x0, false);
    else if(oper1 == "l")
        mov2(oper2, &L, 0x0, false);
    else if(oper1 == "m")
    {
        mov2(oper2, 0x0, &(mainMem[strHexToDec(intToHex(H, 2, L, 2))].sdata), true);
        mainMem[strHexToDec(intToHex(H, 2, L, 2))].used = true;
    }
}

int staxGetLoc(int x, int y)
{
    return strHexToDec(outPlaceAppend(intToHex(x, 2), intToHex(y, 2)));
}

void swap(int *x, int *y)
{
    int tmp;

    tmp = *x;
    *x = *y;
    *y = tmp;
}

void setFlags(int A, bool carryAffected)
{
    if(carryAffected)
    {
        if(A > 255)
            fC = 1;
        else
            fC = 0;
    }

    fP = getParity(A);

    if(A < 0)
        fS = 1;
    else
        fS = 0;

    if(A == 0)
        fZ = 1;
    else
        fZ = 0;
}

Flag getParity(unsigned int x)
{
    unsigned int count = 0, i, b = 1;

    for(i = 0; i < 32; i++)
        if(x & (b << i))
            count++;

    if(count % 2)
        return 1;

    return 0;
}

void dad(int tmp1, int tmp2)
{
    int tmp3 = tmp1 + tmp2;

    if(tmp3 >= 65535)
        fC = 1;

    std::string hE = intToHex(tmp3, 4);

    std::reverse(hE.begin(), hE.end());

    std::string low = hE.substr(0, 2);
    std::string high = hE.substr(2, 2);

    std::reverse(low.begin(), low.end());
    std::reverse(high.begin(), high.end());

    L = strHexToDec(low);
    H = strHexToDec(high);
}
