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

std::vector<std::list<Vec3f>> Generator::calc_flows(Layer layer)
{
    std::list<Line> &lines = layer.lines();

    if (lines.size() == 0)
        return {};

    // A flow can never be empty, so we add the first line to it.
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

// Returns a sorted list of infill points
// The function uses a pretty basic 'trace point' way of getting intersection
// points with the outer perimiter. The algorithm looks a lot like regular polygon
// filling.
//
// the function also takes a lot of arguments, these are just used so that we
// can call the function again to calculate the points in the different direction
// too (horizontal/vertical). This is achieved by providing an implementation for
// get_X() that would actually return the Y, a make_vec that actually returns Vec3f(y, x)
// etc. The linewidth is the line width of the actually printed line.
void Generator::calc_infill_points(std::list<Vec3f> &points,
                                   Layer layer, float interval,
                                   float& (*get_X)(Vec3f&),
                                   float& (*get_Y)(Vec3f&),
                                   Vec3f make_vec(float x, float y),
                                   float linewidth)
{
    for (auto i_line = layer.lines().begin(); i_line != layer.lines().end(); ++i_line)
    {
        Line &line = *i_line;

        // If the line is not within range and cannot be intersected, skip it.
        if (interval > fmax(get_Y(line.p1), get_Y(line.p2)) || interval < fmin(get_Y(line.p1), get_Y(line.p2)))
            continue;

        // Straight lines are ignored as well, they are not part of the inside of a polygon
        if (test_float(get_Y(line.p1), get_Y(line.p2)))
            continue;

        // The x value could be just the x value of either of the points on the lines if the line
        // is perpendicular with the tracing line, or it has to be calculated using the formula
        // we also used in the slicing algorithm.
        // x = (y - y1) / ((y2 - y1) / (x2 - x1)) + x1
        float x;
        if (test_float(get_X(line.p1), get_X(line.p2)))
            x = get_X(line.p1);
        else
            x = (interval - get_Y(line.p1)) / ((get_Y(line.p2) - get_Y(line.p1)) / (get_X(line.p2) - get_X(line.p1))) + get_X(line.p1);
        
        Vec3f point = make_vec(x, interval);

        // at last add the point to the points list.
        int dont_add = false;
        for (auto i = points.begin(); i != points.end(); ++i)
        {
            Vec3f &p = *i;
            // We don't want to add the same point twice.
            if (p == point)
            {
                dont_add = true;
                break;
            }
            else if (get_X(p) > get_X(point))
            {
                points.insert(i, point);
                dont_add = true;
                break;
            }
        }

        if (!dont_add)
            points.push_back(point);
    }

    // Now because we don't want our intersection points to be on the same location
    // as our actual outer perimiter, we subtract or add the linewidth to it.
    int i = 0;
    for (Vec3f &p : points)
    {
        if (i % 2)
            get_X(p) = get_X(p) - linewidth;
        else
            get_X(p) = get_X(p) + linewidth;
        i++;
    }
}

void Generator::calc_infill_flows(std::vector<std::list<Vec3f>> &flows, Layer layer, float interval_size,
                                  bool cross)
{
    std::vector<std::list<Vec3f>> in_poly_flows;

    int path_amount = -1;
    // We add and subtract the line thickness because we don't want the generated points
    // to be inside the outer perimiter, if this is the case, we would get a lot of excess 
    // filament and bad prints.
    for (float yval = layer.min_y() + m_line_thickness; yval <= layer.max_y() - m_line_thickness; yval += interval_size)
    {
        // Todo: create Vec2f
        std::list<Vec3f> points;

        // The first thing we do is calculate the intersecting points.
        // As you can see the cross variable will yield vertical or horizontal lines
        // by just changing the get_x() function to actually return y etc.
        if (cross)
            calc_infill_points(points, layer, yval,
                               +[](Vec3f &x) -> float& { return x.x; },
                               +[](Vec3f &x) -> float& { return x.y; },
                               +[](float x, float y) { return Vec3f(x, y, 0); },
                               m_line_thickness);
        else
            calc_infill_points(points, layer, yval, 
                               +[](Vec3f &x) -> float& { return x.y; },
                               +[](Vec3f &x) -> float& { return x.x; },
                               +[](float x, float y) { return Vec3f(y, x, 0); },
                               m_line_thickness);

        // The path amount is the amount of intersection lines (points.size() / 2).
        if (path_amount == -1)
        {
            path_amount = points.size() / 2;
            in_poly_flows = std::vector<std::list<Vec3f>>(path_amount, std::list<Vec3f>());
        }
        else if (path_amount != points.size() / 2)
        {
            // If the intersection line amount is different then the path_amount, it means we
            // should save the flows and start new ones
            for (std::list<Vec3f> &flow : in_poly_flows)
                flows.push_back(flow);

            in_poly_flows.clear();
            path_amount = points.size() / 2;
            in_poly_flows = std::vector<std::list<Vec3f>>(path_amount, std::list<Vec3f>());
        }

        // The point counter is basically an interator (easier then using std::distance())
        int point_counter = 0;
        for (auto i_point = points.begin(); i_point != points.end(); std::advance(i_point, 2))
        {
            // We want one in every two lines to be added in reverse order so that we get
            // a nice rectilinear pattern.
            Vec3f first, last;
            if (in_poly_flows[point_counter / 2].size() % 4)
            {
                first = *i_point;
                last = *std::next(i_point);
            }
            else
            {
                first = *std::next(i_point);
                last = *i_point;
            }
            in_poly_flows[point_counter / 2].push_back(first);
            in_poly_flows[point_counter / 2].push_back(last);

            point_counter += 2;
        }
    }
    
    // Push the last 'current' flows into the general flow list.
    for (std::list<Vec3f> &flow : in_poly_flows)
        flows.push_back(flow);
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
    m_codes.push_back({Parameter('M', 140), Parameter('S', 0)});

    // Home the x axis
    m_codes.push_back({Parameter('G', 28), Parameter('X', 0)});

    // Disable the motors
    m_codes.push_back({Parameter('M', 84)});
}

void Generator::gen_move_to(Vec3f p, int layer, Vec3f lastpos)
{
    m_codes.push_back({Parameter('G', 92), Parameter('E', 0)});
    float height = (layer + 1) * m_layer_height;

    m_codes.push_back({Parameter('G', 1), Parameter('Z', height + m_retraction_lift), Parameter('E', -m_retraction_length), Parameter('F', m_retraction_speed)});
    
    // Only generate a go to if the head is not already in the correct location
    // I don't think this makes a huge difference.
    if (!(test_float(p.x, lastpos.x) && test_float(p.y, lastpos.y)))
        m_codes.push_back({Parameter('G', 1), Parameter('X', p.x), Parameter('Y', p.y)});
    
    m_codes.push_back({Parameter('G', 1), Parameter('Z', height), Parameter('E', m_retraction_length), Parameter('F', m_retraction_speed)});
    m_codes.push_back({Parameter('G', 92), Parameter('E', 0)});
}

void Generator::gen_layer_gcode(float motor_speed, float extrusion_rat, std::vector<std::list<Vec3f>> flows,
                                int layer, Vec3f lastpos)
{
    LOG("Gcode flow size " << flows.size());
    // Between every flow we will lift the head and move to the next starting point
    for (int i = 0; i < flows.size(); i++)
    {
        std::list<Vec3f> flow = flows[i];

        // Go to the first point (the startpoint) of the flow
        gen_move_to(flow.front(), layer, lastpos);

        // Set the motor speed for the comming flows
        m_codes.push_back({Parameter('G', 1), Parameter('F', motor_speed)});

        float extruded = 0;
        Vec3f &previous_p = flow.front();
        for (auto imove = std::next(flow.begin()); imove != flow.end(); ++imove)
        {
            Vec3f &p = *imove;
            float length = sqrtf32(powf32(previous_p.x - p.x, 2) + powf32(previous_p.y - p.y, 2));
            extruded += length * extrusion_rat;
            m_codes.push_back({Parameter('G', 1), Parameter('X', p.x), Parameter('Y', p.y), Parameter('E', extruded)});
            previous_p = p;
        }
    }
}

void Generator::generate(std::vector<Layer> layers)
{
    // For now we will just focus on generating the gcode for one layer.

    // If there were any previous codes in the array clear them
    // (for example if we reslice the object after moving it etc.)
    m_codes.clear();

    gen_begin_of_gcode();

    Vec3f lastpos = Vec3f();

    for (int i_layer = 0; i_layer < layers.size() / 2; ++i_layer)
    {
        // The first thing we want to do is calculate the adjacent lines and
        // create a flow of nodes.
        std::vector<std::list<Vec3f>> flows = calc_flows(layers[i_layer]);

        // The first layer will generate a nice and tight filled layer by using close 
        // intervals, every layer after that will generate a rectilinear pattern where
        // the infill will be alternating between vertical en horizontal.
        if (i_layer == 0)
            calc_infill_flows(flows, layers[i_layer], 0.6);
        else
            calc_infill_flows(flows, layers[i_layer], 4, i_layer % 2);
        

        gen_layer_gcode(1800, 0.07, flows, i_layer, lastpos);

        // Enable the fan on 50% speed after the first layer.
        if (i_layer == 0)
            m_codes.push_back({Parameter('M', 106), Parameter('S', 127)});

        if (flows.size() != 0 && flows.back().size() != 0)
            lastpos = flows.back().back();
    }

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