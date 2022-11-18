#version 450

// temporary triangle positions
vec2 vert_pos[3] = vec2[](
	vec2(0.0, -0.5),
	vec2(0.5, 0.5),
	vec2(-0.5, 0.5)
);

// temporary per-vertex colors
vec3 vert_colors[3] = vec3[](
	vec3(0.07, 0.75, 0.91),
	vec3(0.76, 0.44, 0.92),
	vec3(0.96, 0.3, 0.34)
);

layout(location = 0) out vec3 fragColor;

void main() {
	gl_Position = vec4(vert_pos[gl_VertexIndex], 0.0, 1.0);
	fragColor = vert_colors[gl_VertexIndex];
}
