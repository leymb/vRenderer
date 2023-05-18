#pragma once

#include <fstream>
#include <random>
#include <string>
#include <vector>

#include "vRenderer/helper_structs/Particle.h"

static std::vector<char> ReadFile(const std::string& a_FilePath)
{
	std::ifstream t_File(a_FilePath, std::ios::ate | std::ios::binary);

	if (!t_File.is_open())
	{
		throw std::runtime_error("Failed to open file!");
	}

	// use read position to determine file size
	const size_t t_FileSize = t_File.tellg();

	// allocate buffer for the data
	std::vector<char> t_Buffer(t_FileSize);

	// read the contents of the file
	t_File.seekg(0);
	t_File.read(t_Buffer.data(), t_FileSize);

	// close file
	t_File.close();

	return t_Buffer;
}

// taken from https://vulkan-tutorial.com/Compute_Shader#page_Compute-queue-families
// distributes particles on a circle
std::vector<Particle>& GenInitialParticles(uint32_t a_NumParticles, float a_Height, float a_Width)
{
	float t_Pi = 3.14159265358979323846;

	// generate particles
	std::default_random_engine t_RandomEngine(static_cast<unsigned>(time(nullptr)));
	std::uniform_real_distribution<float> t_RandomDistribution(0.0f, 1.0f);

	// set initial particle positions
	std::vector<Particle> t_Particles(a_NumParticles);
	for (Particle t_Particle : t_Particles)
	{
		float t_Random = 0.25f * sqrt(t_RandomDistribution(t_RandomEngine));
		float t_Theta = t_RandomDistribution(t_RandomEngine) * 2 * t_Pi;
		float t_X = t_Random * cos(t_Theta) * a_Height / a_Width;
		float t_Y = t_Random * sin(t_Theta);
		t_Particle.m_Pos = glm::vec2(t_X, t_Y);
		t_Particle.m_Velocity = glm::normalize(glm::vec2(t_X, t_Y)) * 0.00025f;
		t_Particle.m_Color = glm::vec4(	
										t_RandomDistribution(t_RandomEngine), 
										t_RandomDistribution(t_RandomEngine),
										t_RandomDistribution(t_RandomEngine), 
										1.0f
										);
	}

	return t_Particles;
}
