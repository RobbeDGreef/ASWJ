#include <stlparser/stlparser.h>

StlParser::StlParser(std::string file)
{
    m_stlfile.open(file, std::ios::binary);
    LOG("Opened file " << file);
}

StlParser::~StlParser()
{
    m_stlfile.close();
}
