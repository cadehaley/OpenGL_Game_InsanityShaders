#version 140
in vec3 LVertexPos3D;
uniform mat4 mvp;

out vec2 TextureCoord;

void main()
{ 
	TextureCoord = vec2(LVertexPos3D.x, LVertexPos3D.y);	
	gl_Position = mvp * vec4( LVertexPos3D, 1.0 );
}
