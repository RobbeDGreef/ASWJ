#pragma once

#include <core.h>
#include <vec3f.h>

// The struct will hold 4 * 4 floats = 48 bytes + 2 attribute bytes 
// giving us a total of 50 bytes.
#define FACET_STRUCT_SIZE 50

// Simple stl facet structure
class Facet
{
public:
    // The 
    struct vec3f normal;
    struct vec3f vertices[3];
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
        {
            LOG("Vert " << i << ": " << vertices[i].to_string());
        }

    }

class StlParser
{
private:
    std::ifstream m_stlfile;
    std::vector<Facet> m_facet_array;

    // These will hold the minimum and maximum z locations
    // of the object.
    float m_min_z;
    float m_max_z;

    float m_layer_height = 0.2;

public:
    StlParser(std::string file);
    ~StlParser();

    void parse();
    void slice();
};
