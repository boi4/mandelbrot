#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#define WIDTH 1920
#define HEIGHT 1080
#define INIT_SCALE 0.002

#define COLOR 15u
#define COLOR_MULTIPLIER 0xffu

#define REAL -0.6743544
#define IMAG 0.43653479029
//#define REAL 0.4
//#define IMAG 0

#define ESCAPE_ABS_SQUARE 4.0
#define MAX_ITERS 0x100u

// globals for sdl
SDL_Window *gWindow;
SDL_Surface *gScreenSurface;
SDL_Renderer *gMyRenderer;


// init the globals
bool init() {
  if(SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    return false;
  }

  gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

  if(gWindow == NULL) {
    printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    return false;
  }

  gScreenSurface = SDL_GetWindowSurface(gWindow);

  gMyRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
  if(!gMyRenderer) {
    printf("rip renderer\n");
    return false;
  }

  return true;
}


void close() {
  SDL_DestroyWindow(gWindow);
  SDL_Quit();
}


// renders pixels from array
void renderPixels(int *pixels) {
  int a;

  for(int y = 0; y < HEIGHT; y++) {
    for(int x = 0; x < WIDTH; x++) {
      a = pixels[y*WIDTH + x];
      SDL_SetRenderDrawColor(gMyRenderer, (a >> 24) & 0xFF, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF);
      SDL_RenderDrawPoint(gMyRenderer, x, y);
    }
  }

  SDL_RenderPresent(gMyRenderer);
}


// returns number of iterations needed to escape
unsigned int iterate(double cr, double ci) {
  double zr = 0, zi = 0, tmp;
  unsigned int iterations;

  for(iterations = 0; zr * zi < ESCAPE_ABS_SQUARE && iterations < MAX_ITERS; iterations++) {
    // z = z * z + c
    tmp = (zr * zr) - (zi * zi) + cr;
    zi = 2.0 * (zr * zi) + ci;
    zr = tmp;
  }
  return iterations;
}


// m is the middle point of the plane
void renderMandel(int *pixels, double mr, double mi, double scale) {
  unsigned int tmp;
  double zi, zr, v;
  double upperr = mr-(double)WIDTH*scale/2; // upper is the complex number on the upper left corner
  double upperi = mi-(double)HEIGHT*scale/2;

  printf("(%f,%f) (%f,%f)\n", upperr, upperi, upperr+(double)WIDTH*scale, upperi+(double)HEIGHT*scale);

  for(int y = 0; y < HEIGHT; y++) {
    zi = upperi + (double)y * scale; // imaginary part stays the same for a fixed y value
    for(int x = 0; x < WIDTH; x++) {
      zr = upperr + (double)x * scale;
      tmp = iterate(zr, zi);
      pixels[y*WIDTH + x] = 0x000000ff | ((tmp * COLOR_MULTIPLIER / MAX_ITERS) << COLOR);
    }
  }

  renderPixels(pixels);
}



int main(int argc, char *args[]) {
  bool quit = false;
  SDL_Event e;

  if(!init()) {
    fprintf(stdout, "Failed to Initialize!\n");
    exit(-1);
  }

  double mr = REAL, mi = IMAG;
  double scale = INIT_SCALE;
  int pixels[WIDTH*HEIGHT];

  while(!quit) {

    renderMandel(pixels, mr, mi, scale);

    while(SDL_PollEvent(&e) != 0) {
      switch(e.type) {
        case SDL_MOUSEMOTION: // fall through
        case SDL_QUIT: quit = true; break;
        default: break;
      }
    }


    scale *= 0.9;
  }

  close();

  return 0;
}
