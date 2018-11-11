#version 330

uniform mat4 combined_xform;
uniform mat4 model_xform;

in vec3 vertex_position;
in vec3 vertex_normal;
in vec2 vertex_texcoord;

out vec3 world_Pos;
out vec3 world_Norm;

out vec2 varying_texcoord;

void main(void)
{
	//Converting position and normal into world space to pass into fragment shader
	vec3 P = mat4x3(model_xform) * vec4(vertex_position, 1.0);
	vec3 N = mat3(model_xform) * vertex_normal;

	world_Pos = P;
	world_Norm = N;

	varying_texcoord = vertex_texcoord;
    gl_Position = combined_xform * vec4(vertex_position, 1.0);
}
