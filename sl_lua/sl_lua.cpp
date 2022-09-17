#include <iostream>
#include"stdio.h"
struct A{
    char a;
    char b;
};
union B{
    A a;
    int t;
};
int main(int argc, char* argv[])
{
    B bb;
    bb.t=0;
    bb.a.a=0x12;
    bb.a.b=0x22;
    printf("%x\n",bb.t);
    std::cout<<bb.t<<std::endl;
    std::cout<<"hello world"<<std::endl;
    return 0;
}
