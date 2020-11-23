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
        m_stlfile.read((char*) &(m_facet_array[i]), FACET_STRUCT_SIZE);
        m_facet_array[i].calc_z_minmax();
    
        // See Facet::calc_z_minmax for an explanation, in short, 
        // we cannot know the initial values for these variables and need
        // to make sure that they are set correctly.
        if (i == 0)
        {
            m_min_z = m_facet_array[i].min_z;
            m_max_z = m_facet_array[i].max_z;
        } else 
        {
            if (m_facet_array[i].min_z < m_min_z)
                m_min_z = m_facet_array[i].min_z;
            
            if (m_facet_array[i].max_z > m_max_z)
                m_max_z = m_facet_array[i].max_z;
        }
    }
}
    }
}