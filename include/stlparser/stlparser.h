#pragma once

#include <core.h>

class StlParser
{
private:
    std::ifstream m_stlfile;
public:
    StlParser(std::string file);
    ~StlParser();
};
