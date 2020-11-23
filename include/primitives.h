#pragma once

#include <core.h>

class Vec3f
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
    Vec3f p1;
    Vec3f p2;

    float m_min_z;
    float m_max_z;

public:
    Line() {}
    Line(Vec3f p1, Vec3f p2);

    std::string to_string() { return "(" + p1.to_string() + ") (" + p2.to_string() + ")"; }

    bool contains_height(float height);
    Vec3f calc_point_from_z(float z);
};