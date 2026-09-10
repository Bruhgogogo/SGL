#include "blitz3d.h"

int main()
{
	Graphics3D(1270, 800, "Hello Raylib!");

	while (!IsWindowShouldClose())
	{
		RenderWorld();
	}
}