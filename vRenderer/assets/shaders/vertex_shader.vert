#version 450

// temporary triangle positions
//vec2 vert_pos[3] = vec2[](
//	vec2(0.0, -0.5),
//	vec2(0.5, 0.5),
//	vec2(-0.5, 0.5)
//);

// temporary per-vertex colors
//vec3 vert_colors[3] = vec3[](
//	vec3(0.07, 0.75, 0.91),
//	vec3(0.76, 0.44, 0.92),
//	vec3(0.96, 0.3, 0.34)
//);

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

layout(binding = 0) uniform UniformBufferObject {
	mat4 model;
	mat4 view;
	mat4 projection;
} ubo;

void main() {
	gl_Position = ubo.projection * ubo.view * ubo.model * vec4(inPos, 0.0, 1.0);
	fragColor = inColor;
}
