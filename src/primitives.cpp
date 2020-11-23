#include <vec3f.h>

Line::Line(vec3f _x, vec3f _y)
{
    p1 = _x;
    p2 = _y;

    if (p1.z < p2.z)
    {
        m_min_z = p1.z;
        m_max_z = p2.z;
    }
    else
    {
        m_min_z = p2.z;
        m_max_z = p1.z;
    }
}

vec3f Line::calc_point_from_z(float z)
{
    // A little math never hurt anyone

    // The x value of the point will be equal to the height 
    // we are slicing at.
    vec3f res;
    res.z = z;

    // Formula y = m(x - x1) + y1 adapted
    // to x = (y-y1) / m + x1
    // 
    // for x1 != x2 because then we get an infinite m. If x1 is actually
    // equal to x2 we just set the x to that value.
    if (p1.x == p2.x)
        res.x = p1.x;
    else
        res.x = (z - p1.z) / ((p2.z - p1.z) / (p2.x - p1.x)) + p1.x;
    
    // Same goes for y, instead of x we just use y
    if (p1.y == p2.y)
        res.y = p1.y;
    
    else
        res.y = (z - p1.z) / ((p2.z - p1.z) / (p2.y - p1.y)) + p1.y;

    return res;
}

bool Line::contains_height(float height)
{
    // We ignore facets that are perfectly perpendicular with the z plane.
    // This is because a completely solid object will always have edges that
    // touch this plane. These edges will thus be recorded instead of the plane
    // itself.
    // TODO: image to make this easier to understand
    if (p1.z == height && p2.z == height)
        return false;

    return height >= m_min_z && height <= m_max_z;
}