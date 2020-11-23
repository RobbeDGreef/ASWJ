#include <stlparser/stlparser.h>

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        ERROR("No input files", 1);
    }

    std::string s(argv[1]);
    StlParser parser(s);
    parser.parse();
    parser.slice();
}
