#ifndef GVIEWER_H
#define GVIEWER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <GL/glew.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>


class Gviewer
{
  private:

    bool gRenderQuad; 		//Render flag

  public:
    int width;    		// Window width
    int height;			// height
    int bpp;			// Color depth
    int flags;			// Rendering flags
    SDL_Window* gWindow; 	//The window we'll be rendering to
    SDL_GLContext gContext; 	//OpenGL context

    // GL variables
    GLuint gProgramID;
    GLint gViewMatrixLocation;
    GLint gVertexPos3DLocation;
    GLuint gVBO;
    GLuint gIBO;
    int FOV;
    float ZMIN,ZMAX,DIM,th,ph;
    glm::mat4 viewMatrix;

    // Game files
    std::vector <SDL_Surface*> textures;
    std::vector <GLuint> textureid;
    GLint uniform_mytexture;


    Gviewer();


    //Initializes matrices and clear color
    bool initGL();

    //Input handler
    void handleKeys( unsigned char key, int x, int y );

    //Per frame update
    void update();

    //Renders quad to the screen
    void render();

    // Handle events for a variety of SDL actions
    void handleEvent(SDL_Event * event);

    //Frees media and shuts down SDL
    void close();

    // Resizes window
    void resize();

    glm::mat4 doView(float Translate, float angle);

    int loadTexture(std::string filename);

    //Shader loading utility programs
    void printProgramLog( GLuint program );
    void printShaderLog( GLuint shader );

};

#endif
