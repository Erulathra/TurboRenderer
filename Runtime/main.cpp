#include "stdio.h"

#include "Engine.h"

int main(int argc, char *argv[])
{
	Turbo::Engine engine = {};
	engine.Init();
	engine.Shutdown();
}
