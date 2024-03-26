#include “DebugHdr.h”
int main()
{
    int a, b;
    @var('a')
    @var('b')
    b=0;
    a=1;

    while(b<10)
    {
        a=a+b;
        b=b+1;
        @trace('b');
    }
    return 0;
}

