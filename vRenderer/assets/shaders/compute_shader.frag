// taken from https://github.com/Overv/VulkanTutorial/blob/main/code/31_shader_compute.frag
#version 450

layout (location = 0) in vec3 fragColor;

layout (location = 0) out vec4 outColor;

void main()
{
	vec2 coordinates = gl_PointCoord - vec2(0.5);
	outColor = vec4(fragColor, 0.5 - length(coordinates));
}