# Part 1: Loading the data

[Repository commit link](https://github.com/RobbeDGreef/aswj/tree/f9a6fe31a69c703914096a52da9e43b02d6a6707)

Parsing the stl data is not that hard. Now that we know the structure of the file, we can
define the structure in the code.
```cpp
struct Facet
{
public:
    struct vec3f normal;
    struct vec3f vertices[3];
    uint16_t attributes;

public:
    void print()
    {
        LOG("Normal: " << normal.to_string());
        for (int i = 0; i < 3; i++)
        {
            LOG("Vert " << i << ": " << vertices[i].to_string());
        }
    }
}__attribute__((packed));
```

The vec3f structure is defined as
```cpp
struct vec3f
{
public:
    float x;
    float y;
    float z;
        
public:
    std::string to_string() 8bada1eaf2f49277e3cd2382f66fc9dfbbdc8727
    {
        return std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z);   
    }
    
};
```

There are two things important here. First of all, the __attribute__((packed)) is **beneficial** for
this to work. This attribute means that the compiler will not add any padding to pad the bytes to
4 byte marks. That would happen here and if you would do a `sizeof(struct Facet)` you would get 52
instead of the correct 50. And secondly, the `print()` function that I added does not increase
the size of the struct at all. It is just there for testing purposes.

I did also add a facet array to the StlParser and a parse() function.
```cpp
class StlParser
{
private:
    std::ifstream m_stlfile;
    // new:
    std::vector<Facet> m_facet_array;
public:
    StlParser(std::string file);
    ~StlParser();
    // new:
    void parse();

};
```
We will save the facet data in this vector.

In the `stlparser.cpp` I implemented the parse function. It simply reads the data into the array.

```cpp
void StlParser::parse()
{
    LOG("parsing file");
    // The first 80 bytes are a unimportant ascii, however
    // for testing we will print it anyway

    char header[80];
    m_stlfile.read(header, 80);
    LOG("File header reads: '" << header << "'");

    // Read the facet count.
    uint32_t facet_count;
    m_stlfile.read((char*) &facet_count, 4);

    LOG("Facet count: " << facet_count);

    // Initialize the facet array that will be used to keep all the facets.
    m_facet_array = std::vector<Facet>(facet_count);

    for (uint i = 0; i < facet_count; i++)
    {
        m_stlfile >> (char*) &(m_facet_array[i]);
        m_facet_array[i].print();
    }
}
```

Now to test this add i changed the main.cpp file a bit, i changed the StlParser(...) call with

```cpp
    std::string s(argv[1]);
    StlParser parser(s);
    parser.parse();
```

And normally if you run the code with `tests/stls/cube.stl` as parameter. You should
get the header, facet count and 12 facets printed to the screen. 