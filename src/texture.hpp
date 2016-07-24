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
#define TILESET_CURSOR_TILE_INDEX        8
#define TILESET_HIGHLIGHTER_TILE_INDEX   16
#endif

// Global SDL object
SDL_Renderer * gRenderer = NULL;
SDL_Window* gWindow = NULL;



////// Texture wrapper class
class LTexture {
public:
  //The actual hardware texture
  SDL_Texture* mTexture;
  SDL_Renderer* mRenderer;

  //Image dimensions
  int mWidth;
  int mHeight;

  //Initializes variables
  LTexture();
  //Deallocates memory
  ~LTexture();

  // Loads image at specified path
  bool loadFromFile(std::string path);

  //Deallocates texture
  void free();

  //Renders texture at given point
  void render(int x, int y, SDL_Rect* clip = NULL);
  void render(Point p, SDL_Rect* clip = NULL);
};

LTexture::LTexture(){
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

void LTexture::free() {
  //Free texture if it exists
  if (mTexture != NULL) {
    SDL_DestroyTexture(mTexture);
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
  }
}

void LTexture::render(int x, int y, SDL_Rect* clip /* = NULL */ ) {
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
void LTexture::render(Point p, SDL_Rect* clip /* = NULL */ ) {
  render(p.x, p.y, clip);
}



//////// Global function (get rid of these!!!)

//Scene sprites
SDL_Rect gSpriteClips[TILESET_TILES], cursorSprite, unitSprite, highlighterSprite, HighCursorSprite, LowCursorSprite;
LTexture gSpriteSheetTexture;

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

//void renderCursor(const Point& cam, const Point& tile_index, SDL_Rect * tile) {
//  if (is_in_map(tile_index)) renderTile(cam, tile_index, tile);
//}

