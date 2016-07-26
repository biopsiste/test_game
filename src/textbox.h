#pragma once
#include <vector>

#include <SDL.h> 
#include <SDL_ttf.h> 
#include "texture.h"

#define TTF_PATH_LAZY     "../resources/ttf_archive/Lazy.ttf"
#define TTF_PATH_GOODDOG  "../resources/ttf_archive/GoodDog.ttf"

#define LINE_SPACING_PX   10

#define SDL_WHITE SDL_Color{ 255, 255, 255 }
#define SDL_RED SDL_Color{  255, 0, 0 }
#define SDL_GREEN SDL_Color{ 0, 255, 0  }
#define SDL_BLUE SDL_Color{ 0, 0, 255 }
#define DEFAULT_TEXT_COLOR SDL_WHITE

// Single line TTF text texture
class LTextTexture : public LTexture {
public:

  TTF_Font *font;
  int size;

  // Load font from .ttf file
  bool loadFormat(std::string ttf_path, int size);
  void setFormat(TTF_Font * _font, int size);
  // set text and color
  bool setText(std::string Text, SDL_Color textColor = DEFAULT_TEXT_COLOR);
};

//////// Multiline TTF TextBox Texture
class LMultiLineTextTexture {
public:
  Point p;
  TTF_Font *font;
  int size;
  std::string text;
  std::vector<std::string> lines;
  std::vector<LTextTexture> linesTexture;

  // Load format from file
  bool loadFormat(std::string ttf_path, int _size);
  // Set multitext line, line break char is '\n'
  void setText(std::string _text, SDL_Color color = DEFAULT_TEXT_COLOR);

  // Display text
  void render(Point p);
};
