#include <primitives.h>
#include <layer.h>
#include <core.h>

void debug_layers_to_file(std::vector<Layer> &layers)
{
    std::ofstream out;
    
    int i = 0;
    for (Layer layer : layers)
    {
        out.open("tests/debug/" + std::to_string(i) + ".txt");
        
        for (Line line : layer.lines())
        {
            out << line.p1.to_string() << " " << line.p2.to_string() << "\n";
        }

        out.close();
        i++;
    }
}