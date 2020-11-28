#include <stlparser/stlparser.h>
#include <generator/generator.h>

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        ERROR("No input files", 1);
    }

    std::string s(argv[1]);
    StlParser parser(s);
    parser.parse();
    
    std::vector<Layer> &layers = parser.slice();
    debug_layers_to_file(layers);

    Generator generator(235, 95, 0.3);
    generator.generate(layers);
    generator.write_to_file("./test.gcode");
}
