Started looking for stl docs
 -> wikipedia
 -> reprap wiki

Started creating simple 3d objects
 -> cube
 -> box
 -> suzan

? How are dimentions handled

there are two types of stl files: binary and ascii
blender gives ascii

found site:
-> https://docs.fileformat.com/cad/stl/

most used fileformat is binary so that is what well write

each triangle are 12 32bit floating point numbers

# 1.1 - Base of the cpp project.
wrote the simple baseline of the project, mostly misc.

found this site
http://www.fabbers.com/tech/STL_Format

binary format:
uint8 * 80      header
uint32          Number of facets in the file

[ * the number of facets in the file
    float32     i of the normal
    float32     j of the normal
    float32     k of the normal

    float32     x of vertex number 1
    float32     y of vertex number 1
    float32     z of vertex number 1

    float32     x of vertex number 2
    float32     y of vertex number 2
    float32     z of vertex number 2

    float32     x of vertex number 3
    float32     y of vertex number 3
    float32     z of vertex number 3

    uint16_t    attribute byte count
]


Added a basic system to load in the facets. Im not yet sure how i will handle large
files but we will see if it ever causes a problem.