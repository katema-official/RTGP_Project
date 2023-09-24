#include <iostream>

#include <main1.h>
#include <main2.h>

int MAIN_CHOSEN = 2;

int main()
{
    if(MAIN_CHOSEN == 1)
    {
        std::cout << "Running nodes visualizer..." << std::endl;
        MAIN1::main1();
    }
    if(MAIN_CHOSEN == 2)
    {
        std::cout << "Running tree visualizer..." << std::endl;
        MAIN2::main2();
    }
    
    
    return 0;
}
