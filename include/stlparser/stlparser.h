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

public:
    void print()
    {
        LOG("Normal: " << normal.to_string());
        for (int i = 0; i < 3; i++)
        {
            LOG("Vert " << i << ": " << vertices[i].to_string());
        }

    }

class StlParser
{
private:
    std::ifstream m_stlfile;
    std::vector<Facet> m_facet_array;

public:
    StlParser(std::string file);
    ~StlParser();

    void parse();
};
