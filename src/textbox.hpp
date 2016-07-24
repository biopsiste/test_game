#pragma once

#define TTF_PATH_LAZY     "../resources/ttf_archive/Lazy.ttf"
#define TTF_PATH_GOODDOG  "../resources/ttf_archive/GoodDog.ttf"


SDL_Color textWhite = { 255,255,255 };
SDL_Color textRed = { 255,  0,  0 };
SDL_Color textGreen = { 0,255,  0 };
SDL_Color textBlue = { 0,  0,255 };

class LTextTexture : public LTexture{
public:
  TTF_Font *font;

  // Load font from .ttf file
  bool loadFormat(std::string ttf_path, int size);
  // set text and format
  bool loadText(std::string Text, SDL_Color textColor);
};

bool LTextTexture::loadFormat(std::string ttf_path, int size) {
  // Open the font .ttf file
  font = TTF_OpenFont(ttf_path.c_str(), size);
  if (font == NULL) {
    printf("Failed to load font %s! SDL_ttf Error: %s\n", ttf_path.c_str(), TTF_GetError());
    return false;
  }
  return true;
}

bool LTextTexture::loadText(std::string Text, SDL_Color textColor) {
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
