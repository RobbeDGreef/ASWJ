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