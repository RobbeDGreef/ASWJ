# A Slicer Writing Journey

## Introduction
Ever since I've had a 3d printer, I've used slicers. PrusaSlicer, Slic3r, Cura, and even 
some others. However I never really knew what that slicer does behind the scenes. 
I know it turns a file into gcode but thats about it. I wanted to change that and so I just
thought why not write one?

I'm certainly not the only one that wonders how slicers do what they do. We know we feed them an
stl file and it gives us gcode but how does it turn an stl file into small slices and how does it
generate that gcode. Those are the kind of questions I want to answer.

So as a goal for myself I am setting my bar at: Slice and successfully print a [benchy](https://www.thingiverse.com/thing:763622) at a Prusa
mk3s using a (binary) stl file.

As another challenge for myself I wanted to document the journey I take and keep it light enough
so that an average person, even a non (c++) programmer, can still understand at least the basics of 
what this code will do. Yet practical enough so that anyone who justs wants to get their hands dirty
and code their own slicer can do so too.

The name "A Slicer Writing Journey" is obviously taken from the popular 
["A Compiler Writing Journey"](https://github.com/DoctorWkt/acwj) repository. It is a great project
and taught me a ton about compilers. I **highly** recommend it to anyone who is curious 
about compilers. Since I wanted to document this journey too, I thought the same naming scheme
was appropriate. The only difference (besides of course the code and the subject of the code) is 
that ACWJ is more practical. This repository aims to provide theory as well so that a non programmer
can read the theory parts to get a better understanding about slicers. By the way I say non 
programmer but I also mean anyone who does not know C++. I think it is very possible to read
the theory and write the slicer in any other language. 

I think it is also important to mention that I am not a slicer expert, I'm not even a 3d printing
expert. I've done my fair share of printing, modeling and coding so I have some idea of what I am 
working on but I am mostly just bored during this global pandemic like the rest of us. 

## The parts
The parts marked with '(contains theory)' start with some theory about how we hope to
achieve what we are doing. You don't need to be able to code to read them.

- [Part 0](docs/part0.md): STL info and the basic code structure (Contains theory)
- [Part 1](docs/part1.md): Loading the data (No theory, just code)
- [Part 2](docs/part2.md): Actual slicing (Contains theory)
- [Part 3](docs/part3.md): Generating some simple gcode (Contains theory)
- [Part 4](docs/part4.md): Infill and a first layer (Contains theory)

This is not complete yet, more parts will come out soon.

## How do I build your code?
The code is being written on linux but it should work on any other OS with a decent C++ compiler
and cmake support.

### Unix users
Running `build.sh` should build you a nice `slicer` binary in the main directory. It
uses CMake and Make to build it.

### Windows users
I am not very familiar with the windows build systems but since the project uses cmake it should not
be a problem. These tutorials assume you at least have a moderate understating of C++ so 
you should be able to build it yourself :P

### The python programs
I've used python to quickly create some small applications to display the generated output.
I did not document them, I just quickly threw something together to check if the actual
code was correct. You are free to use them to test your slicer output as well.

To run them just run

    python3 tests/layerview.py
    python3 tests/gcode_interpreter.py