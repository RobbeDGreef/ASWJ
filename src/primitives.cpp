#include <primitives.h>

Line::Line(Vec3f _x, Vec3f _y)
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

Vec3f Line::calc_point_from_z(float z)
{
    // A little math never hurt anyone

    // The z value of the point will be equal to the height 
    // we are slicing at.
    Vec3f res;
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
    // We ignore facets that are perfectly parallel with the z plane.
    // This is because a completely solid object will always have edges that
    // touch this plane. These edges will thus be recorded instead of the plane
    // itself.
    // TODO: image to make this easier to understand
    if (test_float(p1.z, height) && test_float(p2.z, height))
        return false;

    // We have to account for float precision here too
    return height >= (m_min_z - COMP_PRECISION) && height <= (m_max_z + COMP_PRECISION);
}

void Vec3f::transform(Vec3f offset, Vec3f scale)
{
    // We do not yet support rotation
    x *= scale.x;
    y *= scale.y;
    z *= scale.z;

    x += offset.x;
    y += offset.y;
    z += offset.z;
}

bool test_float(float x, float y, float epsilon)
{
    return fabs(x - y) < epsilon;
}