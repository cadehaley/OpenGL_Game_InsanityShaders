#include "glib.h"

Gviewer::Gviewer(){
  bpp = flags = gProgramID = gVBO = gIBO = 0;

  // Window dimensions
  width =      1080;
  height =     720;
  gWindow = NULL;

  // What to draw
  gRenderQuad = true;

  gVertexPos3DLocation = -1;
  gViewMatrixLocation = -1;

  // GL camera
  FOV =         90;
  ZMIN =        0.01;
  ZMAX =        100.0;
  DIM =         1.8;
  th =          8.0;
  ph =          4.0;
}

bool Gviewer::initGL()
{
	bool success = true;
	GLenum error = GL_NO_ERROR;

	
	// 	LOAD RESOURCES
	// Textures
	glEnable(GL_TEXTURE_2D);

	IMG_Init(IMG_INIT_JPG);
	loadTexture("box.jpg");

	for (int i = 0; i<textures.size(); i++){
		textureid.push_back(-1);
		glGenTextures(1, &textureid[i]);
		glBindTexture(GL_TEXTURE_2D, textureid[i]);
		int Mode = GL_RGB;
		if (textures[i]->format->BytesPerPixel == 4){
			Mode = GL_RGBA;
		}
		
	
		glTexImage2D(GL_TEXTURE_2D,
			0, // level, 0=base, no minimap
			Mode,
			textures[i]->w,
			textures[i]->h,
			0,
			Mode,
			GL_UNSIGNED_BYTE,
			textures[i]->pixels
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		SDL_FreeSurface(textures[i]);
	}

	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Clear screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	


	success = loadShaders();

	if (!success)
		return false;
	


	  // Now attach and link
		
	// VERTEX DATA

	//VBO data
	GLfloat vertexData[] =
	{
	    -0.5f, -0.5f, 0.0f,
	     0.5f, -0.5f, 0.0f,
	     0.5f,  0.5f, 0.0f,
	    -0.5f,  0.5f, 0.0f
	};


	GLuint indexData[] = { 0, 1, 2, 3 };

	// Create VBO
	glGenBuffers(1, &gVBO);
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	// TEXTURE COORDINATES
	GLfloat plane_texcoords[] = {
		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,
	};

	glGenBuffers(1, &vbo_plane_texcoords);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_plane_texcoords);
	glBufferData(GL_ARRAY_BUFFER, sizeof(plane_texcoords), plane_texcoords, GL_STATIC_DRAW);


       gVertexPos3DLocation = glGetAttribLocation(gProgramID, "LVertexPos3D");
       if (gVertexPos3DLocation == -1){
		printf("LVertexPos2d is unused or is not a valid glsl program variable.\n");
		return false;
       }

	// Create IBO
	glGenBuffers(1, &gIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), indexData, GL_STATIC_DRAW);

	glUseProgram(gProgramID);
	// Send model-view-projection matrix

	float angle = SDL_GetTicks() / 1000.0 * 15;
	viewMatrix = doView(0.0f, angle);


	gViewMatrixLocation = glGetUniformLocation(gProgramID, "mvp");
	// Check if ID was received
	if (gViewMatrixLocation == -1){
		printf("Could not bind gViewMatrixLocation uniform.");
		return false;
	}

	uniform_mytexture = glGetUniformLocation(gProgramID, "tex");
	// Check if ID was received
	if (uniform_mytexture == -1){
		printf("Could not bind uniform_mytexture.");
		return false;
	}


	attribute_texcoord = glGetAttribLocation(gProgramID, "texcoord");
	// Check if ID was received
	if (attribute_texcoord == -1){
		printf("Could not bind attribute_texcoord.");
		return false;
	}

	glUseProgram(NULL);


	  
      
  

	  std::cout << glGetString(GL_VERSION) << std::endl;
	  //Check for error
	  error = glGetError();

	  if( error != GL_NO_ERROR )
	  {
	      printf( "Error initializing OpenGL viewer! %s\n", gluErrorString(error) );
	      success = false;
	  }


	  return success;
}



void Gviewer::handleKeys(unsigned char key, int x, int y){
  // Toggle quad
  if (key == 'q'){
    gRenderQuad = !gRenderQuad;
  }
}


void Gviewer::render()
{
	//Clear color buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glClearColor(0.5, 0.5, 1.0, 1.0);

	// Rotate view
	float angle = SDL_GetTicks() / 1000.0 * 40;
	viewMatrix = doView(0.0f, angle);




	//Render quad
	if( gRenderQuad )
	{

		// Bind program
		glUseProgram(gProgramID);

		// Bind textures

		for (int i=0; i<textures.size(); i++){
			glActiveTexture(GL_TEXTURE0 + i);
			glUniform1i(uniform_mytexture, i);
			glBindTexture(GL_TEXTURE_2D, textureid[i]);
		}

		// VERTEX ARRAYS
		// Enable vertex position
		glEnableVertexAttribArray(gVertexPos3DLocation);
		glUniformMatrix4fv(gViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		// Set vertex data
		glBindBuffer(GL_ARRAY_BUFFER, gVBO);
		glVertexAttribPointer(gVertexPos3DLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
		

		// TEX COORDINATE ARRAYS
		// Enable vertex position
		glEnableVertexAttribArray(attribute_texcoord);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_plane_texcoords);
		glVertexAttribPointer(attribute_texcoord, 2, GL_FLOAT, GL_FALSE, 0, 0);

		// Set index data and Render
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIBO);
		int bufsize; glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufsize);
		glDrawElements(GL_TRIANGLE_FAN, bufsize/sizeof(GLuint), GL_UNSIGNED_INT, NULL);

		// Disable vertex position
		glDisableVertexAttribArray(gVertexPos3DLocation);

		// Unbind program
		glUseProgram(NULL);


	}


  //Check for error
  GLuint error = glGetError();

  if( error != GL_NO_ERROR )
  {
      printf( "Error rendering OpenGL! %s\n", gluErrorString(error) );
  }
}


void Gviewer::handleEvent(SDL_Event * event)
{
    // Window events

    // If window event occurred
    if (event->type == 768){
        switch (event->window.event){
            // Resize window
            case SDL_WINDOWEVENT_RESIZED:
                width = event->window.data1;
                height = event->window.data2;
                resize();
                break;
            default:
                break;
        }
    }
}


void Gviewer::close(){
	
	for (int i=0; i<textures.size(); i++){
		glDeleteTextures(1, &textureid[i]);
	}
}


void Gviewer::resize(){
    float asratio;

    if (height == 0) height = 1; //to avoid divide-by-zero

    asratio = width / (double) height;

    glViewport(0, 0, width, height); //adjust GL viewport

/*
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    double ydim = ZMIN*tan(FOV*M_PI/360);
    double xdim = ydim*asratio;
    glFrustum(-xdim,+xdim,-ydim,+ydim,ZMIN,ZMAX);
//    gluPerspective(FOV, asratio, ZMIN, ZMAX); //adjust perspective
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
*/
}


glm::mat4 Gviewer::doView(float Translate, float angle)
{
    glm::mat4 Projection = glm::perspective(glm::radians(80.0f), 4.0f / 3.0f, 0.1f, 100.f);
    glm::mat4 View = glm::lookAt(glm::vec3(0.3, 0.3, 0.3), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

    glm::vec3 axis_y(0,1,0);
    glm::mat4 anim = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis_y);

//    View = glm::rotate(View, anim, glm::vec3(-1.0f, 0.0f, 0.0f));
//    View = glm::rotate(View, anim, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
    return Projection * View * Model * anim;
}

void Gviewer::printProgramLog( GLuint program )
{
    //Make sure name is shader
    if( glIsProgram( program ) )
    {
        //Program log length
        int infoLogLength = 0;
        int maxLength = infoLogLength;

        //Get info string length
        glGetProgramiv( program, GL_INFO_LOG_LENGTH, &maxLength );

        //Allocate string
        char* infoLog = new char[ maxLength ];

        //Get info log
        glGetProgramInfoLog( program, maxLength, &infoLogLength, infoLog );
        if( infoLogLength > 0 )
        {
            //Print Log
            printf( "%s\n", infoLog );
        }

        //Deallocate string
        delete[] infoLog;
    }
    else
    {
        printf( "Name %d is not a program\n", program );
    }
}



// Returns texture id
int Gviewer::loadTexture(std::string filename){
	int tex_idx;
	
	// Load image
	SDL_Surface * current = IMG_Load(filename.c_str());
	if (current == NULL){
		printf("Failed to load texture %s : %s", filename.c_str(), SDL_GetError());
		return -1;
	}
	else
	{
		textures.push_back(current);
	}
	
}




bool Gviewer::loadShaders()
{
	bool success = false;	

	// Generate program
	gProgramID = glCreateProgram();

	// Now the vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	


	// Load vert shader in from file
	std::string line = "";
	std::string vert_string = "";
	std::ifstream vertshader ("main.vert");
	if (vertshader.is_open()){
		while ( getline (vertshader, line)){
			vert_string += line + '\n';
		}
		vertshader.close();
	}
	else{
		printf("Could not load main.vert");
		success = false;
	}
	const GLchar* vertexShaderSource[vert_string.length()] = {vert_string.c_str()};
	



	//Set vertex source
	glShaderSource( vertexShader, 1, vertexShaderSource, NULL );

	//Compile vertex source
	glCompileShader( vertexShader );

	//Check vertex shader for errors
	GLint vShaderCompiled = GL_FALSE;
	glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &vShaderCompiled );
	if( vShaderCompiled != GL_TRUE )
	{
	    printf( "Unable to compile vertex shader %d!\n", vertexShader );
	    printShaderLog( vertexShader );
	    return false;
	}
	// Attach vertex shader to program
	glAttachShader(gProgramID, vertexShader);

	// Create fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		
	    

	// Load fragment shader in from file
	line = "";
	std::string frag_string = "";
	std::ifstream fragshader ("main.frag");
	if (fragshader.is_open()){
		while ( getline (fragshader, line)){
			frag_string += line + '\n';
		}
		fragshader.close();
	}
	else{
		printf("Could not load main.frag");
		success = false;
	}
	const GLchar* fragmentShaderSource[frag_string.length()] = {frag_string.c_str()};




	// Set fragment source
	glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL);

	// Compile source
	glCompileShader(fragmentShader);

	// Error check fragment shader
	GLint fShaderCompiled = GL_FALSE;
	glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &fShaderCompiled );
	if( fShaderCompiled != GL_TRUE )
	{
	    printf( "Unable to compile fragment shader %d!\n", fragmentShader );
	    printShaderLog( fragmentShader );
	    success = false;
	}
	else{
	  // Attach fragment shader to program
	  glAttachShader( gProgramID, fragmentShader );

	  // Link program
	  glLinkProgram( gProgramID );

	  // Error Check
	  GLint programSuccess = GL_TRUE;
	  glGetProgramiv( gProgramID, GL_LINK_STATUS, &programSuccess);
	  if (programSuccess != GL_TRUE){
	    printf( "Error linking program %d!\n", gProgramID);
	    printProgramLog(gProgramID);
	    return false;
	  }
	}	
	return true;
}


void Gviewer::printShaderLog( GLuint shader )
{
    //Make sure name is shader
    if( glIsShader( shader ) )
    {
        //Shader log length
        int infoLogLength = 0;
        int maxLength = infoLogLength;

        //Get info string length
        glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &maxLength );

        //Allocate string
        char* infoLog = new char[ maxLength ];

        //Get info log
        glGetShaderInfoLog( shader, maxLength, &infoLogLength, infoLog );
        if( infoLogLength > 0 )
        {
            //Print Log
            printf( "%s\n", infoLog );
        }

        //Deallocate string
        delete[] infoLog;
    }
    else
    {
        printf( "Name %d is not a shader\n", shader );
    }
}
