#ifdef __cplusplus
    #include <cstdlib>
#else
    #include <stdlib.h>

#endif
#ifdef __APPLE__
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif
#include <math.h>
#define pi 3.14
#include <time.h>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <map>
#include <cstring>
using namespace std;

SDL_Surface *screen;
int width = 900;
int height = 600;
char const* tytul = "";

struct RGB {
    Uint8 r;
    Uint8 g;
    Uint8 b;
};

void setPixel(int x, int y, Uint8 R, Uint8 G, Uint8 B);
SDL_Color getPixel (int x, int y);
void czyscEkran(Uint8 R, Uint8 G, Uint8 B);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////               ////////////////////////////////////////////////////////////////////
///////////////////////////////////     KODER     ////////////////////////////////////////////////////////////////////
///////////////////////////////////               ////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int unsignedModulo(int i, int mod)
{
    if(i >= mod)
        i -= mod;
    else if(i < 0)
        i += mod;
    return i;
}

void FilterRoznicy(RGB & pixel, RGB upperPixel, int m = 1){ //symulacja filtra ró¿nicy dla kolorów
    int r = pixel.r - upperPixel.r * m;
    int g = pixel.g - upperPixel.g * m;
    int b = pixel.b - upperPixel.b * m;
    pixel.r = unsignedModulo(r, 256);
    pixel.g = unsignedModulo(g, 256);
    pixel.b = unsignedModulo(b, 256);
}

void Filter() {
    for(int i=0; i<width; i++){
        RGB upperPixel;
        upperPixel.r = 0;
        upperPixel.g = 0;
        upperPixel.b = 0;
        for(int j=0; j<height; j++){
            RGB c;
            SDL_Color color = getPixel(i, j);
            c.r = color.r;
            c.g = color.g;
            c.b = color.b;

            RGB fc = c;
            FilterRoznicy(fc, upperPixel); //symulacja wygladu filtra ró¿nicy linii
            setPixel(i, j, fc.r,fc.g,fc.b);

            upperPixel = c;
        }
    }

    SDL_Flip(screen);
}

void ReverseFilter() {
    for(int i=0; i<width; i++){
        RGB upperPixel;
        upperPixel.r = 0;
        upperPixel.g = 0;
        upperPixel.b = 0;
        for(int j=0; j<height; j++){
            RGB c;
            SDL_Color color = getPixel(i, j);
            c.r = color.r;
            c.g = color.g;
            c.b = color.b;

            RGB fc = c;
            FilterRoznicy(fc, upperPixel, -1); //symulacja wygladu filtra ró¿nicy linii
            setPixel(i, j, fc.r,fc.g,fc.b);

            upperPixel = fc;
        }
    }

    SDL_Flip(screen);
}

void setPixel(int x, int y, Uint8 R, Uint8 G, Uint8 B)
{
  if ((x>=0) && (x<width) && (y>=0) && (y<height))
  {
    /* Zamieniamy poszczególne sk³adowe koloru na format koloru pixela */
    Uint32 pixel = SDL_MapRGB(screen->format, R, G, B);

    /* Pobieramy informacji ile bajtów zajmuje jeden pixel */
    int bpp = screen->format->BytesPerPixel;

    /* Obliczamy adres pixela */
    Uint8 *p = (Uint8 *)screen->pixels + y * screen->pitch + x * bpp;

    /* Ustawiamy wartoœæ pixela, w zale¿noœci od formatu powierzchni*/
    switch(bpp)
    {
        case 1: //8-bit
            *p = pixel;
            break;

        case 2: //16-bit
            *(Uint16 *)p = pixel;
            break;

        case 3: //24-bit
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            } else {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;

        case 4: //32-bit
            *(Uint32 *)p = pixel;
            break;

    }
         /* update the screen (aka double buffering) */
  }
}

void ladujBMP(char const* nazwa, int x, int y)
{
    SDL_Surface* bmp = SDL_LoadBMP(nazwa);
    if (!bmp)
    {
        printf("Unable to load bitmap: %s\n", SDL_GetError());
    }
    else
    {
        width = bmp->w;
        height = bmp->h;
        screen = SDL_SetVideoMode(width, height, 32,SDL_HWSURFACE|SDL_DOUBLEBUF);
        SDL_Rect dstrect;
        dstrect.x = x;
        dstrect.y = y;
        SDL_BlitSurface(bmp, 0, screen, &dstrect);
        SDL_Flip(screen);
        SDL_FreeSurface(bmp);
    }
}

void czyscEkran(Uint8 R, Uint8 G, Uint8 B)
{
    SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, R, G, B));
    SDL_Flip(screen);
}

SDL_Color getPixel (int x, int y) {
    SDL_Color color ;
    Uint32 col = 0 ;
    if ((x>=0) && (x<width) && (y>=0) && (y<height)) {
        //determine position
        char* pPosition=(char*)screen->pixels ;
        //offset by y
        pPosition+=(screen->pitch*y) ;
        //offset by x
        pPosition+=(screen->format->BytesPerPixel*x);
        //copy pixel data
        memcpy(&col, pPosition, screen->format->BytesPerPixel);
        //convert color
        SDL_GetRGB(col, screen->format, &color.r, &color.g, &color.b);
    }
    return ( color ) ;
}


int main ( int argc, char** argv )
{
    // console output
    freopen( "CON", "wt", stdout );
    freopen( "CON", "wt", stderr );

    // initialize SDL video
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "Unable to init SDL: %s\n", SDL_GetError() );
        return 1;
    }

    // make sure SDL cleans up before exit
    atexit(SDL_Quit);

    // create a new window
    screen = SDL_SetVideoMode(width, height, 32,SDL_HWSURFACE|SDL_DOUBLEBUF);
    //screen=SDL_SetVideoMode(width, height, 8, SDL_HWPALETTE);
    if ( !screen )
    {
        printf("Unable to set video: %s\n", SDL_GetError());
        return 1;
    }

    SDL_WM_SetCaption( tytul , NULL );
    // program main loop
        cout<< "a,s,d,f - wczytaj obrazek1,2,3,4.bmp" << endl;
        cout<< "1. Zastosuj filtr roznicy linii"<< endl;
        cout<< "2. Odwroc filtr roznicy linii"<< endl;
    bool done = false;
    while (!done)
    {
        // message processing loop
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {

            // check for messages
            switch (event.type)
            {
                // exit if the window is closed
            case SDL_QUIT:
                done = true;
                break;

                // check for keypresses
            case SDL_KEYDOWN:
                {
                    // exit if ESCAPE is pressed
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                        done = true;
                    if (event.key.keysym.sym == SDLK_1)
                        Filter();
                    if (event.key.keysym.sym == SDLK_2)
                        ReverseFilter();
                    if (event.key.keysym.sym == SDLK_a)
                        ladujBMP("obrazek1.bmp", 0, 0);
                    if (event.key.keysym.sym == SDLK_s)
                        ladujBMP("obrazek2.bmp", 0, 0);
                    if (event.key.keysym.sym == SDLK_d)
                        ladujBMP("obrazek3.bmp", 0, 0);
                    if (event.key.keysym.sym == SDLK_f)
                        ladujBMP("obrazek4.bmp", 0, 0);
                    if (event.key.keysym.sym == SDLK_b)
                        czyscEkran(0, 0, 0);          break;
                     }
            } // end switch
        } // end of message processing

    } // end main loop


    // all is well ;)
    printf("Exited cleanly\n");
    return 0;
}
