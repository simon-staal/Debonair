#include <fstream>
#include <iostream>
#include <string>

int main()
{
    std::ifstream is ("examples/ca.crt");
    if(!is.is_open()){
        std::cerr << "Unable to open file" << std::endl;
        exit(1);
    }
    std::string buffer;
    while(!is.eof()){
        std::getline(is, buffer);
        std::cout << '"' << buffer << "\\n\" \\" << std::endl;
    }

    is.close();
}