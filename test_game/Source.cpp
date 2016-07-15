//Using SDL, SDL_image, standard math, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

//Screen dimension constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 672;

const int tileX = 0;
const int tileY = 32;
const int tileWidth = 64;
const int tileHeight = 64;
const int tileHeightOffset = 32;
const int mapX = 300;
const int mapY = 300;

inline void clamp(int& i, int a = 0, int b = 9) {
	if(i < a) i = a; else if(i > b) i = b;
}

struct Point {
	int x, y;
};

Point isometric2cartesian(Point p) {
	return Point{
		(2 * p.y + p.x) / 2,
		(2 * p.y - p.x) / 2
	};
}

Point cartesian2isometric(Point p) {
	return Point{
		p.x - p.y,
		(p.x + p.y) / 2
	};
}

Point tile2screen(Point p) {
	return Point{
		tileWidth * p.x / 2,
		tileHeight * p.y / 2
	};
}

Point screen2tile(Point p) {
	return Point{
		p.x / tileWidth,
		p.y / tileHeight
	};
}

Point mouse2tile(Point p) {
	return Point{
		p.x / tileWidth - p.y / (tileHeight / 2) + 1,
		p.x / tileWidth + p.y / (tileHeight / 2) + 1
	};
}

//Texture wrapper class
class LTexture {
public:
	//Initializes variables
	LTexture();

	//Deallocates memory
	~LTexture();

	//Loads image at specified path
	bool loadFromFile(std::string path);

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
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Scene sprites
SDL_Rect gSpriteClips[160], cursor;
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
	if(loadedSurface == NULL) {
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	} else {
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if(newTexture == NULL) {
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		} else {
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
	if(mTexture != NULL) {
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
	if(clip != NULL) {
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
	if(clip != NULL) {
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
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	} else {
		//Set texture filtering to linear
		if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if(gWindow == NULL) {
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		} else {
			//Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if(gRenderer == NULL) {
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			} else {
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if(!(IMG_Init(imgFlags) & imgFlags)) {
					printf("SDL_image could not initialize! SDL_mage Error: %s\n", IMG_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia() {
	//Loading success flag
	bool success = true;

	//Load sprite sheet texture
	if(!gSpriteSheetTexture.loadFromFile("../resources/iso-64x64-outside_numeri2.png")) {
		printf("Failed to load sprite sheet texture!\n");
		success = false;
	} else {

		for(int i = 0; i < 160; ++i) {
			int r = i / 10, c = i % 10;
			gSpriteClips[i].x = tileWidth * c;
			gSpriteClips[i].y = tileHeight * r;
			gSpriteClips[i].w = tileWidth;
			gSpriteClips[i].h = tileHeight;
		}

		cursor.x = tileWidth * (159 % 10);
		cursor.y = tileHeight * (159 / 10);
		cursor.w = tileWidth;
		cursor.h = tileHeight;
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

int main(int argc, char* args[]) {
	//Start up SDL and create window
	if(!init()) {
		printf("Failed to initialize!\n");
	} else {
		//Load media
		if(!loadMedia()) {
			printf("Failed to load media!\n");
		} else {
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			unsigned char map0[20][20] = {
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
			};
			unsigned char map1[20][20] = {
				{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
				{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
				{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
				{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
				{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
				{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
				{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
				{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
				{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
				{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
				{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
				{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
				{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
				{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
				{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
				{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
				{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
				{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
				{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
				{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
			};

			//While application is running
			int mouseX = 0, mouseY = 0;
			int cartX = 0, cartY = 0;
			int isoX = 0, isoY = 0;
			int tileX = 0, tileY = 0;
			bool done = false;
			while(!quit) {

				//Handle events on queue
				while(SDL_PollEvent(&e) != 0) {
					//User requests quit
					if(e.type == SDL_QUIT) {
						quit = true;
					}

					//Get mouse position
					if(e.type == SDL_MOUSEMOTION) {
						SDL_GetMouseState(&mouseX, &mouseY);
						mouseX -= mapX; mouseY -= mapY;

						//int tileY = mouseX / tileWidth + mouseY / (tileHeight - tileHeightOffset) - 1 - 1;
						//int tileX = mouseX / (tileHeight - tileHeightOffset) - tileY - 1;
						//clamp(tileX); clamp(tileY);

						//isoX = (tileX * tileWidth / 2) + (tileY * tileWidth / 2) + mapX;
						//isoY = (tileY * (tileHeight - tileHeightOffset) / 2) - (tileX * (tileHeight - tileHeightOffset) / 2) + mapY;

						//cout << tileX << "  " << tileY << "  " << isoX << "  " << isoY << endl;
						//cartY = mouseX / tileWidth + mouseY / (tileHeight - tileHeightOffset) - 1 - 1;
						//cartX = mouseX / (tileHeight - tileHeightOffset) - cartY - 1;

						//clamp(cartX, 0, 4); clamp(cartY, 0, 4);
						//cout /*<< tileX << "  " << tileY << "  "*/ << cartX << "  " << cartY << endl;

						auto mouseTile = mouse2tile(Point{ mouseX, mouseY });
						auto mouseP = tile2screen(mouseTile);
						cout << mouseX << "  " << mouseY << " --- " << mouseTile.x << "  " << mouseTile.y << " --- " << mouseP.x << "  " << mouseP.y << endl;
					}
					//if(e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP) {
				}

				//if(!done) {
				//Clear screen
				SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xFF);
				SDL_RenderClear(gRenderer);

				for(int r = 0; r < 5; ++r) {
					for(int c = 0; c < 5; ++c) {
						//cartX = tileWidth * c / 2, cartY = tileHeight * r / 2;
						//isoX = cartX - cartY, isoY = (cartX + cartY) / 2;
						//isoX = (c * tileWidth / 2) + (r * tileWidth / 2) + mapX;
						//isoY = (r * (tileHeight - tileHeightOffset) / 2) - (c * (tileHeight - tileHeightOffset) / 2) + mapY;
						//cout << cartX << "  " << cartY << "  " << isoX << "  " << isoY << endl;
						//gSpriteSheetTexture.render(isoX, isoY, &gSpriteClips[map1[r][c]]);
						auto p = cartesian2isometric(tile2screen(Point{ r, c }));
						p.x += mapX; p.y += mapY;
						gSpriteSheetTexture.render(p, &gSpriteClips[c + r * 5]);
					}
				}

				//isoX = mouseX / 64 * 64, isoY = mouseY / 64 * 64;
				//isoX = cartX - cartY, isoY = (cartX + cartY) / 2;

				//cartY = mouseX / tileWidth + mouseY / (tileHeight - tileHeightOffset) - 1 - 1;
				//cartX = mouseX / (tileHeight - tileHeightOffset) - cartY - 1;
				//clamp(cartX, 0, 4); clamp(cartY, 0, 4);

				////cartX = tileWidth * tileX / 2, cartY = tileHeight * tileY / 2;
				////isoX = cartX - cartY, isoY = (cartX + cartY) / 2;

				//isoX = (cartX * tileWidth / 2) + (cartY * tileWidth / 2) + mapX;
				//isoY = (cartY * (tileHeight - tileHeightOffset) / 2) - (cartX * (tileHeight - tileHeightOffset) / 2) + mapY;

				//cartX = 64 * tileX / 2, cartY = 64 * tileY / 2;
				//isoX = cartX - cartY, isoY = (cartX + cartY) / 2;

				auto mouseTile = mouse2tile(Point{ mouseX, mouseY });
				auto mouseP = cartesian2isometric(tile2screen(mouseTile)); mouseP.x += mapX; mouseP.y += mapY;
				gSpriteSheetTexture.render(mouseP, &cursor);

				//Update screen
				SDL_RenderPresent(gRenderer);
				done = true;
				//}
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}