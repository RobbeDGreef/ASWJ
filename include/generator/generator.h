#pragma once

#include <core.h>
#include <layer.h>
#include <primitives.h>

#define MM_PER_MIN(x) x*60

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

    float m_retraction_length = 0.8f;
    float m_retraction_lift = 0.6f;
    float m_retraction_speed = MM_PER_MIN(35);
    float m_line_thickness = 0.7f;
    
private:
    // This function will add the boilerplate codes (set temperature, homing etc)
    void gen_begin_of_gcode();
    void gen_end_of_gcode();
    void gen_layer_gcode(float motor_speed, float extrusion_rat, std::vector<std::list<Vec3f>> lines,
                         int layer, Vec3f lastpos);
    void gen_move_to(Vec3f p, int layer, Vec3f lastpos);
    void gen_retraction(float height);

    void calc_infill_points(std::list<Vec3f> &points, Layer layer, 
                                                   float interval, 
                                                   float& (*get_X)(Vec3f&),
                                                   float& (*get_Y)(Vec3f&),
                                                   Vec3f make_vec(float x, float y),
                                                   float linewidth);

    void calc_infill_flows(std::vector<std::list<Vec3f>> &flows, Layer lines, float interval_size,
                           bool cross=false);
    std::vector<std::list<Vec3f>> calc_flows(Layer lines);

public:
    Generator(int temperature, int bed_temp, float layer_height);
    void generate(std::vector<Layer> layers);
    void write_to_file(std::string file);
};