#pragma once

#include <core.h>

struct vec3f
{
public:
    float x;
    float y;
    float z;


public:
    std::string to_string() 
    {
        return std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z);   
    }

};

class Line
{
public:
    struct vec3f p1;
    struct vec3f p2;

    float m_min_z;
    float m_max_z;

public:
    Line() {}
    Line(vec3f p1, vec3f p2);

    std::string to_string() { return "(" + p1.to_string() + ") (" + p2.to_string() + ")"; }

    bool contains_height(float height);
    vec3f calc_point_from_z(float z);
};