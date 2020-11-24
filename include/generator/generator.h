#pragma once

#include <core.h>
#include <primitives.h>

class Parameter
{
private:
    char m_letter;
    bool m_is_float;
    union
    {
        float m_fvalue;
        int m_ivalue;
    };

public:
    Parameter(char letter, float val);
    Parameter(char letter, int val);

    std::string to_string();
};

typedef std::vector<Parameter> Code;
std::string code_to_string(Code);

class Generator
{
private:
    std::vector<Code> m_codes;
    int m_temp;
    int m_bed_temp;
    float m_layer_height;
    
private:
    // This function will add the boilerplate codes (set temperature, homing etc)
    void gen_begin_of_gcode();
    void gen_end_of_gcode();
    void gen_layer_gcode(float motor_speed, float extrusion_rat, std::vector<std::list<Vec3f>> lines);
    void gen_move_to(Vec3f p, int layer);

    std::vector<std::list<Vec3f>> calc_flows(std::list<Line> lines);

public:
    Generator(int temperature, int bed_temp, float layer_height);
    void generate(std::vector<std::list<Line>> layers);
    void write_to_file(std::string file);
};