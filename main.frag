#version 140

varying vec2 TextureCoord;
uniform sampler2D tex;

out vec4 LFragment; 

void main()
{ 
	LFragment = vec4( 1.0, 1.0, 1.0, 1.0 )* texture2D(tex,TextureCoord);
}
