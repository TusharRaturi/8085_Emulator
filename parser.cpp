#include "parser.h"

std::string outPlaceAppend(std::string a, std::string b)
{
    return std::string().append(a).append(b);
}

void extractDelimited(TaggedSubtokens token[10], std::string s, std::string delimiter, int *out_length)
{
    size_t pos = 0;

    int i = 0;

    while ((pos = s.find(delimiter)) != std::string::npos)
    {
        token[i++].value = s.substr(0, pos);
        s.erase(0, pos + delimiter.length());
    }

    *out_length = i;

    if(s.length() > 0)
    {
        if(s == delimiter)
            return;

        (*out_length)++;
        token[i].value = s;
    }
}

int strToInt(std::string n)
{
    int tmp = 0;

    int x = 0;

    for(int i = 0; i < n.length(); i++)
    {
        if(!isdigit(n[i]))
        {
            error("Found Invalid Number/Integer");
            return 0;
        }
    }

    while(x < n.length())
    {
        int dig = n[x] - 48;
        tmp = tmp * 10 + dig;
        x++;
    }

    return tmp;
}

int makeSignedx16(int val)
{
    return (0x8000&val ? (int)(0x7FFF&val)-0x8000 : val);
}

int makeSignedx8(int val)
{
    return (0x80&val ? (int)(0x7F&val)-0x80 : val);
}

int strHexToDec(std::string in)
{
    if(!(in.length() <= 4))
        error("There was an error while Integer-Hex conversion. This is most probably a bug. Please contact the developer of this software.");

    unsigned int x;
    std::stringstream ss;

    ss << std::hex << in;
    ss >> x;

    //std::cout << "LENGTH IS: " << in.length() <<" IN STR HEX TO DEC:" << x << std::endl;

    return static_cast<int>(x);
}

int strHexToDecx8(std::string in)
{
    /*unsigned int x;
    stringstream ss;

    ss << hex << in;
    ss >> x;

    return makeSignedx8(static_cast<int>(x));*/
    return strHexToDec(in);
}

int strHexToDecx16(std::string in)
{
    /*unsigned int x;
    stringstream ss;

    ss << hex << in;
    ss >> x;

    return makeSignedx16(static_cast<int>(x));*/
    return strHexToDec(in);
}

std::string intToHex(int x, int digits1, int y, int digits2)
{
    return outPlaceAppend(intToHex(x, digits1), intToHex(y, digits2));
}

std::string intToHex(int x, int digits)
{
    std::stringstream stream;
    stream << std::setfill('0') << std::setw(digits) << std::hex << x;
    return stream.str();
}

std::string intToHex(int x)
{
    std::stringstream stream;
    stream << std::setfill('0') << std::setw(2) << std::hex << x;
    return stream.str();
}

bool isHexDigit(char ch)
{
    return isdigit(ch) || ch == 'a' || ch == 'b' || ch == 'c' || ch == 'd' || ch == 'e' || ch == 'f';
}

bool parse(std::string in, std::string &in_op, TaggedSubtokens (&in_subTokens)[10], int &in_subTokenLength)
{
    for(int i = 0; i < in.length(); i++)
    {
        if(!isalnum(in[i]) && in[i] != ' ' && in[i] != ',')
        {
            error(std::string("Illegal symbol: ").append(std::string(1, in[i])));
            return false;
        }
    }

    int i = 0;

    TaggedSubtokens tokens[10];
    int tokenLength = 0;

    extractDelimited(tokens, in, " ", &tokenLength);

    TaggedSubtokens subTokens[10];
    int subTokenLength = 0;

    extractDelimited(subTokens, tokens[1].value, ",", &subTokenLength);

    if(isValidOperation(tokens[0].value, subTokens, subTokenLength))
    {
        in_op = tokens[0].value;

        for(int i = 0; i < subTokenLength; i++)
            in_subTokens[i] = subTokens[i];

        in_subTokenLength = subTokenLength;

        return true;
    }
    else
        error(std::string("Instruction ").append(tokens[0].value).append(" is either wrong / wrongly written or undefined"));

    return false;
}

template <typename T> std::string to_string(const T& n)
{
    std::ostringstream strm;
    strm << n;
    return strm.str();
}

bool isXBitHex(std::string &in, int bits, std::string command)
{
    int noOfBytes = bits / 8;
    int numLength = bits / 4;

    if(in.length() < 1 || in.length() > numLength)
    {
        error(std::string("Only ").append(to_string(noOfBytes)).append(std::string(" byte(s) of data can be used with ")).append(command).append(std::string(" which should be in the format: ")).append(std::string(noOfBytes == 1 ? "XX" : "XXXX")).append("(where each X is 0 to f in hex)"));
        return false;
    }

    for(int i = 0; i < in.length(); i++)
        if(!isHexDigit(in[i]))
        {
            error(std::string("Illegal Symbol: ") += in[i]);
            error(std::string("Only ").append(to_string(noOfBytes)).append(std::string(" byte(s) of data can be used with ")).append(command).append(std::string(" which should be in the format: ")).append(std::string(noOfBytes == 1 ? "XX" : "XXXX")).append("(where each X is 0 to f in hex)"));
            return false;
        }

    while(in.length() < numLength)
        in.insert(0, "0");

    return true;
}

bool isRegisterOrMemory(std::string in)
{
    if(in == "a" || in == "b" || in == "c" || in == "d" || in == "e" || in == "h" || in == "l" || in == "m")
        return true;

    return false;
}

bool isRegPair(std::string in)
{
    if(in == "b" || in == "d" || in == "h")
        return true;
    return false;
}

bool isValidOperation(std::string op, TaggedSubtokens (&subTokens)[10], int subTokenLength)
{
    if(op == "mov")
    {
        if(subTokenLength != 2)
            error("Wrong number of arguments.");
        else
        {
            if(isRegisterOrMemory(subTokens[0].value))
            {
                if(isRegisterOrMemory(subTokens[1].value))
                {
                    if(!(subTokens[0].value == "m" && subTokens[1].value == "m"))
                    {
                        subTokens[0].td = TD_STR;
                        subTokens[1].td = TD_STR;

                        return true;
                    }
                    else
                        error("Both cannot be memory");
                }
                else
                    expected("Register or memory in argument 2");
            }
            else
                expected("Register or memory in argument 1");
        }
    }
    else if(op == "mvi")
    {
        if(subTokenLength != 2)
            error("Wrong number of arguments.");
        else
        {
            if(isRegisterOrMemory(subTokens[0].value))
            {
                if(isXBitHex(subTokens[1].value, 8, op))
                {
                    subTokens[0].td = TD_STR;
                    subTokens[1].td = TD_NUM;

                    return true;
                }
            }
            else
                expected("Register in argument 1");
        }
    }
    else if(op == "lxi")
    {
        if(subTokenLength != 2)
            error("Wrong number of arguments.");
        else
        {
            if(isRegPair(subTokens[0].value))
            {
                if(isXBitHex(subTokens[1].value, 16, op))
                {
                    subTokens[0].td = TD_STR;
                    subTokens[1].td = TD_BIG_NUM;

                    return true;
                }
            }
            else
                expected("Compatible register(pair) in argument 1");
        }
    }
    else if(op == "lda")
    {
        if(subTokenLength != 1)
            error("Wrong number of arguments.");
        else
        {
            if(isXBitHex(subTokens[0].value, 16, op))
            {
                subTokens[0].td = TD_BIG_NUM;
                return true;
            }
        }
    }
    else if(op == "sta")
    {
        if(subTokenLength != 1)
            error("Wrong number of arguments.");
        else
        {
            if(isXBitHex(subTokens[0].value, 16, op))
            {
                subTokens[0].td = TD_BIG_NUM;
                return true;
            }
        }
    }
    else if(op == "lhld")
    {
        if(subTokenLength != 1)
            error("Wrong number of arguments.");
        else
        {
            if(isXBitHex(subTokens[0].value, 16, op))
            {
                subTokens[0].td = TD_BIG_NUM;
                return true;
            }
        }
    }
    else if(op == "shld")
    {
        if(subTokenLength != 1)
            error("Wrong number of arguments.");
        else
        {
            if(isXBitHex(subTokens[0].value, 16, op))
            {
                subTokens[0].td = TD_BIG_NUM;
                return true;
            }
        }
    }
    else if(op == "stax")
    {
        if(subTokenLength != 1)
            error("Wrong number of arguments.");
        else
        {
            if(isRegPair(subTokens[0].value))
            {
                subTokens[0].td = TD_STR;
                return true;
            }
            else
                expected("Valid Register");
        }
    }
    else if(op == "xchg")
    {
        if(subTokenLength != 0)
            error("Wrong number of arguments.");
        else
            return true;
    }
    //LOAD AND STORE---------------------------------

    //ARITHMATIC-------------------------------------
    else if(op == "add")
    {
        if(subTokenLength != 1)
            error("Wrong number of arguments.");
        else
        {
            std::string oper = subTokens[0].value;

            if(isRegisterOrMemory(oper))
            {
                subTokens[0].td = TD_STR;
                return true;
            }
            else
                expected("Register or memory");
        }
    }
    else if(op == "adi")
    {
        if(subTokenLength != 1)
            error("Wrong number of arguments.");
        else
        {
            if(isXBitHex(subTokens[0].value, 8, op))
            {
                subTokens[0].td = TD_NUM;
                return true;
            }
        }
    }
    else if(op == "sub")
    {
        if(subTokenLength != 1)
            error("Wrong number of arguments.");
        else
        {
            std::string oper = subTokens[0].value;

            if(isRegisterOrMemory(oper))
            {
                subTokens[0].td = TD_STR;
                return true;
            }
            else
                expected("Register or memory");
        }
    }
    else if(op == "inr")
    {
        if(subTokenLength != 1)
            error("Wrong number of arguments.");
        else
        {
            std::string oper = subTokens[0].value;

            if(isRegisterOrMemory(oper))
            {
                subTokens[0].td = TD_STR;
                return true;
            }
            else
                expected("Register or memory");
        }
    }
    else if(op == "dcr")
    {
        if(subTokenLength != 1)
            error("Wrong number of arguments.");
        else
        {
            std::string oper = subTokens[0].value;

            if(isRegisterOrMemory(oper))
            {
                subTokens[0].td = TD_STR;
                return true;
            }
            else
                expected("Register or memory");
        }
    }
    else if(op == "inx")
    {
        if(subTokenLength != 1)
            error("Wrong number of arguments.");
        else
        {
            std::string oper = subTokens[0].value;

            if(isRegPair(oper))
            {
                subTokens[0].td = TD_STR;
                return true;
            }
            else
                expected("Register or memory");
        }
    }
    else if(op == "dcx")
    {
        if(subTokenLength != 1)
            error("Wrong number of arguments.");
        else
        {
            std::string oper = subTokens[0].value;

            if(isRegPair(oper))
            {
                subTokens[0].td = TD_STR;
                return true;
            }
            else
                expected("Register or memory");
        }
    }
    else if(op == "dad")
    {
        if(subTokenLength != 1)
            error("Wrong number of arguments.");
        else
        {
            std::string oper = subTokens[0].value;

            if(isRegPair(oper))
            {
                subTokens[0].td = TD_STR;
                return true;
            }
            else
                expected("Register or memory");
        }
    }
    else if(op == "sui")
    {
        if(subTokenLength != 1)
            error("Wrong number of arguments.");
        else
        {
            if(isXBitHex(subTokens[0].value, 8, op))
            {
                subTokens[0].td = TD_NUM;
                return true;
            }
        }
    }
    //ARITHMATIC-------------------------------------

    //LOGICAL----------------------------------------
    else if(op == "cma")
    {
        if(subTokenLength != 0)
            error("Wrong number of arguments.");
        else
            return true;
    }
    else if(op == "cmp")
    {
        if(subTokenLength != 1)
            error("Wrong number of arguments.");
        else
        {
            std::string oper = subTokens[0].value;

            if(isRegisterOrMemory(oper))
            {
                subTokens[0].td = TD_STR;
                return true;
            }
            else
                expected("Register or memory");
        }
    }
    //LOGICAL----------------------------------------

    //BRANCHING--------------------------------------
    else if(op == "jmp")
    {
        if(subTokenLength != 1)
            error("Wrong number of arguments.");
        else
        {
            if(isXBitHex(subTokens[0].value, 16, op))
            {
                subTokens[0].td = TD_BIG_NUM;
                return true;
            }
        }
    }
    else if(op == "jc")
    {
        if(subTokenLength != 1)
            error("Wrong number of arguments.");
        else
        {
            if(isXBitHex(subTokens[0].value, 16, op))
            {
                subTokens[0].td = TD_BIG_NUM;
                return true;
            }
        }
    }
    else if(op == "jnc")
    {
        if(subTokenLength != 1)
            error("Wrong number of arguments.");
        else
        {
            if(isXBitHex(subTokens[0].value, 16, op))
            {
                subTokens[0].td = TD_BIG_NUM;
                return true;
            }
        }
    }
    else if(op == "jz")
    {
        if(subTokenLength != 1)
            error("Wrong number of arguments.");
        else
        {
            if(isXBitHex(subTokens[0].value, 16, op))
            {
                subTokens[0].td = TD_BIG_NUM;
                return true;
            }
        }
    }
    else if(op == "jnz")
    {
        if(subTokenLength != 1)
            error("Wrong number of arguments.");
        else
        {
            if(isXBitHex(subTokens[0].value, 16, op))
            {
                subTokens[0].td = TD_BIG_NUM;
                return true;
            }
        }
    }
    //BRANCHING--------------------------------------

    else if(op == "set")
    {
        if(subTokenLength != 2)
            error("Wrong number of arguments.");
        else if(isXBitHex(subTokens[0].value, 16, op))
        {
            if(isXBitHex(subTokens[1].value, 8, op))
            {
                subTokens[0].td = TD_BIG_NUM;
                subTokens[1].td = TD_NUM;
                return true;
            }
        }
    }
    else if(op == "hlt")
    {
        if(subTokenLength != 0)
            error("Wrong number of arguments.");
        else
            return true;
    }

    return false;
}

std::string trim(char in[100])
{
    int i = 0;

    for(int i = 0; i < 100; i++)
    {
        if(in[i] == 10 || in[i] == 13 || in[i] == 15)
        {
            in[i] = (char)0;
            break;
        }

        in[i] = tolower(in[i]);
    }

    return std::string(in);
}
