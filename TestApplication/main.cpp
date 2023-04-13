#include <exception>
#include <iostream>
#include <ostream>

#include "vRenderer/vRenderer.h"
#include "vRenderer/camera/Camera.h"

void Run(VRenderer& a_Renderer, Camera& a_Camera)
{
	while (!a_Renderer.ShouldTerminate())
	{
		a_Camera.UpdateAspectRatio(a_Renderer.GetWindowExtent());

		a_Renderer.Render(a_Camera);
	}
}

int main(int argc, char* argv[])
{
	const int t_WindowWidth = 800;
	const int t_WindowHeight = 600;

	Camera t_Camera = {{2.0f, 2.0f, 2.0f}, t_WindowWidth, t_WindowHeight};

	VRenderer t_Renderer = {};
	t_Renderer.Init(t_WindowWidth, t_WindowHeight);

	// try to run the app and catch any potential exceptions.
    // If an exception is caught, print it
	try
	{
		Run(t_Renderer, t_Camera);
	}
	catch(const std::exception& t_Exceptions){
		std::cerr << t_Exceptions.what() << std::endl;
        return EXIT_FAILURE;
	}

	return t_Renderer.Terminate();
}
