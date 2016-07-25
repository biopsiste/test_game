#pragma once

#define TTF_PATH_LAZY     "../resources/ttf_archive/Lazy.ttf"
#define TTF_PATH_GOODDOG  "../resources/ttf_archive/GoodDog.ttf"

#define LINE_SPACING_PX   10

SDL_Color textWhite = { 255,255,255 };
SDL_Color textRed = { 255,  0,  0 };
SDL_Color textGreen = { 0,255,  0 };
SDL_Color textBlue = { 0,  0,255 };

// Single line TTF text texture
class LTextTexture : public LTexture {
public:
  TTF_Font *font;
  int size;

  // Load font from .ttf file
  bool loadFormat(std::string ttf_path, int size);
  void setFormat(TTF_Font * _font, int size);
  // set text and color
  bool setText(std::string Text, SDL_Color textColor = textWhite);
};

bool LTextTexture::loadFormat(std::string ttf_path, int _size) {
  // Open the font .ttf file
  size = _size;
  font = TTF_OpenFont(ttf_path.c_str(), size);
  if (font == NULL) {
    printf("Failed to load font %s! SDL_ttf Error: %s\n", ttf_path.c_str(), TTF_GetError());
    return false;
  }
  return true;
}

void LTextTexture::setFormat(TTF_Font * _font, int _size){
  font = _font;
  size = _size;
}

bool LTextTexture::setText(std::string Text, SDL_Color textColor /*= textWhite*/) {
  //Get rid of preexisting texture 
  free();

  SDL_Surface* textSurface = TTF_RenderText_Solid(font, Text.c_str(), textColor);
  if (textSurface == NULL) {
    printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
  }
  else {
    //Create texture from surface pixels 
    mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
    if (mTexture == NULL) {
      printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
    }
    else {
      //Get image dimensions 
      mWidth = textSurface->w;
      mHeight = textSurface->h;
    }

    //Get rid of old surface 
    SDL_FreeSurface(textSurface);
  }
  //Return success 
  return mTexture != NULL;
}



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
  void setText(std::string _text, SDL_Color color = textWhite);

  // Display text
  void render(Point p);
};

bool LMultiLineTextTexture::loadFormat(std::string ttf_path, int _size) {
  // Open the font .ttf file
  size = _size;
  font = TTF_OpenFont(ttf_path.c_str(), size);
  if (font == NULL) {
    printf("Failed to load font %s! SDL_ttf Error: %s\n", ttf_path.c_str(), TTF_GetError());
    return false;
  }
  return true;
}

void LMultiLineTextTexture::setText(std::string _text, SDL_Color color /* = textWhite */){
  text = _text;
  std::istringstream s(text);
  std::string line;
  lines.resize(0);
  while (std::getline(s, line)) lines.push_back(line);

  linesTexture.resize(lines.size());
  for (size_t i = 0; i < linesTexture.size(); i++) {
    linesTexture[i].setFormat(font, size);
    linesTexture[i].setText(lines[i], color);
  }
};

void LMultiLineTextTexture::render(Point p) {
  auto pivot = p;
  for (size_t i = 0; i < linesTexture.size(); i++) {
    if ( i != 0 ) pivot += Point{ 0, linesTexture[i].mHeight + LINE_SPACING_PX };
    linesTexture[i].render(pivot);
  }
}
