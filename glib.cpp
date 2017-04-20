#include "glib.h"

Gviewer::Gviewer(){
  bpp = flags = gProgramID = gVBO = gIBO = 0;

  width = 1080;
  height = 720;
  gWindow = NULL;
  gRenderQuad = true;

  gVertexPos2DLocation = -1;
}

bool Gviewer::initGL(){
  bool success = true;
  GLenum error = GL_NO_ERROR;


  glClearColor(1.0, 1.0, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  // Swap back and front buffer
  SDL_GL_SwapWindow(gWindow);


  // Generate program
  gProgramID = glCreateProgram();

  // Now the vertex shader
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);


  //Get vertex source
  const GLchar* vertexShaderSource[] =
  {
      "#version 140\nin vec2 LVertexPos2D; void main() { gl_Position = vec4( LVertexPos2D.x, LVertexPos2D.y, 0, 1 ); }"
  };

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
      success = false;
  }
  else{
    // Attach vertex shader to program
      glAttachShader(gProgramID, vertexShader);

      // Create fragment shader
      GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

      // Get fragment source
      const GLchar* fragmentShaderSource[] =
      {
            "#version 140\nout vec4 LFragment; void main() { LFragment = vec4( 1.0, 1.0, 1.0, 1.0 ); }"
      };

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
            success = false;
          }
          else{
              // Now attach and link
              // Get vertex attribute location
              gVertexPos2DLocation = glGetAttribLocation(gProgramID, "LVertexPos2D");
              if (gVertexPos2DLocation == -1){
                printf("LVertexPos2d is not a valid glsl program variable.\n");
                success = false;
              }
              else{
                  // Now that it is linked, send the vertex data
                  //Initialize clear color
                    glClearColor( 0.f, 0.f, 0.f, 1.f );

                    //VBO data
                    GLfloat vertexData[] =
                    {
                        -0.5f, -0.5f,
                         0.5f, -0.5f,
                         0.5f,  0.5f,
                        -0.5f,  0.5f
                    };

                    //IBO data
                    GLuint indexData[] = { 0, 1, 2, 3 };

                    // Create VBO
                    glGenBuffers(1, &gVBO);
                    glBindBuffer(GL_ARRAY_BUFFER, gVBO);
                    glBufferData(GL_ARRAY_BUFFER, 2 * 4 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);

                    // Create IBO
                    glGenBuffers(1, &gIBO);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIBO);
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), indexData, GL_STATIC_DRAW);
              }
          }
      }
  }

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



void Gviewer::render(){
  //Clear color buffer
  glClear( GL_COLOR_BUFFER_BIT );

  //Render quad
  if( gRenderQuad )
  {

      // Bind program
      glUseProgram(gProgramID);

      // Enable vertex position
      glEnableVertexAttribArray(gVertexPos2DLocation);

      // Set vertex data
      glBindBuffer(GL_ARRAY_BUFFER, gVBO);
      glVertexAttribPointer(gVertexPos2DLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);

      // Set index data and Render
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIBO);
      glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, NULL);

      // Disable vertex position
      glDisableVertexAttribArray(gVertexPos2DLocation);

      // Unbind program
      glUseProgram(NULL);

      //  Axes for reference
      glColor3f(0,1,1);
      glBegin(GL_LINES);
      glVertex3f(0,0,0);
      glVertex3f(2,0,0);
      glVertex3f(0,0,0);
      glVertex3f(0,2,0);
      glVertex3f(0,0,0);
      glVertex3f(0,0,2);
      glEnd();
      glDisable(GL_DEPTH_TEST);

  }

  //Check for error
  GLuint error = glGetError();

  if( error != GL_NO_ERROR )
  {
      printf( "Error rendering OpenGL! %s\n", gluErrorString(error) );
  }
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
