#pragma once
// MSVC Debug Helper
#ifdef _MSC_VER
#define CLI_PAUSE           std::cout << "\nENTER to quit..."; std::cin.get();
#endif

#define MESSAGE             "no text wrapping built-in (hit X to quit or D to hide/show)"

//#define SHOW_PATH
#define SHOW_TEXT
//#define SHOW_BUTTONS
