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

  std::cout << glGetString(GL_VERSION) << std::endl;
  //Check for error
  error = glGetError();

  if( error != GL_NO_ERROR )
  {
      //printf( "Error initializing OpenGL viewer! %s\n", gluErrorString(error) );
      std::cout << error << std::endl;
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
      glBegin( GL_QUADS );
          glVertex2f( -0.5f, -0.5f );
          glVertex2f( 0.5f, -0.5f );
          glVertex2f( 0.5f, 0.5f );
          glVertex2f( -0.5f, 0.5f );
      glEnd();
  }
}




/*
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
*/
