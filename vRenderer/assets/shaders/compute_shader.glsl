// taken from https://vulkan-tutorial.com/Compute_Shader#page_Compute-pipelines
#version 450

layout (binding = 0) uniform ParameterUBO {
    float delta;
} ubo;

// define Particle struct to mirror CPU side struct
struct Particle {
    vec2 pos;
    vec2 vel;
    vec4 color;
}

// define Shader Storage Buffer for both write and read
layout(std140, binding = 1) readonly buffer InputParticleSSBO {
    Particle inParticles[ ];
}

layout(std140, binding = 2) buffer OutputParticleSSBO {
    Particle outParticles[ ];
}

// define number of invocations
layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

void main()
{
    uint index = gl_GlobalInvocationID.x;

    Particle inParticle = inParticles[index];

    outParticles[index].pos = inParticle.pos + inParticle.vel * ubo.delta;
    outParticles[index].vel = inParticle.vel;
}