#include "glib.h"
#include <algorithm>





Gviewer::Gviewer(){
	bpp = flags = gProgramID = gVBO = gIBO = 0;

	// Game data
	model_data_path = "models/rosslyn_building_interior.obj";
	vert_shader_path = "main.vert";
	frag_shader_path = "main.frag";
	texture_1_path = "models/megatexture.jpg";

	// Window dimensions
	width =      1440;
	height =     900;
	gWindow = NULL;

	gVertexPos3DLocation = -1;
	gViewMatrixLocation = -1;

	// GL camera
	FOV =         90;
	ZMIN =        0.01;
	ZMAX =        100.0;
	DIM =         1.8;
	th =          8.0;
	ph =          4.0;

	// Initial camera loc
	m_position = glm::vec3(0,1,0);
	m_direction = glm::vec3(0,0,1);
	x_rot = glm::mat4(1); y_rot = glm::mat4(1);

	mouseX = mouseY = 0;
}





bool Gviewer::initGL(){

	bool success = true;
	GLenum error = GL_NO_ERROR;
	// SDL params
	SDL_SetRelativeMouseMode(SDL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Clear screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	success = loadShaders();

	if (!success)
		return false;

	// 	LOAD RESOURCES

		// Textures
	glEnable(GL_TEXTURE_2D);
	IMG_Init(IMG_INIT_JPG);
	loadTexture(texture_1_path);
	// Bind textures to array 'textures'
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

		// Model Data
	
	// Load model data from path specified in model_data
	std::vector<glm::vec3> model_vertices;
	std::vector<glm::vec2> model_uvs;
	std::vector<glm::vec3> model_normals;
	std::vector<GLushort> model_elements;
	loadObj(model_data_path, model_vertices, model_uvs, model_normals, model_elements);
	
	uniform_mytexture = glGetUniformLocation(gProgramID, "tex");
	// Check if ID was received
	if (uniform_mytexture == -1){
		printf("Could not bind uniform_mytexture.");
		return false;
	}
	gTexCoordLocation = glGetAttribLocation(gProgramID, "texcoord");
	// Check if ID was received
	if (gTexCoordLocation == -1){
		printf("Could not bind gTexCoordLocation.");
		return false;
	}
	// VBO shader location	
	gVertexPos3DLocation = glGetAttribLocation(gProgramID, "LVertexPos3D");
        if (gVertexPos3DLocation == -1){
		printf("LVertexPos3D is unused or is not a valid glsl program variable.\n");
		return false;
        }
	gTimeLocation = glGetUniformLocation(gProgramID, "time");
        if (gVertexPos3DLocation == -1){
		printf("time is unused or is not a valid glsl program variable.\n");
		return false;
        }
	gFactorLocation = glGetUniformLocation(gProgramID, "factor");
        if (gVertexPos3DLocation == -1){
		printf("factor is unused or is not a valid glsl program variable.\n");
		return false;
        }
	factor =0.00;

	glUseProgram(gProgramID);


		// TEXTURE DATA

	// Send texture coordinate buffer
	glGenBuffers(1, &gTBO);
	glBindBuffer(GL_ARRAY_BUFFER, gTBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * model_uvs.size(), &model_uvs[0].x, GL_STATIC_DRAW);

		// MODEL DATA
	// Send Vertex buffer
	glGenBuffers(1, &gVBO);	
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(glm::vec3) * model_vertices.size(),	// GLfloat elements
		&model_vertices[0],
		GL_STATIC_DRAW
	);
	
	// Generate and send Index Buffer Object (IBO)
	glGenBuffers(1, &gIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIBO);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER, 
		(sizeof(GLushort) * model_elements.size()),	//GLushort elements
		&model_elements[0], 
		GL_STATIC_DRAW
	);

	// Model-view-projection matrix
	gViewMatrixLocation = glGetUniformLocation(gProgramID, "mvp");
	// Check if ID was received
	if (gViewMatrixLocation == -1){
		printf("Could not bind gViewMatrixLocation uniform.");
		return false;
	}

	glUseProgram(NULL);

	//Check for errors
	error = glGetError();

	if( error != GL_NO_ERROR )
	{
	    printf( "Error initializing OpenGL viewer! %s\n", gluErrorString(error) );
	    success = false;
	}

	return success;
}





void Gviewer::render()
{
	//Clear color buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Rotate view
	float angle = SDL_GetTicks() / 1000.0 * 5;
	viewMatrix = doView(0.0f, angle);

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
	
	// TEX COORDINATE ARRAYS
	// Enable vertex position
	glEnableVertexAttribArray(gTexCoordLocation);
	glBindBuffer(GL_ARRAY_BUFFER, gTBO);
	glVertexAttribPointer(
		gTexCoordLocation, 
		2, 
		GL_FLOAT, 
		GL_FALSE, 
		0, 
		0
	);

		// ELEMENT ARRAYS

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIBO);
	int bufsize; glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufsize);
	
	// Update shader inputs
	GLfloat time = (SDL_GetTicks() / 1000.0);
	glUniform1f(gTimeLocation, time);
	if (time > 10.0 && factor <=1.0)
		factor += 0.00001;
	glUniform1f(gFactorLocation, factor);

	glDrawElements(GL_TRIANGLES, bufsize/sizeof(GLushort), GL_UNSIGNED_SHORT, NULL);
	// Disable vertex position
	glDisableVertexAttribArray(gVertexPos3DLocation);
	// Unbind program
	glUseProgram(NULL);
	//Check for errors
	GLuint error = glGetError();
	if( error != GL_NO_ERROR )
	{
	    printf( "Error rendering OpenGL! %s\n", gluErrorString(error) );
	}
}






void Gviewer::handleEvent(SDL_Event * event)
{

	// If window event occurred
	if (event->type == 768){

		mouseX = event->motion.xrel;
		mouseY = event->motion.yrel;
		
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



void Gviewer::resize(){

	float asratio;
	if (height == 0) height = 1; //to avoid divide-by-zero
	asratio = width / (double) height;
	// Adjust GL viewport
	glViewport(0, 0, width, height);
}



void Gviewer::close(){
	
	for (int i=0; i<textures.size(); i++){
		glDeleteTextures(1, &textureid[i]);
	}
}





//-----PRIVATE FUNCTIONS-----//






bool Gviewer::loadShaders()
{
	bool success = false;	

	// Generate program
	gProgramID = glCreateProgram();

		// VERTEX SHADER
	// Create vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// Load vert shader in from file
	std::string line = "";
	std::string vert_string = "";
	std::ifstream vertshader (vert_shader_path);
	if (vertshader.is_open()){
		while ( getline (vertshader, line)){
			vert_string += line + '\n';
		}
		vertshader.close();
	}
	else{
		printf("Could not load vertex shader file");
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
	if( vShaderCompiled != GL_TRUE ){
		printf( "Unable to compile vertex shader %d!\n", vertexShader );
		printShaderLog( vertexShader );
		return false;
	}
	// Attach vertex shader to program
	glAttachShader(gProgramID, vertexShader);

		// FRAGMENT SHADER
	// Create fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	// Load fragment shader in from file
	line = "";
	std::string frag_string = "";
	std::ifstream fragshader (frag_shader_path);
	if (fragshader.is_open()){
		while ( getline (fragshader, line)){
			frag_string += line + '\n';
		}
		fragshader.close();
	}
	else{
		printf("Could not load fragment shader file");
		success = false;
	}
	const GLchar* fragmentShaderSource[frag_string.length()] = {frag_string.c_str()};
	// Set fragment source
	glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL);
	// Compile source
	glCompileShader(fragmentShader);
	// Check for errors in fragment shader
	GLint fShaderCompiled = GL_FALSE;
	glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &fShaderCompiled );
	if( fShaderCompiled != GL_TRUE ){
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

	return true; // Shaders successfully compiled
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
	else{
		textures.push_back(current);
	}
	return 0;
}


// OBJ loader that will generate a ready-to-use VBO, IBO, and UVs that economizes by sharing
// 	duplicate vertices and UV coordinates
void Gviewer::loadObj(std::string filename, std::vector<glm::vec3> &vertices, std::vector<glm::vec2> &uvs, std::vector<glm::vec3> &normals, std::vector<GLushort> &elements)
{
	// Check if .obj file exists
	std::ifstream in(filename.c_str(), std::ios::in);
	if (!in){
		std::cerr << "Cannot open " << filename << std::endl; exit(1);
	}
	
	// Establish vectors to hold model data
	std::vector<glm::vec3> file_vertices; std::vector<glm::vec2> file_uvs;
	std::string line;
	while (getline(in, line))
	{
		if (line.substr(0,2) == "v "){
			// Parse line
			std::vector<std::string> results;
			parseLine(line, results);	
			// Place parsed results in vector
			if (results.size() != 3){std::cerr << "Failed to read OBJ vert"; exit(1);};
			glm::vec3 temp;
			temp.x = stof(results[0]); temp.y = stof(results[1]); temp.z = stof(results[2]);
			file_vertices.push_back(temp);	
		}
		else if (line.substr(0,2) == "vt"){	
			std::vector<std::string> results;
			parseLine(line, results);
			if (results.size() != 2){std::cerr << "Failed to read OBJ vt"; exit(1);};
			glm::vec2 uv;
			uv.x = stof(results[0]); uv.y = 1 - stof(results[1]);
			file_uvs.push_back(uv);
		}
		else if (line.substr(0,2) == "s ")
		{
			break;
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
	// Establish uv vectors and continue the loop
	std::vector<std::vector<glm::vec2>> ordered_uvs(file_vertices.size());
	std::vector<glm::ivec2> elem_mapping;

	while (getline(in, line))
	{
		if (line.substr(0,2) == "f ")
		{				
			// Parse line
			std::vector<std::string> results;
			parseLine(line, results);
			GLushort a,b,c;
			if (results.size() == 6){ // If OBJ has no UVs associated, is not supported
				printf("OBJ must have UVs and normals exported"); exit(1);
			}
			else if (results.size() == 9){ // If UVs are included	
				// Load indices and UV coordinates
				for (int i = 0; i<9; i+=3){
					// Store indices
					GLushort in_index = stoi(results[i]); in_index--;
					elements.push_back(in_index);
					// Get uv coord at that index
					glm::vec2 in_coord = file_uvs[stoi(results[i+1]) - 1];
					bool match = true;
					int assoc_uvs = ordered_uvs[in_index].size();
					if (assoc_uvs > 0){
						// Check if the current coord is already loaded
						match = false;
						for (int j = 0; j<assoc_uvs; j++){
							if (in_coord.x == ordered_uvs[in_index][j].x
							 && in_coord.y == ordered_uvs[in_index][j].y){
								match = true;
								// Add mapping position
								glm::ivec2 map = glm::ivec2(in_index,j);
								elem_mapping.push_back(map);
							 }
						}
					}
					// Otherwise associate the uv coordinate with that vertex
					if (assoc_uvs == 0 || !match){
						ordered_uvs[in_index].push_back(in_coord);
						// Add mapping position
						glm::ivec2 map = glm::ivec2(in_index,assoc_uvs);
						elem_mapping.push_back(map);
					}
				}
			}
			else{
				std::cerr << "OBJ faces are in an unsupported format" << std::endl; exit(1);
			}
		}
		else if (line[0] == '#'){
			/* ignoring this line */
		}
		else{
			/* ignoring this line */
		}

	}

	normals.resize(file_vertices.size(), glm::vec3(0.0, 0.0, 0.0));
	for (int i = 0; i < elements.size(); i+=3)
	{
		GLushort ia = elements[i];
		GLushort ib = elements[i+1];
		GLushort ic = elements[i+2];
		glm::vec3 normal = glm::normalize(glm::cross(
		glm::vec3(file_vertices[ib]) - glm::vec3(file_vertices[ia]),
		glm::vec3(file_vertices[ic]) - glm::vec3(file_vertices[ia])));
		normals[ia] = normals[ib] = normals[ic] = normal;
	}

// Map element values to their absolute position in ordered_uvs
for (int i = 0; i<elements.size(); i++){
	int absolute_pos = 0;
	for (int j = 0; j<elem_mapping[i].x; j++){
		absolute_pos += ordered_uvs[j].size();
	}
	absolute_pos += elem_mapping[i].y;
	elements[i] = absolute_pos;
}
	// In order to support multiple UV coords and normals per vertex
	// Populate vectors with duplicate vertices by "flattening" 2d vectors
	for (int i = 0; i<ordered_uvs.size(); i++){
		for (int j = 0; j<ordered_uvs[i].size(); j++){
			uvs.push_back(ordered_uvs[i][j]);
			// Duplicate vertices
			vertices.push_back(file_vertices[i]);	
		}	
	}
}






// Output: Populates string vector "vert_inds", created and called in loadObj()
void Gviewer::parseLine(std::string line, std::vector<std::string> &vert_inds )
{	
	// Parse line
	bool match = false;
	bool prev_match = false;
	std::string seg = "";
	for (int i = 0; i< line.size(); i++){
		// Match criteria: '.','-',[0-9]
		match = ((line[i] >= 48 && line[i] <= 57) || line[i] == 46 || line[i] == 45);
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
}





glm::mat4 Gviewer::doView(float Translate, float angle){

	glm::mat4 Projection = glm::perspective(glm::radians(80.0f), 4.0f / 3.0f, 0.1f, 100.f);

	// X rotation
	glm::mat4 new_x_rot = glm::rotate(glm::mat4(1), glm::radians((float)mouseX*(-0.5f)), glm::vec3(0,1,0));
	x_rot = x_rot * new_x_rot;
	glm::vec3 y_pivot = glm::cross(m_direction,glm::vec3(0,1,0));

	// Y rotation
	glm::mat4 new_y_rot = glm::rotate(glm::mat4(1), glm::radians((float)mouseY*(0.5f)), y_pivot);
	y_rot = new_y_rot * y_rot;
	m_direction = glm::vec3(x_rot * y_rot * glm::vec4(0.0f,0.0f,1.0f, 1.0f));
	glm::mat4 View = glm::lookAt(m_position, m_position + m_direction, glm::vec3(0.0, 1.0, 0.0));
	
	// Mouse control
	mouseX = mouseY = 0;
	glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
	
	return Projection * View * Model;
}





void Gviewer::printProgramLog( GLuint program ){
    //Make sure name is shader
    if( glIsProgram( program ) ){
        //Program log length
        int infoLogLength = 0;
        int maxLength = infoLogLength;
        //Get info string length
        glGetProgramiv( program, GL_INFO_LOG_LENGTH, &maxLength );
        //Allocate string
        char* infoLog = new char[ maxLength ];
        //Get info log
        glGetProgramInfoLog( program, maxLength, &infoLogLength, infoLog );
        if( infoLogLength > 0 ){
            //Print Log
            printf( "%s\n", infoLog );
        }
        //Deallocate string
        delete[] infoLog;
    }
    else{
        printf( "Name %d is not a program\n", program );
    }
}





void Gviewer::printShaderLog( GLuint shader ){
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
