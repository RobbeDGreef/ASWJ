#pragma once

#include <core.h>
#include <primitives.h>

// The layer class holds a sorted list of lines. The line 
// with the smallest y value is at the start of the list.
// Using this technique we can more efficiently do infill
// detection.
class Layer
{
private:
    std::list<Line> m_lines;

    bool m_bb_set = false;
    float m_min_x = 0, m_min_y = 0, m_max_x = 0, m_max_y = 0;

private:
    void set_bb(Line &line);

public:
    float max_x() { return m_max_x; }
    float max_y() { return m_max_y; }
    float min_x() { return m_min_x; }
    float min_y() { return m_min_y; }

    std::list<Line> &lines() { return m_lines; }
    void insert(Line line);

    Layer() {}
    Layer(std::list<Line> lines);
};