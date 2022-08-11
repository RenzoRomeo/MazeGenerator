#include "MazeGenerator.h"

int main(int argc, char *argv[])
{
	MazeGenerator gen(50, 50, 500, 500);
	gen.Run();
	return 0;
}