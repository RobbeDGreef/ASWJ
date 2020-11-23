#include <vec3f.h>
#include <core.h>

void debug_layers_to_file(std::vector<std::list<Line>> &layers)
{
    std::ofstream out;
    
    int i = 0;
    for (std::list<Line> list : layers)
    {
        out.open("tests/debug/" + std::to_string(i) + ".txt");
        
        for (Line line : list)
        {
            out << line.p1.to_string() << " " << line.p2.to_string() << "\n";
        }

        out.close();
        i++;
    }
}