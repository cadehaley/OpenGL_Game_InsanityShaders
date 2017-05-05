#version 140
in vec3 LVertexPos3D;
in vec2 texcoord;
uniform mat4 mvp;
uniform float time;
uniform float factor;

out vec2 TextureCoord;

void main()
{ 
	float seed = (0.01+(0.2*factor))*LVertexPos3D.x+time;
	vec3 warped_vertex = vec3(LVertexPos3D.x,(LVertexPos3D.y+((cos(seed)+sin(seed))*factor)),LVertexPos3D.z) + 0.3*factor*(cos(seed) + 0.3*sin(seed));
	
	TextureCoord = texcoord;
	gl_Position = mvp * vec4( warped_vertex, 1.0 );
	
}
