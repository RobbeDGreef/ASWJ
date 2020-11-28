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
        
        if (m_facet_array[i].min_z < m_min_z)
            m_min_z = m_facet_array[i].min_z;
    }

    apply_transform();
}

std::vector<Layer> &StlParser::slice()
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
    m_layers = std::vector<Layer>(ceil(m_object_height / m_layer_height)+1);

    int i_layer = 0;
    for (float height = 0; height <= m_object_height; height += m_layer_height)
    {
        for (Facet facet : m_facet_array)
        {
            // If the height is in range of the facets min and max height
            // try and find the intersection points.
            // Accounting for floating point precision is needed here as well.
            if (facet.min_z <= (height + COMP_PRECISION) && facet.max_z >= (height - COMP_PRECISION))
            {              
                // The three lines every triangle consists of
                Line lines[3] = {
                    Line(facet.vertices[0], facet.vertices[1]),
                    Line(facet.vertices[1], facet.vertices[2]),
                    Line(facet.vertices[2], facet.vertices[0]),
                };

                // We keep track of the intersecting points in this vector
                std::vector<Vec3f> intersections;
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

                if (intersections.size() == 2 && intersections[0] != intersections[1])
                    m_layers[i_layer].insert(Line(intersections[0], intersections[1]));
            }
        }
        i_layer++;
    }

    return m_layers;
}

void StlParser::apply_transform()
{
    m_offset.z = -m_min_z * m_scale.z;
    LOG("min_z: " << m_min_z);
    LOG("offset: " << m_offset.to_string());
    LOG("scale: " << m_scale.to_string());

    for (Facet &facet : m_facet_array)
    {
        for (int i = 0; i < 3; i++)
        {
            facet.vertices[i].transform(m_offset, m_scale);

            if (facet.vertices[i].z > m_object_height)
                m_object_height = facet.vertices[i].z;
        }
        facet.calc_z_minmax();
    }
    LOG("Object height: " << m_object_height);
}