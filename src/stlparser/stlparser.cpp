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

void StlParser::parse()
{
    LOG("parsing file");
    // The first 80 bytes are a unimportant ascii, however
    // for testing we will print it anyway

    char header[80];
    m_stlfile.read(header, 80);
    LOG("File header reads: '" << header << "'");
    
    // Read the facet count.
    uint32_t facet_count;
    m_stlfile.read((char*) &facet_count, 4);

    LOG("Facet count: " << facet_count);

    // Initialize the facet array that will be used to keep all the facets.
    m_facet_array = std::vector<Facet>(facet_count);

    for (uint i = 0; i < facet_count; i++)
    {
        m_stlfile >> (char*) &(m_facet_array[i]);
        m_facet_array[i].print();
    }
}