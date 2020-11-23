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

void StlParser::slice()
{
    // This algorithm is O(n*m) where is n is the amount of facets
    // and m the height of the object divided by the layer height. 
    // Thats bad. However we will focus on a better solution later.
    
    // We create a vector of lists of lines to hold the layer data.
    // For the amount of layers we choose a vector because we already know
    // how large this array will be and random access is nice. We don't know
    // how many lines it will hold though and using a vector here would
    // be very inefficient.
    //
    // The amount of layers is equal to the height of the object divided by the
    // layer height rounded up. (+1 because we use <= in the loop instead of <
    // and don't want any segfaults)
    std::vector<std::list<Line>> layers(ceil((m_max_z - m_min_z) / m_layer_height)+1);

    int i_layer = 0;
    for (float height = m_min_z; height <= m_max_z; height += m_layer_height)
    {
        for (Facet facet : m_facet_array)
        {
            // If the height is in range of the facets min and max height
            // try and find the intersection points.
            if (facet.min_z <= height && facet.max_z >= height)
            {
                // The three lines every triangle consists of
                Line lines[3] = {
                    Line(facet.vertices[0], facet.vertices[1]),
                    Line(facet.vertices[1], facet.vertices[2]),
                    Line(facet.vertices[2], facet.vertices[0]),
                };

                // We keep track of the intersecting points in this vector
                std::vector<vec3f> intersections;
                for (int i = 0; i < 3; i++)
                {
                    if (lines[i].contains_height(height))
                    {
                        intersections.push_back(lines[i].calc_point_from_z(height));
                    }
                }
                
                // If only one point intersects the z plane, we ignore it
                // because this means we cannot create a line. And printing
                // a single point is useless.
                //
                // We don't handle any cases where all 3 points intersect because
                // we already check if this is the case in Line::contains_height()
                // and if so, discard the case as false.
                // meaning we can never reach the point where 3 vertices intersect.

                if (intersections.size() == 2)
                    layers[i_layer].push_back(Line(intersections[0], intersections[1]));
            }
        }
        i_layer++;
    }

    // Now lets print out layers to inspect the output   
    for (int i = 0; i < layers.size(); i++)
    {
        LOG("Layer: " << i);
        for (Line line : layers[i])
        {
            LOG("intersection line: " << line.to_string());
        }
    }

    // Because this data is not very visible i wrote a script to dump this to
    // files and display the data using python.
    debug_layers_to_file(layers);
}