#pragma once

#include "SDL/SDL.h"

#include <vector>
#include <bitset>

enum class Sides
{
	Top = 0,
	Right,
	Bottom,
	Left
};

struct Cell
{
	uint32_t x;
	uint32_t y;
	bool visited;
	std::bitset<4> borders{ "1111" };

	Cell(uint32_t x, uint32_t y)
		: x(x), y(y), visited(false) {}
};

class MazeGenerator
{
public:
	MazeGenerator(uint32_t sizeX, uint32_t sizeY, uint32_t width, uint32_t height);
	~MazeGenerator();

	void Run();
private:
	void DrawCells();
	Cell* GetNeighbor(uint32_t x, uint32_t y, Sides side);
	uint32_t Index(uint32_t x, uint32_t y) { return y * m_size_x + x; }
	Sides OppositeSide(Sides side)
	{
		switch (side)
		{
		case Sides::Top:
			return Sides::Bottom;
		case Sides::Right:
			return Sides::Left;
		case Sides::Bottom:
			return Sides::Top;
		case Sides::Left:
			return Sides::Right;
		default:
			return Sides::Top;
		}
	}
private:
	SDL_Renderer* m_renderer;
	SDL_Window* m_window;

	uint32_t m_size_x, m_size_y;
	uint32_t m_width, m_height;
	std::vector<Cell> m_cells;
	Cell* m_current_cell = nullptr;

	std::vector<Cell*> m_stack;
};