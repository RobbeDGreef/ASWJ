#pragma once

#include <core.h>

// The precision we want in our testing
#define COMP_PRECISION 0.0001f

bool test_float(float x, float y, float epsilon);

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

    bool operator==(const Vec3f other)
    {
        return test_float(x, other.x, COMP_PRECISION) && test_float(y, other.y, COMP_PRECISION) 
            && test_float(z, other.z, COMP_PRECISION);
    }

    bool operator!=(const Vec3f other)
    {
        // TODO: tolerance
        return !test_float(x, other.x, COMP_PRECISION) || !test_float(y, other.y, COMP_PRECISION)
            || !test_float(z, other.z, COMP_PRECISION);
    }

    Vec3f() : x(0), y(0), z(0) {}
    Vec3f(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

    void transform(Vec3f offset, Vec3f scale);
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