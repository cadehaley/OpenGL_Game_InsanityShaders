#version 140
in vec3 LVertexPos3D;
attribute vec2 texcoord;
uniform mat4 mvp;

out vec2 TextureCoord;

void main()
{ 
	TextureCoord = texcoord;	
	gl_Position = mvp * vec4( LVertexPos3D, 1.0 );
}
