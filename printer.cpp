#include "printer.h"

int delayVal = 20000000;
int a = 0;

void setDelay(int inDelayVal)
{
    delayVal = inDelayVal;
}

void delay()
{
    for(int i = 0; i < 2 * delayVal; i++)
    {a++;}
}

void write(std::string in)
{
    for(int i = 0; i < in.length(); i++)
    {
        std::cout << in[i];
        delay();
    }
}
