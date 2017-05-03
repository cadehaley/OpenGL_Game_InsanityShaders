#version 140

in vec2 TextureCoord;
uniform sampler2D tex;

out vec4 LFragment; 

void main()
{ 
	LFragment = vec4( 0.7, 0.7, 0.7, 1.0 )* texture2D(tex,TextureCoord);
}
