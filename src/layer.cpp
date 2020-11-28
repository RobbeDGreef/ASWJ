#include <layer.h>

// x < y
static bool sortfunction(Line x, Line y)
{
    return fmin(x.p1.y, x.p2.y) < fmin(y.p1.y, y.p2.y);
}

Layer::Layer(std::list<Line> lines) : m_lines(lines)
{
    m_lines.sort(sortfunction);

    // Set some bounding box initial values.
    m_min_x = m_max_x = fmin(lines.front().p1.x, lines.front().p2.x);
    m_min_y = m_max_y = fmax(lines.front().p1.y, lines.front().p2.y);
    m_bb_set = true;
    
    // Check the bounding box for all the next lines.
    for (auto i = std::next(lines.begin()); i != lines.end(); ++i) 
        set_bb(*i);
}

void Layer::set_bb(Line &line)
{
    // TODO: is there any better way?
    if (fmin(line.p1.x, line.p2.x) < m_min_x)
        m_min_x = fmin(line.p1.x, line.p2.x);
    
    if (fmin(line.p1.y, line.p2.y) < m_min_y)
        m_min_y = fmin(line.p1.y, line.p2.y);
    
    if (fmax(line.p1.x, line.p2.x) > m_max_x)
        m_max_x = fmax(line.p1.x, line.p2.x);
    
    if (fmax(line.p1.y, line.p2.y) > m_max_y)
        m_max_y = fmax(line.p1.y, line.p2.y);
}

void Layer::insert(Line line)
{
    // Set the bounding box of the layer.
    if (!m_bb_set)
    {
        // If it is unset, set some initial values.
        m_min_x = m_max_x = fmin(line.p1.x, line.p2.x);
        m_min_y = m_max_y = fmax(line.p1.y, line.p2.y);
        m_bb_set = true;
    }
    else
        set_bb(line);

    // The layer is sorted.
    for (auto i = m_lines.begin(); i != m_lines.end(); ++i)
    {
        if (!sortfunction(*i, line))
        {
            m_lines.insert(i, line);
            return;
        }
    }
    m_lines.push_back(line);
}