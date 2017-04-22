// Resource: http://lazyfoo.net/tutorials/SDL/01_hello_SDL/linux/cli/index.php

//Using SDL and standard IO
#include <GL/glew.h>
#include <GL/glu.h>
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
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ) < 0 )
	{
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{


		//Use OpenGL 2.1
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
		// OpenGL prefs
		//SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
		//SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
		//SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
		//SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
		SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );


		//Create window
		viewer.gWindow = SDL_CreateWindow( "Insanity", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, viewer.width, viewer.height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE );
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

				// Use OpenGL 3.0 and later
				glewExperimental = GL_TRUE;
				// Initialize GLEW
				GLenum GlewInitResult = glewInit();
				// Error check
				if (GLEW_OK != GlewInitResult)
				{
					printf("ERROR: %s\n",glewGetErrorString(GlewInitResult));
					exit(EXIT_FAILURE);
				}



				// Initialize OpenGL
				if ( !viewer.initGL() )
				{
					printf("Unable to initialize OpenGL!\n");
					success = false;
				}
				else
				{
					bool running = true;


					while (running){
						SDL_Event event;
						while (SDL_PollEvent(&event)){
								// If user quits
								if (event.type == SDL_QUIT){
									running = false;
								}

								if (event.type = SDL_KEYDOWN){
								}

								// Pass event to event handler
								viewer.handleEvent(&event);

								viewer.render();
								SDL_GL_SwapWindow(viewer.gWindow);

						}
					}
				}
			}
		}
	}

	//Destroy window
	SDL_DestroyWindow( viewer.gWindow );

	//Quit SDL subsystems
	SDL_Quit();

	return success;
}
