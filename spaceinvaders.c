#include <stdio.h>
#include <unistd.h>
#include <ncurses.h>

#include "lib_lista_space.h"

typedef struct Game {
	int score;
	int level;
	int maxRows;
	int maxCols;
} t_game;

typedef struct Elements {
	t_lista aliens;
	t_lista structures;
	t_lista shots;
	t_lista bombs;
} t_elements;


int  setupInput         (t_game *game);
int  validateWindowSize (int *rows, int *cols);
void startGame          (t_game *game, t_elements *elements);
void inicializeLists    (t_elements *elements);
void addAliens          (t_elements *elements);
void addStructures      (t_elements *elements);
void playGame           (t_game *game, t_elements *elements);
void printScreen        (int rows, int cols);

int main ()
{
	t_game game;
	t_elements elements;

	if (!setupInput (&game))
		return 1;

	startGame (&game, &elements);

	playGame (&game, &elements);

	printScreen (game.maxRows, game.maxCols);

	endwin();
	return 0;
}

int setupInput (t_game *game)
{
	initscr();
	cbreak();               /* desabilita o buffer de entrada */
	noecho();               /* não mostra os caracteres digitados */
	nodelay(stdscr, TRUE);  /* faz com que getch não aguarde a digitação */
	keypad(stdscr, TRUE);   /* permite a leitura das setas */
	curs_set(FALSE);        /* não mostra o cursor na tela */

	if (!validateWindowSize (&game->maxRows, &game->maxCols))
		return 0;

	return 1;
}

int validateWindowSize (int *rows, int *cols)
{
	getmaxyx (stdscr, *rows, *cols);

	if (*rows < 38 || *cols < 100)
	{
		endwin();
		printf ("Window is too small.\nMinimum size: 38x100.\nCurrent size: %dx%d\n",*rows,*cols);
		return 0;
	}

	return 1;	
}

void startGame(t_game *game, t_elements *elements)
{
	game->score = 0;
	game->level = 1;

	inicializeLists (elements);

	addAliens (elements);
	addStructures (elements);
}

void inicializeLists (t_elements *elements)
{
	inicializa_lista (&elements->aliens);
	inicializa_lista (&elements->structures);
	inicializa_lista (&elements->shots);
	inicializa_lista (&elements->bombs);
}

void addAliens (t_elements *elements){}
void addStructures (t_elements *elements){}

void playGame(t_game *game, t_elements *elements){}

void printScreen (int rows, int cols)
{
	erase();

	int i, j;
	for ( i=0; i < rows; i++ )
		for ( j=0; j < cols; j++ )
		{
			mvaddch(i,j,'.');
		}
	refresh();
	sleep(2);
}
