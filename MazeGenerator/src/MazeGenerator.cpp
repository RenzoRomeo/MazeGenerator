#include "MazeGenerator.h"

#include <iostream>

MazeGenerator::MazeGenerator(uint32_t sizeX, uint32_t sizeY, uint32_t width, uint32_t height)
	: m_size_x(sizeX), m_size_y(sizeY), m_width(width), m_height(height)
{
	m_cells.reserve(sizeX * sizeY);
	for (int y = 0; y < sizeY; y++)
	{
		for (int x = 0; x < sizeX; x++)
		{
			m_cells.push_back(Cell(x, y));
		}
	}
	m_current_cell = &m_cells.at(0);

	SDL_Init(SDL_INIT_VIDEO);

	m_window = SDL_CreateWindow(
		"Maze Generator", 
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width,
		height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
		);

	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_PRESENTVSYNC);

	if (!m_window)
		std::cout << "Could not create window: " << SDL_GetError() << '\n';
}

MazeGenerator::~MazeGenerator()
{
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}

void MazeGenerator::Run()
{
	m_stack.push_back(&m_cells[0]);
	SDL_Event e;
	while (true) {
		if (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
				break;
		}

		SDL_RenderClear(m_renderer);
		SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
		SDL_RenderFillRect(m_renderer, NULL);

		DrawCells();

		if (!m_stack.empty())
		{
			Cell* cell = *(m_stack.end() - 1);
			cell->visited = true;
			m_current_cell = cell;

			std::bitset<4> possibleSides{ "0000" };
			Cell* neighbor = nullptr;

			for (int i = 0; i < 4; i++)
			{
				neighbor = GetNeighbor(cell->x, cell->y, (Sides)i); // Sides go from 0 to 3
				if (neighbor && !neighbor->visited) possibleSides.set((size_t)i);
			}

			if (!possibleSides.any())
			{
				m_stack.erase(m_stack.end() - 1);
				continue;
			}
			int i = 0;
			while (!possibleSides.test(i = rand() % 4));
			Sides side = (Sides)(i);
			Sides oppositeSide = OppositeSide(side);
			neighbor = GetNeighbor(cell->x, cell->y, side);
			cell->borders.reset((size_t)side);
			neighbor->borders.reset((size_t)oppositeSide);
			m_stack.push_back(neighbor);
		}

		SDL_RenderPresent(m_renderer);
	}
}

void MazeGenerator::DrawCells()
{
	static uint32_t cellW = m_width / m_size_x;
	static uint32_t cellH = m_height / m_size_y;
	for (const Cell& cell : m_cells)
	{
		SDL_Rect rect;
		rect.x = cell.x * cellW;
		rect.y = cell.y * cellH;
		rect.w = cellW;
		rect.h = cellH;

		if (m_current_cell == &cell)
		{
			SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);
		}
		else if (cell.visited)
		{
			SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
		}
		else
		{
			SDL_SetRenderDrawColor(m_renderer, 50, 50, 50, 255);
		}

		SDL_RenderFillRect(m_renderer, &rect);

		SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);

		if (GetNeighbor(cell.x, cell.y, Sides::Left) && (cell.borders.test((size_t)Sides::Left)))
		{
			SDL_Rect b;
			b.x = cell.x * cellW;
			b.y = cell.y * cellH;
			b.w = 1;
			b.h = cellH;
			SDL_RenderFillRect(m_renderer, &b);
		}

		if (GetNeighbor(cell.x, cell.y, Sides::Right) && (cell.borders.test((size_t)Sides::Right)))
		{
			SDL_Rect b;
			b.x = (cell.x + 1) * cellW - 1;
			b.y = cell.y * cellH;
			b.w = 1;
			b.h = cellH;
			SDL_RenderFillRect(m_renderer, &b);
		}

		if (GetNeighbor(cell.x, cell.y, Sides::Top) && (cell.borders.test((size_t)Sides::Top)))
		{
			SDL_Rect b;
			b.x = cell.x * cellW;
			b.y = cell.y * cellH;
			b.w = cellW;
			b.h = 1;
			SDL_RenderFillRect(m_renderer, &b);
		}

		if (GetNeighbor(cell.x, cell.y, Sides::Bottom) && (cell.borders.test((size_t)Sides::Bottom)))
		{
			SDL_Rect b;
			b.x = cell.x * cellW;
			b.y = (cell.y + 1) * cellH - 1;
			b.w = cellW;
			b.h = 1;
			SDL_RenderFillRect(m_renderer, &b);
		}
	}
}

Cell* MazeGenerator::GetNeighbor(uint32_t x, uint32_t y, Sides side)
{
	uint32_t dx = 0, dy = 0;

	switch (side)
	{
	case Sides::Top:
		dy = -1;
		break;
	case Sides::Right:
		dx = 1;
		break;
	case Sides::Bottom:
		dy = 1;
		break;
	case Sides::Left:
		dx = -1;
		break;
	default:
		return nullptr;
	}

	uint32_t neighborX = x + dx;
	uint32_t neighborY = y + dy;

	if (neighborX < 0 || neighborX >= m_size_x) return nullptr;
	if (neighborY < 0 || neighborY >= m_size_y) return nullptr;

	return &m_cells.at(Index(neighborX, neighborY));
}
