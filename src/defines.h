// Copyright 2016 Marco Di Cristina, Alessandro Fabbri
#pragma once

// MSVC Debug Helper
#ifdef _MSC_VER
#include <iostream>
#define CLI_PAUSE           std::cout << "\nENTER to quit..."; std::cin.get();
#endif

#define MESSAGE             "no text wrapping built-in (hit X to quit or D to hide/show)"

//#define SHOW_BUTTONS
#define SHOW_TEXT
#define SHOW_CURSOR
//#define SHOW_UNITS
//#define SHOW_PATH
#define SHOW_GUI


#define MAP_TEST_1_PATH     "../resources/maps/test_map_t.txt"

