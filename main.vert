#version 140
in vec3 LVertexPos3D;
uniform mat4 mvp;

void main()
{ 
	gl_Position = mvp * vec4( LVertexPos3D, 1.0 );
}
