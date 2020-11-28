#pragma once

#include <stdio.h>
#include <iostream>

#define COLOR_RED "\u001b[31;1m"
#define COLOR_END "\u001b[0m"

#define DW(x) do { x; } while (0)

#define LOG(x) DW(std::cout << x << "\n")
#define FLOG(x) DW(printf(x))
#define ERROR(x, errcode) DW(std::cerr << COLOR_RED << x << COLOR_END << "\n"; exit(errcode))

class Layer;
void debug_layers_to_file(std::vector<Layer> &layers);