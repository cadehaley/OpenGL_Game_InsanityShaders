#ifndef GVIEWER_H
#define GVIEWER_H

#include <GL/glew.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <iostream>

class Gviewer
{
  private:

    bool gRenderQuad; //Render flag

  public:
    int width;    // Window width
    int height;   // height
    int bpp;      // Color depth
    int flags;    // Rendering flags
    SDL_Window* gWindow; //The window we'll be rendering to
    SDL_GLContext gContext; //OpenGL context

    GLuint gProgramID;
    GLint gVertexPos2DLocation;
    GLuint gVBO;
    GLuint gIBO;

    Gviewer();


    //Initializes matrices and clear color
    bool initGL();

    //Input handler
    void handleKeys( unsigned char key, int x, int y );

    //Per frame update
    void update();

    //Renders quad to the screen
    void render();

    //Frees media and shuts down SDL
    void close();

    //Shader loading utility programs
    void printProgramLog( GLuint program );
    void printShaderLog( GLuint shader );

};

#endif
