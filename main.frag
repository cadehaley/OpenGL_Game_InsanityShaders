#version 140

in vec2 TextureCoord;
uniform sampler2D tex;
uniform float factor;

out vec4 LFragment; 

void main()
{ 
	LFragment = vec4( 0.7-(factor*0.2), 0.7-(factor*0.5), 0.7-(factor*0.5), 1.0 )* texture2D(tex,TextureCoord);
}
