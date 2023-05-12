#version 450

struct Particle 
{
	vec2 m_Position;
	vec2 m_Velocity;
	vec4 m_Color;
};

layout (std140, binding = 1) readonly buffer ParticleSSBOIn
{
	Particle particlesIn[];
}

layout (std140, binding = 2) buffer ParticlesSSBOOut
{
	Particle particlesOut[];
}

void main()
{
	particlesOut[index].m_Position = particlesIn[index].m_Position + particlesIn[index].m_Velocity.xy * ubo.deltaTime;
}