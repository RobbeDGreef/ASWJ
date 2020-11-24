#pragma once

#include <core.h>
#include <primitives.h>

// The struct will hold 4 * 4 floats = 48 bytes + 2 attribute bytes 
// giving us a total of 50 bytes.
#define FACET_STRUCT_SIZE 50

// Simple stl facet structure
class Facet
{
public:
    // The 
    Vec3f normal;
    Vec3f vertices[3];
    uint16_t attributes;

    float min_z;
    float max_z;

public:
    // This function will calculate the minumum and maximum z height of the
    // facet. This will later be used to eliminate facets that are not at the 
    // correct slicing height.
    void calc_z_minmax()
    {
        // We need to set the min and max initially because we cannot
        // know the initial value of these variables. The z location
        // can be negative, 0, positive. So we need to make sure the inital
        // value is at least set to the first vert.
        min_z = max_z = vertices[0].z;
        
        for (int i = 1; i < 3; i++)
        {
            if (vertices[i].z < min_z)
                min_z = vertices[i].z;

            if (vertices[i].z > max_z)
                max_z = vertices[i].z;
        }
    }

    std::string to_string()
    {
        return "(" + vertices[0].to_string() + ") (" + vertices[1].to_string() + ") (" + vertices[1].to_string() + ")";
    }
};

class StlParser
{
private:
    std::ifstream m_stlfile;
    std::vector<Facet> m_facet_array;

    float m_min_z = 0;
    float m_object_height = 0;

    float m_layer_height = 0.3;

    Vec3f m_offset = Vec3f(80,80,0);
    Vec3f m_scale = Vec3f(10,10,10);

     std::vector<std::list<Line>> m_layers;

public:
    StlParser(std::string file);
    ~StlParser();

    void parse();
    std::vector<std::list<Line>> &slice();
    void apply_transform();
};
