#pragma once

#include <core.h>
#include <vec3f.h>

// Simple stl facet structure
struct Facet
{
public:
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
}__attribute__((packed));

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
