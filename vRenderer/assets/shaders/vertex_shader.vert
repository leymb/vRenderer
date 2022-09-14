#version 450

// temporary triangle positions
vec2 vert_pos[3] = vec2[](
	vec2(0.0, -0.5),
	vec2(0.5, 0.5),
	vec2(-0.5, 0.5)
);

// temporary per-vertex colors
vec3 vert_colors[3] = vec3[](
	vec3(1.0, 0.0, 0.0),
	vec3(0.0, 1.0, 0.0),
	vec3(0.0, 0.0, 1.0)
);

layout(location = 0) out vec3 fragColor;

void main() {
	gl_Position = vec4(vert_pos[gl_VertexIndex], 0.0, 1.0);
	fragColor = vert_colors[gl_VertexIndex];
}
