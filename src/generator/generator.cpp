#include <generator/generator.h>

Parameter::Parameter(char letter, float val)
{
    m_letter = letter;
    m_fvalue = val;
    m_is_float = true;
}

Parameter::Parameter(char letter, int val)
{
    m_letter = letter;
    m_ivalue = val;
    m_is_float = false;
}

std::string Parameter::to_string()
{
    // We set the precision of the float to 6 because the gcode of
    // prusaslicer does not seem to show any numbers larger than that
    // and im assuming it is the limit.

    std::stringstream stream;
    if (m_is_float)
        stream << m_letter << std::setprecision(6) << m_fvalue;
    else
        stream << m_letter + std::to_string(m_ivalue);

    return stream.str();
}

Generator::Generator(int temp, int bed_temp, float layer_height)
{
    // Reserver at least 500 lines for performance
    m_codes.reserve(500);
    m_temp = temp;
    m_bed_temp = bed_temp;
    m_layer_height = layer_height;
}

std::vector<std::list<Vec3f>> Generator::calc_flows(std::list<Line> lines)
{
    if (lines.size() == 0)
        return {};
    
    // A flow can never be empty.
    std::vector<std::list<Vec3f>> flows(1, std::list<Vec3f>{lines.front().p1, lines.front().p2});
    lines.pop_front();
    int i_flow = 0;
    
    while (lines.size() != 0)
    {
        Vec3f &p = flows[i_flow].back();
        
        for (auto i_line = lines.begin(); i_line != lines.end(); ++i_line)
        {
            Line line = *i_line;
            if (line.p1 == p || line.p2 == p)
            {
                Vec3f &to_push = (line.p1 == p) ? line.p2 : line.p1;
                flows[i_flow].push_back(to_push);
                lines.erase(i_line);
                break;
            }
        }

        // If we did not find an adjacent line
        if (flows[i_flow].back() == p)
        {
            i_flow++;
            flows.push_back(std::list<Vec3f>{lines.front().p1, lines.front().p2});
        }
    }

    return flows;
}

void Generator::gen_begin_of_gcode()
{
    // Turn off the fan for the first layer
    m_codes.push_back({Parameter('M', 107)});

    // Set the extruder temperature and do not wait
    m_codes.push_back({Parameter('M', 104), Parameter('S', m_temp)});

    // Set the bed temperature
    m_codes.push_back({Parameter('M', 140), Parameter('S', m_bed_temp)});
    
    // Wait for the bed temperature
    m_codes.push_back({Parameter('M', 190), Parameter('S', m_bed_temp)});
    
    // Wait for extruder temperature
    m_codes.push_back({Parameter('M', 109), Parameter('S', m_temp)});

    // Home all axis without mesh bed level
    m_codes.push_back({Parameter('G', 28), Parameter('M', 0)});

    // Mesh bed leveling
    m_codes.push_back({Parameter('G', 80)});

    // Typical prusa intro line: 
    m_codes.push_back({Parameter('G', 92)});
    m_codes.push_back({Parameter('G', 1), Parameter('Y', -3.0f), Parameter('F', 1000)});
    m_codes.push_back({Parameter('G', 1), Parameter('X', 100.0f), Parameter('E', 12.0f), Parameter('F', 1000)});
    
    // Lift the nozzle
    m_codes.push_back({Parameter('G', 1), Parameter('Z', 5), Parameter('F', 5000)});

    // Set to millimeters, absolute coordinates and absolute distance for extrusion
    m_codes.push_back({Parameter('G', 21)});
    m_codes.push_back({Parameter('G', 90)});
    m_codes.push_back({Parameter('M', 82)});

    // Interpret current extrusion as 0
    m_codes.push_back({Parameter('G', 92), Parameter('E', 0)});
}

void Generator::gen_end_of_gcode()
{
    m_codes.push_back({Parameter('G', 92), Parameter('E', 0)});

    // Turn off the fan
    m_codes.push_back({Parameter('M', 107)});

    // Turn off temperature
    m_codes.push_back({Parameter('M', 104), Parameter('S', 0)});

    // Home the x axis
    m_codes.push_back({Parameter('G', 28), Parameter('X', 0)});

    // Disable the motors
    m_codes.push_back({Parameter('M', 84)});
}

void Generator::gen_move_to(Vec3f p, int layer)
{
    float height = (layer + 1) * m_layer_height;
    m_codes.push_back({Parameter('G', 92), Parameter('E', 0)});
    m_codes.push_back({Parameter('G', 1), Parameter('Z', height + 1), Parameter('E', -2.0f)});
    m_codes.push_back({Parameter('G', 1), Parameter('X', p.x), Parameter('Y', p.y)});
    m_codes.push_back({Parameter('G', 1), Parameter('Z', height), Parameter('E', 2.0f)});
    m_codes.push_back({Parameter('G', 92), Parameter('E', 0)});
}

void Generator::gen_layer_gcode(float motor_speed, float extrusion_rat, std::vector<std::list<Vec3f>> flows)
{
    LOG("Gcode flow size " << flows.size());
    // Between every flow we will lift the head and move to the next starting point
    for (int i = 0; i < flows.size(); i++)
    {
        std::list<Vec3f> flow = flows[i];
        gen_move_to(flow.front(), i);
        m_codes.push_back({Parameter('G', 1), Parameter('F', motor_speed)});

        float extruded = 0;
        Vec3f &previous_p = flow.front();
        for (auto imove = std::next(flow.begin()); imove != flow.end(); ++imove)
        {
            Vec3f &p = *imove;
            float length = sqrtf32( powf32(previous_p.x - p.x, 2) + powf32(previous_p.y - p.y, 2));
            extruded += length * extrusion_rat;
            m_codes.push_back({Parameter('G', 1), Parameter('X', p.x), Parameter('Y', p.y), Parameter('E', extruded)});
            previous_p = p;
        }
    }
}

void Generator::generate(std::vector<std::list<Line>> layers)
{
    // For now we will just focus on generating the gcode for one layer.

    // If there were any previous codes in the array clear them
    // (for example if we reslice the object after moving it etc.)
    m_codes.clear();

    gen_begin_of_gcode();

    // The first thing we want to do is calculate the adjacent lines and 
    // create a flow of nodes.
    std::vector<std::list<Vec3f>> flows = calc_flows(layers[0]);

    // The first layer has custom settings for better adhesion.
    gen_layer_gcode(1800, 0.091, flows);

    gen_end_of_gcode();
}

std::string code_to_string(Code code)
{
    std::string s = code[0].to_string();

    for (int i = 1; i < code.size(); i++)
    {
        s += " " + code[i].to_string();
    }

    return s;
}

void Generator::write_to_file(std::string file)
{
    std::ofstream out;
    out.open(file);

    if (!out.is_open())
        ERROR("Could not open file '" << file << "'", 1);

    for (Code code : m_codes)
    {
        out << code_to_string(code) << "\n";
    }

    out.close();
}