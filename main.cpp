#include <iostream>
#include "CommandProcessor.hpp"
#include <sstream>

int main()
{
    CommandProcessor cp;
    while (cp.getEndProgram() == false)
    {   
        std::string input;
        std::getline(std::cin, input);
        cp.processInput(input);
        
    }
    std::cout << "GOODBYE" << std::endl;

    return 0;
}

