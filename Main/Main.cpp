#include <iostream>
#include <SDL.h>

using std::cout;
using std::endl;

const int T = 60;

int GRID[T][T];

void display()
{
	for (int i = 0; i < T; i++)
	{
		for (int j = 0; j < T; j++)
		{
			cout << GRID[i][j] << ' ';
		}
		cout << endl;
	}
}

void init()
{
	for (int i = 0; i < T; i++)
	{
		for (int j = 0; j < T; j++)
		{
			GRID[i][j] = 0;
		}
	}
}

bool estVide()
{
	int i = 0;
	int j = 0;

	while (i < T)
	{
		while (j < T && GRID[i][j] != 1)
		{
			j++;
		}
		if (j < T) i = T;
		j = 0;
		i++;
	}

	return (i == T);
}

int living_neighbours(int x, int y, int** grid)
{
	int living = 0;
	int i = x + 1;
	int j = y - 1;
	int cond1 = (x - 2 + T) % T;
	int cond2 = (y + 2 + T) % T;
	i = (i + T) % T;
	j = (j + T) % T;
	while (i != cond1)
	{
		while (j != cond2)
		{
			living += grid[i][j];
			j = (j + 1 + T) % T;
		}
		j = y - 1;
		j = (j + T) % T;
		i = (i - 1 + T) % T;
	}
	living -= grid[x][y];

	return living;
}

bool change(int i, int j, int** grid)
{
	int living = living_neighbours(i, j, grid);
	bool changed = false;

	if (grid[i][j] == 0 && living == 3)
	{
		changed = true;
	}
	else if (grid[i][j] == 1 && living != 3 && living != 2)
	{
		changed = true;
	}

	return changed;
}

int** clone()
{
	int** temp_grid = new int* [T];

	for (int i = 0; i < T; i++)
	{
		temp_grid[i] = new int[T];
	}

	for (int i = 0; i < T; i++)
	{
		for (int j = 0; j < T; j++)
		{
			temp_grid[i][j] = GRID[i][j];
		}
	}

	return temp_grid;
}

void free_clone(int** temp_grid)
{
	for (int i = 0; i < T; i++)
	{
		delete[] temp_grid[i];
	}
	delete[] temp_grid;
}

void copy_values(int** temp_grid)
{
	for (int i = 0; i < T; i++)
	{
		for (int j = 0; j < T; j++)
		{
			temp_grid[i][j] = GRID[i][j];
		}
	}
}

void evolve(int** temp)
{
	for (int i = 0; i < T; i++)
	{
		for (int j = 0; j < T; j++)
		{
			if (change(i, j, temp))
			{
				GRID[i][j] = 1 - GRID[i][j];
			}
		}
	}
}

void ErrorExit(const char* message)
{
	SDL_Log("ERREUR : %s - %s\n", message, SDL_GetError());
	SDL_Quit();
	exit(EXIT_FAILURE);
}

void SDL_display(SDL_Renderer* render)
{
	SDL_Rect rectangle = { 0, 0, 10, 10 };
	int i = 0, j = 0;

	while (j < T && rectangle.x + rectangle.w <= T*10)
	{
		while (i < T && rectangle.y <= T*10)
		{
			if (GRID[i][j])
			{
				if (SDL_SetRenderDrawColor(render, 255, 255, 255, 255) != 0) ErrorExit("Changement de couleur echoue");
				if (SDL_RenderDrawRect(render, &rectangle) != 0) ErrorExit("Dessin de rectangle echoue");
				SDL_RenderFillRect(render, &rectangle);
			}

			rectangle.y = rectangle.y + rectangle.h;
			i++;
		}
		rectangle.y = 0;

		rectangle.x = rectangle.x + rectangle.w;
		j++;
		i = 0;
	}

	SDL_RenderPresent(render);
	SDL_Delay(20);

	if (SDL_SetRenderDrawColor(render, 0, 0, 0, 255) != 0) ErrorExit("Changement de couleur echoue");
	SDL_RenderClear(render);
}


int main(int argv, char** argc)
{
	int i = 1;

	if (SDL_Init(SDL_INIT_VIDEO) != 0) ErrorExit("Initialisation");

	SDL_Window* window = SDL_CreateWindow("Game of Life", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, T*10, T*10, SDL_WINDOW_OPENGL);

	SDL_Renderer* render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	init();
	display();

	int** temp = clone();
	int x, y;

	SDL_Event event;
	bool running = true;
	bool pause = true;
	while (running)
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
					running = false;
					break;
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_SPACE)
					{
						pause = !pause;
					}
					break;
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == SDL_BUTTON_LEFT)
					{
						x = event.button.x / (600 / T);
						y = event.button.y / (600 / T);
						if (x < T && y < T)
						{
							GRID[y][x] = 1 - GRID[y][x];
						}
						display();
						SDL_display(render);
						copy_values(temp);
					}
					break;
				default:
					break;
			}
		}

		if (!pause)
		{
			evolve(temp);
			cout << "TOUR " << i << endl;
			display();
			SDL_display(render);
			copy_values(temp);
			i++;
		}
	}

	free_clone(temp);

	SDL_DestroyRenderer(render);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}