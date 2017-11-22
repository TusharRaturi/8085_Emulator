#include "memory.h"
#include "instructions.h"
#include "memElement.h"
#include "printer.h"

std::string getPrintableHex(int in)
{
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << std::hex << in;

    std::string a = ss.str();
    reverse(a.begin(), a.end());

    a = a.substr(0, 2);
    reverse(a.begin(), a.end());

    return a;
}

void printAllRegistersAndFlags()
{
    std::cout << std::endl << "Register values ->" << std::endl << std::endl;

    std::cout << "a = " << getPrintableHex(A) << std::endl;
    std::cout << "b = " << getPrintableHex(B) << std::endl;
    std::cout << "c = " << getPrintableHex(C) << std::endl;
    std::cout << "d = " << getPrintableHex(D) << std::endl;
    std::cout << "e = " << getPrintableHex(E) << std::endl;
    std::cout << "h = " << getPrintableHex(H) << std::endl;
    std::cout << "l = " << getPrintableHex(L) << std::endl;

    std::cout << std::endl << "Flag values ->" << std::endl << std::endl;
    std::cout << "S  = " << fS << std::endl;
    std::cout << "Z  = " << fZ << std::endl;
    std::cout << "AC = " << fAC << std::endl;
    std::cout << "P  = " << fP << std::endl;
    std::cout << "C  = " << fC << std::endl;

    std::cout << std::endl;
}

void displayHelpInfo()
{
    setDelay(10000000);
    write("Commands:\nType r to run till the next break point is encountered or the program ends.\n");
    write("Type b and then lines numbers on which breakpoints are to be put. Eg: \"b 5 6\" will put break points on lines 5 and 6.\n");
    write("Press enter to step a program when a breakpoint is encountered or at the start of the program.\n");
    setDelay(20000000);
}

void debugProg(int PC, int addressSpace)
{
    std::cout << "PC = " << intToHex(PC, 4) << std::endl;

    if(PC == getInitAddress())
    {
        printAllRegistersAndFlags();
        displayUsedMem();
    }

    if(mainMem[PC].brkPt || step)
    {
        step = false;
        std::string in;

        bool correct = false;

        while(!correct)
        {
            std::cout << "8085#DEBUG>> ";
            getline(std::cin, in);

            if(in == "r")
            {
                system("cls");
                break;
            }
            else if(in == "p")
            {
                printAllRegistersAndFlags();
                displayUsedMem();
            }
            else if(in == "")
            {
                step = true;
                system("cls");
                break;
            }
            else if(in[0] == 'b' && in[1] == ' ')
            {
                TaggedSubtokens ts[10];
                int len;
                extractDelimited(ts, in, " ", &len);

                for(int i = 1; i < len; i++)
                {
                    int line = strToInt(ts[i].value);

                    for(int j = getInitAddress(); j < getInitAddress() + addressSpace;)
                    {
                        if(mainMem[j].lineNo == line)
                            mainMem[j].brkPt = true;
                        j += mainMem[j].pcInc;
                    }
                }
            }
            else if(in == "help")
                displayHelpInfo();

            else if(in == "x")
                exit(0);
            else
                std::cout << "Invalid input." << std::endl;
        }
    }

    if(step == false)
        system("cls");
}

bool executeAProgramLine(int &PC, int addressSpace)
{
    if(debug)
        debugProg(PC, addressSpace);

    std::string op = mainMem[PC].sdata;

    bool jmpInst = false;

    //LOAD AND STORE---------------------------------
    if(op == "mov")
    {
        std::string oper1 = mainMem[PC].extData[0];
        std::string oper2 = mainMem[PC].extData[1];

        mov(oper1, oper2);
    }
    else if(op == "mvi")
    {
        std::string oper = mainMem[PC].extData[0];
        int tmp = strHexToDec(mainMem[PC + 1].sdata);

        if(oper == "a")
            A = tmp;
        else if(oper == "b")
            B = tmp;
        else if(oper == "c")
            C = tmp;
        else if(oper == "d")
            D = tmp;
        else if(oper == "e")
            E = tmp;
        else if(oper == "h")
            H = tmp;
        else if(oper == "l")
            L = tmp;
        else if(oper == "m")
        {
            int ad = strHexToDec(outPlaceAppend(intToHex(H, 2), intToHex(L, 2)));
            mainMem[ad].sdata = intToHex(tmp);
            mainMem[ad].used = true;
        }
    }
    else if(op == "lxi")
    {
        int tmp1 = strHexToDec(mainMem[PC + 1].sdata);
        int tmp2 = strHexToDec(mainMem[PC + 2].sdata);

        std::string oper = mainMem[PC].extData[0];

        if(oper == "b")
        {
            B = tmp1;
            C = tmp2;
        }
        else if(oper == "d")
        {
            D = tmp1;
            E = tmp2;
        }
        else if(oper == "h")
        {
            H = tmp1;
            L = tmp2;
        }
    }
    else if(op == "lda")
        A = strHexToDec(mainMem[strHexToDec(outPlaceAppend(mainMem[PC + 1].sdata, mainMem[PC + 2].sdata))].sdata);
    else if(op == "sta")
    {
        mainMem[strHexToDec(outPlaceAppend(mainMem[PC + 1].sdata, mainMem[PC + 2].sdata))].sdata = intToHex(A);
        mainMem[strHexToDec(outPlaceAppend(mainMem[PC + 1].sdata, mainMem[PC + 2].sdata))].used = true;
    }
    else if(op == "lhld")
    {
        int tmp = strHexToDec(outPlaceAppend(mainMem[PC + 1].sdata, mainMem[PC + 2].sdata));

        L = strHexToDec(mainMem[tmp].sdata);
        H = strHexToDec(mainMem[tmp + 1].sdata);
    }
    else if(op == "shld")
    {
        int tmp = strHexToDec(outPlaceAppend(mainMem[PC + 1].sdata, mainMem[PC + 2].sdata));

        mainMem[tmp].used = true;
        mainMem[tmp].sdata = intToHex(L);
        if(tmp < 65535)
            mainMem[tmp + 1].sdata = intToHex(H);
        else
            mainMem[0].sdata = intToHex(H);
    }
    else if(op == "stax")
    {
        int tmp = 0;

        if(mainMem[PC].extData[0] == "b")
            tmp = staxGetLoc(B, C);
        else if(mainMem[PC].extData[0] == "d")
            tmp = staxGetLoc(D, E);
        else if(mainMem[PC].extData[0] == "h")
            tmp = staxGetLoc(H, L);

        mainMem[tmp].sdata = intToHex(A);
        mainMem[tmp].used = true;
    }
    else if(op == "xchg")
    {
        swap(&H, &D);
        swap(&L, &E);
    }
    //LOAD AND STORE---------------------------------

    //ARITHMATIC-------------------------------------
    else if(op == "add")
    {
        std::string oper = mainMem[PC].extData[0];

        if(oper == "a")
            A = A + A;
        else if(oper == "b")
            A = A + B;
        else if(oper == "c")
            A = A + C;
        else if(oper == "d")
            A = A + D;
        else if(oper == "e")
            A = A + E;
        else if(oper == "h")
            A = A + H;
        else if(oper == "l")
            A = A + L;
        else if(oper == "m")
            A = A + strHexToDec(mainMem[strHexToDec(intToHex(H, 2, L, 2))].sdata);

        setFlags(A, true);
    }
    else if(op == "adi")
    {
        int tmp = strHexToDec(mainMem[PC + 1].sdata);

        A = A + tmp;
        setFlags(A, true);
    }
    else if(op == "sub")
    {
        std::string oper = mainMem[PC].extData[0];

        if(oper == "a")
            A = A - A;
        else if(oper == "b")
            A = A - B;
        else if(oper == "c")
            A = A - C;
        else if(oper == "d")
            A = A - D;
        else if(oper == "e")
            A = A - E;
        else if(oper == "h")
            A = A - H;
        else if(oper == "l")
            A = A - L;
        else if(oper == "m")
            A = A - strHexToDec(mainMem[strHexToDec(intToHex(H, 2, L, 2))].sdata);

        setFlags(A, true);
    }
    else if(op == "inr")
    {
        std::string oper = mainMem[PC].extData[0];

        if(oper == "a")
            setFlags(++A, false);
        else if(oper == "b")
            setFlags(++B, false);
        else if(oper == "c")
            setFlags(++C, false);
        else if(oper == "d")
            setFlags(++D, false);
        else if(oper == "e")
            setFlags(++E, false);
        else if(oper == "h")
            setFlags(++H, false);
        else if(oper == "l")
            setFlags(++L, false);
        else if(oper == "m")
        {
            int memo = strHexToDec(mainMem[strHexToDec(intToHex(H, 2, L, 2))].sdata);
            mainMem[strHexToDec(intToHex(H, 2, L, 2))].sdata = intToHex(++memo);
            setFlags(memo, false);
        }
    }
    else if(op == "dcr")
    {
        std::string oper = mainMem[PC].extData[0];

        if(oper == "a")
            setFlags(--A, false);
        else if(oper == "b")
            setFlags(--B, false);
        else if(oper == "c")
            setFlags(--C, false);
        else if(oper == "d")
            setFlags(--D, false);
        else if(oper == "e")
            setFlags(--E, false);
        else if(oper == "h")
            setFlags(--H, false);
        else if(oper == "l")
            setFlags(--L, false);
        else if(oper == "m")
        {
            int memo = strHexToDec(mainMem[strHexToDec(intToHex(H, 2, L, 2))].sdata);
            mainMem[strHexToDec(intToHex(H, 2, L, 2))].sdata = intToHex(--memo);
            setFlags(memo, false);
        }
    }
    else if(op == "inx")
    {
        std::string oper = mainMem[PC].extData[0];

        if(oper == "b")
        {
            C++;
            if(C > 255)
            {
                C--;
                B++;
            }
            if(B > 255)
                B--;
        }
        else if(oper == "d")
        {
            E++;
            if(E > 255)
            {
                E--;
                D++;
            }
            if(D > 255)
                D--;
        }
        else if(oper == "h")
        {
            L++;
            if(L > 255)
            {
                L--;
                H++;
            }
            if(H > 255)
                H--;
        }
    }
    else if(op == "dcx")
    {
        std::string oper = mainMem[PC].extData[0];

        if(oper == "b")
        {
            C--;
            if(C < -256)
            {
                C++;
                B--;
            }
            if(B < -256)
            {
                B = 0;
                C = 0;
            }
        }
        else if(oper == "d")
        {
            E--;
            if(E < -256)
            {
                E++;
                D--;
            }
            if(D < -256)
            {
                D = 0;
                E = 0;
            }
        }
        else if(oper == "h")
        {
            L--;
            if(L < -256)
            {
                L++;
                H--;
            }
            if(H < -256)
            {
                H = 0;
                L = 0;
            }
        }
    }
    else if(op == "dad")
    {
        std::string oper = mainMem[PC].extData[0];

        int tmp1, tmp2;

        if(oper == "b")
        {
            tmp1 = strHexToDec(intToHex(H, 2, L, 2));
            tmp2 = strHexToDec(intToHex(B, 2, C, 2));
        }
        else if(oper == "d")
        {
            tmp1 = strHexToDec(intToHex(H, 2, L, 2));
            tmp2 = strHexToDec(intToHex(D, 2, E, 2));
        }
        else if(oper == "h")
            tmp1 = tmp2 = strHexToDec(intToHex(H, 2, L, 2));

        dad(tmp1, tmp2);
    }
    else if(op == "sui")
    {
        int tmp = strHexToDec(mainMem[PC + 1].sdata);

        A = A - tmp;
        setFlags(A, false);
    }
    //ARITHMATIC-------------------------------------

    //LOGICAL----------------------------------------
    else if(op == "cma")
        A = ~A;
    else if(op == "cmp")
    {
        std::string oper = mainMem[PC].extData[0];

        if(oper == "a")
            fZ = 1;
        else if(oper == "b")
        {
            if(A < B)
                fC = 1;
            else if(A == B)
                fZ = 1;
            else
                fC = fZ = 0;
        }
        else if(oper == "c")
        {
            if(A < C)
                fC = 1;
            else if(A == C)
                fZ = 1;
            else
                fC = fZ = 0;
        }
        else if(oper == "d")
        {
            if(A < D)
                fC = 1;
            else if(A == D)
                fZ = 1;
            else
                fC = fZ = 0;
        }
        else if(oper == "e")
        {
            if(A < E)
                fC = 1;
            else if(A == E)
                fZ = 1;
            else
                fC = fZ = 0;
        }
        else if(oper == "h")
        {
            if(A < H)
                fC = 1;
            else if(A == H)
                fZ = 1;
            else
                fC = fZ = 0;
        }
        else if(oper == "l")
        {
            if(A < L)
                fC = 1;
            else if(A == L)
                fZ = 1;
            else
                fC = fZ = 0;
        }
        else if(oper == "m")
        {
            int tmp = strHexToDec(mainMem[strHexToDec(intToHex(H, 2, L, 2))].sdata);

            if(A < tmp)
                fC = 1;
            else if(A == tmp)
                fZ = 1;
            else
                fC = fZ = 0;
        }
    }
    //LOGICAL----------------------------------------

    //BRANCHING--------------------------------------
    else if(op == "jmp")
    {
        PC = strHexToDec(outPlaceAppend(mainMem[PC + 1].sdata, mainMem[PC + 2].sdata));
        jmpInst = true;
    }
    else if(op == "jc")
    {
        if(fC == 1)
        {
            PC = strHexToDec(outPlaceAppend(mainMem[PC + 1].sdata, mainMem[PC + 2].sdata));
            jmpInst = true;
        }
    }
    else if(op == "jnc")
    {
        if(fC == 0)
        {
            PC = strHexToDec(outPlaceAppend(mainMem[PC + 1].sdata, mainMem[PC + 2].sdata));
            jmpInst = true;
        }
    }
    else if(op == "jz")
    {
        if(fZ == 1)
        {
            PC = strHexToDec(outPlaceAppend(mainMem[PC + 1].sdata, mainMem[PC + 2].sdata));
            jmpInst = true;
        }
    }
    else if(op == "jnz")
    {
        if(fZ == 0)
        {
            PC = strHexToDec(outPlaceAppend(mainMem[PC + 1].sdata, mainMem[PC + 2].sdata));
            jmpInst = true;
        }
    }
    //BRANCHING--------------------------------------

    else if(op == "set")
    {
        mainMem[strHexToDec(outPlaceAppend(mainMem[PC + 1].sdata, mainMem[PC + 2].sdata))].sdata = mainMem[PC + 3].sdata;
        mainMem[strHexToDec(outPlaceAppend(mainMem[PC + 1].sdata, mainMem[PC + 2].sdata))].used = true;
    }
    else if(op == "hlt")
    {
        printAllRegistersAndFlags();
        displayUsedMem();
        return true;
    }
    else
        error(std::string("Undefined Command").append(" at ").append(intToHex(PC)).append(": ").append(op).append(" ").append(mainMem[PC].extData[0]).append(" ").append(mainMem[PC].extData[1]));

    if(!jmpInst)
        PC += mainMem[PC].pcInc;

    if(debug)
    {
        printAllRegistersAndFlags();
        displayUsedMem();
    }

    return false;
}

int main(int argc, char** argv)
{
    write("WELCOME TO THE 8085 EMULATOR.\n");

    char *filename = new char[100];
    debug = false;
    std::string inputString;

    if(argc == 1)
    {
        bool correct = false;

        while(!correct)
        {
            write("\nDo you want to load a program from a file?(y/n) ");

            std::string line = "";
            getline(std::cin, line);

            if(line == "y")
            {
                write("Enter the file name: ");
                std::cin.getline(filename, 100);
                correct = true;
            }
            else if(line == "n")
            {
                correct = true;
                filename = "";
            }
            else
                error("Wrong Input");
        }

        correct = false;

        while(!correct)
        {
            write("Do you want to debug?(y/n) ");

            std::string line = "";
            getline(std::cin, line);

            if(line == "y")
            {
                debug = true;
                correct = true;
            }
            else if(line == "n")
                correct = true;
            else
                error("Wrong Input");
        }
    }
    else if(argc == 2)
    {
        std::string x(argv[1]);

        if(x == "-h")
        {
            write("\nCommand line argument format:-\n");
            write("\nFormat 1 => \"<exe path>\" <-This will run the program in normal mode.(It will ask for file input - if any - and debugger also).\n");
            write("\nFormat 2 => \"<exe path> -h\" <-This will print this help text.\n");
            write("\nFormat 3 => \"<exe path> -d\" <-This will enable the debugger but will also ask for the filename(if any).\n");
            write("\nFormat 4 => \"<exe path> <file path>\" <-This will input the file from the provided path but the debugger will be disabled.\n");
            write("\nFormat 5 => \"<exe path> <file path> -d\" <-This will input the file from the provided path and will enable the debugger.\n");

            std::cout << std::endl << "Press Enter to exit." << std::endl;
            getline(std::cin, inputString);

            exit(0);
        }

        if(x == "-d")
        {
            debug = true;

            bool correct = false;

            while(!correct)
            {
                write("Do you want to load a program from a file?(y/n) ");

                std::string line = "";
                getline(std::cin, line);

                if(line == "y")
                {
                    write("Enter the file name: ");
                    std::cin.getline(filename, 100);
                    correct = true;
                }
                else if(line == "n")
                {
                    correct = true;
                    filename = "";
                }
                else
                    error("Wrong Input");
            }
        }
        else
            filename = argv[1];
    }
    else if(argc == 3)
    {
        filename = argv[1];
        if(std::string(argv[2]) == "-d")
            debug = true;
        else
            std::cout << "Wrong argument." << std::endl;
    }

    write("Enter the initial address: ");
    getline(std::cin, inputString);

    int address = strHexToDec(inputString);
    int addressSpace = 0;

    if(filename == "")
        inputProgram(address, addressSpace);
    else
        if(!loadProgram(filename, address, addressSpace))
            inputProgram(address, addressSpace);

    int PC = address;

    if(debug)
    {
        write("\nMemory: \n\n");
        char *ad = new char[100];
        itoa(addressSpace, ad, 10);
        write(std::string("Space Occupied by program: ").append(ad).append(" bytes").append("\n\n"));
        delay();
        displayMem(intToHex(address), intToHex(address + addressSpace));
        std::cout << std::endl;
    }

    bool halt = false;

    if(debug)
        mainMem[strHexToDec(inputString)].brkPt = true;

    while(!halt)
        halt = executeAProgramLine(PC, addressSpace);

    std::cout << "Press Enter to exit." << std::endl;
    getline(std::cin, inputString);

    return 0;
}
