// Resource: http://lazyfoo.net/tutorials/SDL/01_hello_SDL/linux/cli/index.php

//Using SDL and standard IO
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <stdio.h>
#include <iostream>
#include "glib.h"

using namespace std;


int main( int argc, char* args[] )
{
	// Initialization flag
	bool success = true;

	// Create viewer Object
	Gviewer viewer;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Use OpenGL 2.1
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );

		//Create window
		viewer.gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, viewer.width, viewer.height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
		if( viewer.gWindow == NULL )
		{
			printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			// Create Context
			viewer.gContext = SDL_GL_CreateContext( viewer.gWindow );

			if ( viewer.gContext == NULL ){
				printf( "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError() );
        success = false;
			}
			else{
				// Use vsync
				if ( SDL_GL_SetSwapInterval(1) < 0 ){
					printf("Warning: Unable to set Vsync. SDL Error: %s\n", SDL_GetError());
				}

				// Initialize OpenGL
				if ( !viewer.initGL() ){
					printf("Unable to initialize OpenGL!\n");
					success = false;
				}
			}

			//Fill the surface white
			//SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0xFF, 0xFF, 0xFF ) );

			//Update the surface
			//SDL_UpdateWindowSurface( window );

			//Wait two seconds
			//SDL_Delay( 2000 );
		}
	}

	//Destroy window
	//SDL_DestroyWindow( window );

	//Quit SDL subsystems
	//SDL_Quit();

	return success;
}
