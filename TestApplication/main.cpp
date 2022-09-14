#include <exception>
#include <iostream>
#include <ostream>

#include "vRenderer/vRenderer.h"

void Run(const VRenderer& a_Renderer)
{
	while (!a_Renderer.ShouldTerminate())
	{
		a_Renderer.Render();
	}
}

int main(int argc, char* argv[])
{
	VRenderer t_Renderer = {};
	t_Renderer.Init();

	// try to run the app and catch any potential exceptions.
    // If an exception is caught, print it
	try
	{
		Run(t_Renderer);
	}
	catch(const std::exception& t_Exceptions){
		std::cerr << t_Exceptions.what() << std::endl;
        return EXIT_FAILURE;
	}

	return t_Renderer.Terminate();
}
