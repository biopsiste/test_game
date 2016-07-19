#pragma once
//Texture wrapper class
class LTexture {
public:
  //Initializes variables
  LTexture();
  //Deallocates memory
  ~LTexture();

  //Loads image at specified path
  bool loadFromFile(std::string path);
  // Loads text from .ttf file
  bool loadTextMedia(std::string path, std::string text, SDL_Color textColor);
  //Creates image from font string 
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
SDL_Rect gSpriteClips[160], cursorSprite, unitSprite, highlighterSprite;
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
    printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() ); 
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
  //Set rendering space and render to screen
  SDL_Rect renderQuad = { p.x, p.y, mWidth, mHeight };

  //Set clip rendering dimensions
  if (clip != NULL) {
    renderQuad.w = clip->w;
    renderQuad.h = clip->h;
  }

  //Render to screen
  SDL_RenderCopy(gRenderer, mTexture, clip, &renderQuad);
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
    gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (gWindow == NULL) {
      printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
      success = false;
    }
    else {
      //Create renderer for window
      gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
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

    for (int i = 0; i < 160; ++i) {
      int r = i / 10, c = i % 10;
      gSpriteClips[i].x = int( TILE_W * c );
      gSpriteClips[i].y = int( TILE_H * r );
      gSpriteClips[i].w = int( TILE_W );
      gSpriteClips[i].h = int( TILE_H );
    }

    cursorSprite = gSpriteClips[159];
		unitSprite = gSpriteClips[119];
		highlighterSprite = gSpriteClips[44];
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

void renderTile(int i, int j, SDL_Rect * tile) {
  gSpriteSheetTexture.render(tile2screen(i, j), tile);
}

void renderTile(Point tile_index, SDL_Rect * tile) {
  renderTile(tile_index.x, tile_index.y, tile);
}

void renderCursor(Point tile_index, SDL_Rect * tile) {
  if( is_in_map(tile_index) ) renderTile(tile_index, tile);
}
