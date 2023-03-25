#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTextCoord;

layout(location = 0) out vec4 OutColor;

layout(binding = 1) uniform sampler2D texSampler;


void main() {
	OutColor = texture(texSampler, fragTextCoord);
}
