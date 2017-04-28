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

	
	glEnable(GL_DEPTH_TEST);
//	glEnable(GL_CULL_FACE);

	// Clear screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	


	success = loadShaders();

	if (!success)
		return false;



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

	
		
	// LOAD MODEL DATA

	std::vector<GLfloat> suzanne_vertices;
	std::vector<glm::vec3> suzanne_normals;
	std::vector<GLushort> suzanne_elements;
	std::string path = "models/suzanne.obj";

	loadObj(path, suzanne_vertices, suzanne_normals, suzanne_elements);

	//~suzanne_elements;
	//for (int i = 0; i<20; i++){
	//	printf("\n%f",*(&suzanne_vertices[0]+(i)));
	//	printf("\n    %f",suzanne_vertices[i]);
	//	printf("\n Array size: %d",(&suzanne_elements[1] - &suzanne_elements[0]));
	//	printf("GLfloat size: %d",sizeof(GLfloat));
	//}

	

//printf("Made it here");
//std::cin.ignore();

	//glm::vec3 v1 = glm::vec3(0.0,0.0,0.0);
	//glm::vec3 v2 = glm::vec3(1.0,1.0,0.0);
	//glm::vec3 v3 = glm::vec3(0.0,1.0,0.0);
	//GLushort e1 = 0; GLushort e2 = 1; GLushort e3 = 2;
	//suzanne_vertices.push_back(v1);suzanne_vertices.push_back(v2);suzanne_vertices.push_back(v3);
	//suzanne_elements.push_back(e1);suzanne_elements.push_back(e2);suzanne_elements.push_back(e3);

	//printf("Made it here. a=%d b=%d c=%d",a,b,c);



/*
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

*/


	glUseProgram(gProgramID);

	// VBO shader location	
	gVertexPos3DLocation = glGetAttribLocation(gProgramID, "LVertexPos3D");
       if (gVertexPos3DLocation == -1){
		printf("LVertexPos3D is unused or is not a valid glsl program variable.\n");
		return false;
       }
	// Send Vertex buffer
	glGenBuffers(1, &gVBO);	
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(GLfloat) * suzanne_vertices.size(),	// GLfloat elements
		&suzanne_vertices[0],
		GL_STATIC_DRAW
	);
//	glVertexAttribPointer(
//		gVertexPos3DLocation,	// attribute
//		4,			// # elements/vertex
//		GL_FLOAT,		// type
//		GL_FALSE,		// Take as-is
//		0,			// No extra data between each pos
//		0			// First element offset
//	);

	
/*  NEW

	// NBO shader location
	gNormal3DLocation = glGetAttribLocation(gProgramID, "LNormal3D");
       if (gNormal3DLocation == -1){
		printf("LNormal3D is unused or is not a valid glsl program variable.\n");
		return false;
       }
	// Send normal buffer
	glEnableVertexAttribArray(gNormal3DLocation);
	glBindBuffer(GL_ARRAY_BUFFER, gNBO);
	glVertexAttribPointer(
		gNormal3DLocation,
		3,			// # elements
		GL_FLOAT,
		GL_FALSE,
		0,
		0
	);
*/
	// Gen and send IBO
	glGenBuffers(1, &gIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIBO);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER, 
		(sizeof(GLushort) * suzanne_elements.size()),	//GLushort elements
		&suzanne_elements[0], 
		GL_STATIC_DRAW
	);




	// Model-view-projection matrix
	gViewMatrixLocation = glGetUniformLocation(gProgramID, "mvp");
	// Check if ID was received
	if (gViewMatrixLocation == -1){
		printf("Could not bind gViewMatrixLocation uniform.");
		return false;
	}
	//float angle = SDL_GetTicks() / 1000.0 * 15;
	//viewMatrix = doView(0.0f, angle);

	// * Then redraw in render * //



	





/*

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

*/

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
/*
		for (int i=0; i<textures.size(); i++){
			glActiveTexture(GL_TEXTURE0 + i);
			glUniform1i(uniform_mytexture, i);
			glBindTexture(GL_TEXTURE_2D, textureid[i]);
		}
*/
		// VERTEX ARRAYS
		// Enable vertex position
		glEnableVertexAttribArray(gVertexPos3DLocation);

		// Set vertex data
		glBindBuffer(GL_ARRAY_BUFFER, gVBO);		
		glVertexAttribPointer(
			gVertexPos3DLocation,	// attribute
			3,			// # elements/vertex
			GL_FLOAT,		// type
			GL_FALSE,		// Take as-is
			0,			// No extra data between each pos
			0			// First element offset
		);
		
		
		glUniformMatrix4fv(gViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		
/*		// TEX COORDINATE ARRAYS
		// Enable vertex position
		glEnableVertexAttribArray(attribute_texcoord);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_plane_texcoords);
		glVertexAttribPointer(attribute_texcoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
*/
		// ELEMENT ARRAYS

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIBO);
		int bufsize; glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufsize);
		glDrawElements(GL_TRIANGLES, bufsize/sizeof(GLushort), GL_UNSIGNED_SHORT, NULL);

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
    glm::mat4 View = glm::lookAt(glm::vec3(0.6, 0.6, 0.6), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

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




void Gviewer::loadObj(std::string filename, std::vector<GLfloat> &vertices, std::vector<glm::vec3> &normals, std::vector<GLushort> &elements)
{
	std::ifstream in(filename.c_str(), std::ios::in);
	if (!in)
	{
		std::cerr << "Cannot open " << filename << std::endl; exit(1);
	}

	std::string line;
	while (getline(in, line))
	{
		if (line.substr(0,2) == "v ")
		{
			std::istringstream s(line.substr(2));
			GLfloat v;

			// Parse line
			bool match = false;
			bool prev_match = false;
			std::string seg = "";
			for (int i = 0; i< line.size(); i++){
				match = ((line[i] >= 48 && line[i] <= 57) || line[i] == 46 || line[i] == 45);
				if ((!match && prev_match) || (i == line.size()-1)){
					vertices.push_back(stof(seg));
					seg = "";
					prev_match = false;
				}
				else if (match){
					seg = seg + line[i];
					prev_match = true;
				}	
			}
			//s >> v;  vertices.push_back(v);
			//s >> v;  vertices.push_back(v);
			//s >> v;  vertices.push_back(v);
			
		}
		else if (line.substr(0,2) == "f ")
		{	
			GLushort a,b,c;
			std::vector<std::string> vert_inds;

			// Parse line
			bool match = false;
			bool prev_match = false;
			std::string seg = "";
			for (int i = 0; i< line.size(); i++){
				match = (line[i] >= 48 && line[i] <= 57);
				if ((!match && prev_match) || (i == line.size()-1)){
					vert_inds.push_back(seg);
					seg = "";
					prev_match = false;
				}
				else if (match){
					seg = seg + line[i];
					prev_match = true;
				}	
			}


			a = stoi(vert_inds[0]); b = stoi(vert_inds[2]); c = stoi(vert_inds[4]);
			a--; b--; c--;
		
		elements.push_back(a); elements.push_back(b); elements.push_back(c);
		}
		else if (line[0] == '#')
		{
			/* ignoring this line */
		}
		else
		{
			/* ignoring this line */
		}

	}

	normals.resize(vertices.size(), glm::vec3(0.0, 0.0, 0.0));


	for (int i = 0; i < elements.size(); i+=3)
	{
		GLushort ia = elements[i];
		GLushort ib = elements[i+1];
		GLushort ic = elements[i+2];
	
		glm::vec3 normal = glm::normalize(glm::cross(
		glm::vec3(vertices[ib]) - glm::vec3(vertices[ia]),
		glm::vec3(vertices[ic]) - glm::vec3(vertices[ia])));
		normals[ia] = normals[ib] = normals[ic] = normal;

	}

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
