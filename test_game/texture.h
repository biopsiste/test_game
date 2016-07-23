#pragma once
#include <fstream>
#include <sstream>

#define USE_TILESET                      2

#if USE_TILESET == 1
//tileset 1
#define TILESET_PATH                     "../resources/iso-64x64-outside_numeri_transp.png"

#define TILESET_COLUMNS                  10
#define TILESET_ROWS                     16
#define TILESET_TILES                    160

#define TILESET_UNIT_TILE_INDEX          119
#define TILESET_CURSOR_TILE_INDEX        159
#define TILESET_HIGHLIGHTER_TILE_INDEX   44
#elif USE_TILESET == 2
//tileset 2
#define TILESET_PATH                     "../resources/basic_ground_tiles_64_halftile.png"

#define TILESET_COLUMNS                  8
#define TILESET_ROWS                     7
#define TILESET_TILES                    56

#define TILESET_UNIT_TILE_INDEX          47
#define TILESET_CURSOR_TILE_INDEX        7
#define TILESET_HIGHLIGHTER_TILE_INDEX   15
#endif

//Texture wrapper class
class LTexture {
public:
  //Initializes variables
  LTexture();
  //Deallocates memory
  ~LTexture();

  // Loads image at specified path
  bool loadFromFile(std::string path);
  // Loads font from .ttf file
  bool loadTextMedia(std::string path, std::string text, SDL_Color textColor);
  // Set text and color
  bool setText( std::string textureText, SDL_Color textColor );

  //Deallocates texture
  void free();

  //Renders texture at given point
  void render(int x, int y, SDL_Rect* clip = NULL);
  void render(Point p, SDL_Rect* clip = NULL);

  //Gets image dimensions
  int getWidth();
  int getHeight();

private:
  //The actual hardware texture
  SDL_Texture* mTexture;

  //Image dimensions
  int mWidth;
  int mHeight;
};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia(std::string path);

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

// Font and text texture
TTF_Font *gFont = NULL;
LTexture gTextTexture, gTextTexture2;

//Scene sprites
SDL_Rect gSpriteClips[TILESET_TILES], cursorSprite, unitSprite, highlighterSprite, HighCursorSprite, LowCursorSprite;
LTexture gSpriteSheetTexture;

LTexture::LTexture() {
  //Initialize
  mTexture = NULL;
  mWidth = 0;
  mHeight = 0;
}

LTexture::~LTexture() {
  //Deallocate
  free();
}

bool LTexture::loadFromFile(std::string path) {
  //Get rid of preexisting texture
  free();

  //The final texture
  SDL_Texture* newTexture = NULL;

  //Load image at specified path
  SDL_Surface* loadedSurface = IMG_Load(path.c_str());
  if (loadedSurface == NULL) {
    printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
  }
  else {
    //Color key image
    SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

    //Create texture from surface pixels
    newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
    if (newTexture == NULL) {
      printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
    }
    else {
      //Get image dimensions
      mWidth = loadedSurface->w;
      mHeight = loadedSurface->h;
    }

    //Get rid of old loaded surface
    SDL_FreeSurface(loadedSurface);
  }

  //Return success
  mTexture = newTexture;
  return mTexture != NULL;
}

bool LTexture::setText(std::string textureText, SDL_Color textColor) { 
  //Get rid of preexisting texture 
  free(); 
  
  SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor ); 
  if( textSurface == NULL ) { 
    printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() ); 
  } 
  else { 
    //Create texture from surface pixels 
    mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface ); 
    if( mTexture == NULL ) { 
      printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() ); 
    } 
    else { 
      //Get image dimensions 
      mWidth = textSurface->w; 
      mHeight = textSurface->h; 
    } 
    
    //Get rid of old surface 
    SDL_FreeSurface( textSurface ); 
  } 
  //Return success 
  return mTexture != NULL; 
}

bool LTexture::loadTextMedia(std::string path, std::string text, SDL_Color textColor) {
  bool success = true; 
  
  //Open the font 
  gFont = TTF_OpenFont( path.c_str(), 35 ); 
  if( gFont == NULL ) { 
    printf( "Failed to load font %s! SDL_ttf Error: %s\n", path.c_str(), TTF_GetError() ); 
    success = false; 
  } 
  else { 
    //Render initial text 
    if( !setText( text, textColor ) ) { 
      printf( "Failed to render text texture!\n" ); 
      success = false; 
    } 
  } 
  
  return success;
}

void LTexture::free() {
  //Free texture if it exists
  if (mTexture != NULL) {
    SDL_DestroyTexture(mTexture);
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
  }
}

void LTexture::render(int x, int y, SDL_Rect* clip) {
  //Set rendering space and render to screen
  SDL_Rect renderQuad = { x, y, mWidth, mHeight };

  //Set clip rendering dimensions
  if (clip != NULL) {
    renderQuad.w = clip->w;
    renderQuad.h = clip->h;
  }

  //Render to screen
  SDL_RenderCopy(gRenderer, mTexture, clip, &renderQuad);
}
void LTexture::render(Point p, SDL_Rect* clip) {
  render(p.x, p.y, clip);
}

int LTexture::getWidth() {
  return mWidth;
}

int LTexture::getHeight() {
  return mHeight;
}

bool init() {
  //Initialization flag
  bool success = true;

  //Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
    success = false;
  }
  else {
    //Set texture filtering to linear
    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
      printf("Warning: Linear texture filtering not enabled!");
    }

    //Create window
    gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
    if (gWindow == NULL) {
      printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
      success = false;
    }
    else {
      //Create renderer for window
      gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
      if (gRenderer == NULL) {
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        success = false;
      }
      else {
        //Initialize renderer color
        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

        //Initialize PNG loading
        int imgFlags = IMG_INIT_PNG;
        if (!(IMG_Init(imgFlags) & imgFlags)) {
          printf("SDL_image could not initialize! SDL_mage Error: %s\n", IMG_GetError());
          success = false;
        }
      }
    }
  }

  return success;
}

bool loadMedia(std::string path) {
  //Loading success flag
  bool success = true;

  //Load sprite sheet texture
  if (!gSpriteSheetTexture.loadFromFile(path)) {
    printf("Failed to load sprite sheet texture!\n");
    success = false;
  }
  else {
    for (int i = 0; i < TILESET_TILES; ++i) {
      int r = i / TILESET_COLUMNS, c = i % TILESET_COLUMNS;
      gSpriteClips[i].x = int( TILE_W * c );
      gSpriteClips[i].y = int( TILE_H * r );
      gSpriteClips[i].w = int( TILE_W );
      gSpriteClips[i].h = int( TILE_H );
    }

    cursorSprite = gSpriteClips[TILESET_CURSOR_TILE_INDEX];
    HighCursorSprite = gSpriteClips[TILESET_CURSOR_TILE_INDEX-1];
    LowCursorSprite = gSpriteClips[TILESET_CURSOR_TILE_INDEX];
		unitSprite = gSpriteClips[TILESET_UNIT_TILE_INDEX];
		highlighterSprite = gSpriteClips[TILESET_HIGHLIGHTER_TILE_INDEX];
	}

  return success;
}

void close() {
  //Free loaded images
  gSpriteSheetTexture.free();

  //Destroy window	
  SDL_DestroyRenderer(gRenderer);
  SDL_DestroyWindow(gWindow);
  gWindow = NULL;
  gRenderer = NULL;

  //Quit SDL subsystems
  IMG_Quit();
  SDL_Quit();
}

///////////// useful render function

void renderTile(const Point& cam, const Point& tile_index, SDL_Rect * tile) {
  gSpriteSheetTexture.render(tile2screen(tile_index) - cam, tile);
}

void renderCursor(const Point& cam, const Point& tile_index, SDL_Rect * tile) {
  if (is_in_map(tile_index)) renderTile(cam, tile_index, tile);
}

void smart_renderCursor(const Point& cam, const Point& tile_index) {
  if (is_in_map(tile_index)) {
    SDL_Rect * currentCursor;
    if (tile_index.x < MAP_W / 2 && tile_index.y < MAP_H / 2) currentCursor = &HighCursorSprite;
    else currentCursor = &LowCursorSprite;
    renderTile(cam, tile_index, currentCursor);
  }
}

struct Cube {
  int height; //even height: is an halftile, odd height: is full sized cube
  int sprite_index;
  bool passable;
  //bool empty;

  Cube() : height{}, sprite_index{}, passable(true)/*, empty(false)*/ {}
  Cube(int _h, int _si, bool _pass) : height(_h), sprite_index(_si), passable(_pass)/*, empty(false)*/ {}
  Cube(std::string entry) {
    std::istringstream istr(entry);
    std::string token;
    std::getline(istr, token, ','); sprite_index = std::stoi(token);
    std::getline(istr, token, ','); height = std::stoi(token);
    std::getline(istr, token, ','); passable = std::stoi(token);
    //std::getline(istr, token, ','); empty = std::stoi(token);
    //std::cout << sprite_index << " " << height << " " << passable << " " << empty << std::endl; system("pause");
  }
};

struct CubeStack {
  std::vector<Cube> s;

  CubeStack() {}
  CubeStack(std::string stackentry) {
    std::istringstream istr(stackentry);
    std::string token;
    while(std::getline(istr, token, '|')) {
      //std::cout << token << std::endl; //system("pause");
      s.emplace_back(token);
    }
    //std::cout << "---" << std::endl; system("pause");
  }

  void render(const Point& cam, const Point& pos) {
    for(int i = 0; i < s.size(); ++i) {
      gSpriteSheetTexture.render(tile2screen(pos) - Point{ 0, i*int(TILE_H) / 2 } -cam, &gSpriteClips[s[i].sprite_index]);
    }
  }
};

struct Map {
  //std::vector<MapLayer> m;
  std::vector<std::vector<CubeStack>> m;

  Map(std::string filename) {
    std::ifstream ifi(filename);
    std::string line, stackentry; int w, h;
    std::getline(ifi, line); //ignore header
    ifi >> w >> h; //std::cout << w << " " << h << std::endl;
    std::getline(ifi, line); //ignore w h
    m.assign(w, std::vector<CubeStack>(h));
    int i = 0;
    while(std::getline(ifi, line)) {
      //std::cout << line << std::endl; system("pause");
      std::istringstream istr(line);
      int j = 0;
      while(istr >> stackentry) {
        //std::cout << stackentry << std::endl;
        //std::cout << i << " " << j << std::endl;
        m[i][j] = CubeStack(stackentry);
        j++;
      }
      i++;
    }
  }

  void render(const Point& cam) {
    for(int i = 0; i < m.size(); i++) {
      for(int j = 0; j < m[0].size(); j++) {
        m[i][j].render(cam, { i,j });
      }
    }
  }
};
