#define PEEKSIZE 8
#include "comm.h"

class instruction1
{
public:
    unsigned char bytes[16];
    int size;
    string opr, opnd;
};