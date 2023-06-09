// taken from https://github.com/Overv/VulkanTutorial/blob/main/code/31_shader_compute.vert
#version 450

layout (location = 0) in vec2 inPos;
layout (location = 1) in vec4 inColor;

layout (location = 0) out vec3 fragColor;

void main() {
	gl_PointSize = 14.0;
	gl_Position = vec4(inPos.xy, 1.0, 1.0);
	fragColor = inColor.rgb;
}