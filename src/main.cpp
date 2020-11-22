#include <stlparser/stlparser.h>

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        ERROR("No input files", 1);
    }
    
    StlParser(std::string(argv[1]));
}
