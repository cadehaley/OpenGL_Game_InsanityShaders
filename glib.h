#ifndef GVIEWER_H
#define GVIEWER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <GL/glew.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>

// GLM matrix helper functions
#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "glm/gtx/vector_angle.hpp" // glm::orientedAngle
#include "glm/gtc/type_ptr.hpp"


class Gviewer
{
    public:
	// SDL Initialization
	int width;    		// Window width
	int height;			// height
	int bpp;			// Color depth
	int flags;			// Rendering flags
	SDL_Window* gWindow; 	//The window we'll be rendering to
	SDL_GLContext gContext; 	//OpenGL context


	// Game variables
	int mouseX, mouseY;
	GLfloat factor;


	//-----Public Functions-----//


	/**
	// Gviewer constructor
	//	Initializes camera placement, mouse position, and window dimensions
	//	(no paramaters)
	//	(void)
	*/
	Gviewer();
	
	/**
	// initGL
	//	Initializes camera placement, mouse position, and window dimensions
	//	(no paramaters)
	//	(void)
	*/
	bool initGL();

	
	/**
	// render
	//	Updates camera placement, vertex and texture data, then draws image
	//	(no paramaters)
	//	(void)
	*/
	void render();


	/**
	// handleEvent
	//	Maps SDL window events to graphics actions
	//	@param event - Type of SDL action that occurred
	//	(void)
	*/
	// Handle events for a variety of SDL actions
	void handleEvent(SDL_Event * event);


	/**
	// resize
	//	Resizes OpenGL viewport
	//	(no paramaters)
	//	(void)
	*/
	void resize();


	/**
	// close
	//	Frees media and shuts down SDL
	//	(no paramaters)
	//	(void)
	*/
	void close();




    private:

	// GL variables
	GLuint gProgramID;
	GLint gViewMatrixLocation;
	GLint gVertexPos3DLocation;
	GLint gNormal3DLocation;
	GLint gTimeLocation;
	GLint gFactorLocation;
	GLuint gVBO;
	GLuint gNBO;
	GLuint gIBO;

	int FOV;
	float ZMIN,ZMAX,DIM,th,ph;
	glm::vec3 m_position, m_direction;
	glm::mat4 x_rot; glm::mat4 y_rot;
	glm::mat4 viewMatrix;

	// Game files
	std::string vert_shader_path;
	std::string frag_shader_path;
	std::string model_data_path;
	std::string texture_1_path;
	std::vector <SDL_Surface*> textures;
	std::vector <GLuint> textureid;
	GLint gTexCoordLocation;
	GLuint gTBO;
	GLint uniform_mytexture;


	//-----Private Functions-----//

	/**
	// loadShaders
	//	Compiles and links vertex and fragment shaders	
	//	(no paramaters)
	//	(void)
	*/
	bool loadShaders();


	/**
	// loadTexture
	//	Loads texture file as an SDL surface
	//	@param filename - texture path
	//	@return success code, 0=success, -1=failure
	*/
	int loadTexture(std::string filename);


	/**
	// loadObj
	//	Loads OBJ model data into OpenGL-compatible vectors
	//	@param filename - OBJ file path
	//	@param vertices - Address of an XYZ vector for vertices
	//	@param uvs      - Address of an XY vector for uvs
	//	@param normals  - Address of an XYZ vector for vertex normals
	//	@param elements - Address of an integer vector for face-drawing order
	//	(void)
	*/
	void loadObj(std::string filename, std::vector<glm::vec3> &vertices, std::vector<glm::vec2> &uvs, std::vector<glm::vec3> &normals, std::vector<GLushort> &elements);


	/**
	// parseLine
	//	Sub-function of loadObj for parsing OBJ lines
	//	@param line 	 - line of input in need of parsing
	//	@param vert_inds - vector of parsed strings
	*/
	void parseLine(std::string line, std::vector<std::string> &vert_inds );


	/**
	// doView
	//	Creates model-view-projection matrix from camera location and angle
	//	@param Translate -  location of camera
	//	@param angle	 -  camera angle
	*/
	glm::mat4 doView(float Translate, float angle);

	//Shader compiling helper functions
	void printProgramLog( GLuint program );
	void printShaderLog( GLuint shader );

};

#endif
