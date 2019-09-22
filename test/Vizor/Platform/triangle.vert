#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec3 albedo;

layout(binding = 0) uniform UBO {
	mat4 projection;
	mat4 model;
	mat4 view;
} camera;

vec2 positions[3] = vec2[](
	vec2(0.0, -0.5),
	vec2(0.5, 0.5),
	vec2(-0.5, 0.5)
);

vec3 colors[3] = vec3[](
	vec3(1.0, 0.0, 0.0),
	vec3(0.0, 1.0, 0.0),
	vec3(0.0, 0.0, 1.0)
);

void main() {
	vec4 vertex = vec4(positions[gl_VertexIndex], 0.0, 1.0);
	vec4 position = camera.view * camera.model * vertex;
	
	gl_Position = camera.projection * position;
	albedo = colors[gl_VertexIndex];
}