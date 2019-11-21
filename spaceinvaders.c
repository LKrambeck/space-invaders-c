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

/*
	Prototypes 
*/
int  setupInput         (t_game *game);
int  validateWindowSize (int *rows, int *cols);
void startGame          (t_game *game, t_elements *elements);
void inicializeLists    (t_elements *elements);
void addAliens          (t_elements *elements);
void addStructures      (t_game *game, t_elements *elements);
void addAllBarriers     (t_game *game, t_elements *elements);
void addBarrier         (int i, int j, t_elements *elements);
void playGame           (t_game *game, t_elements *elements);
void printScreen        (t_game *game, t_elements *elements);
void printBorders       (t_game *game);
void printAllAliens     (t_elements *elements);
void printAlien         (int xPos, int yPos, int xSize, int ySize);
void printStructures    (t_elements *elements);
void printShots         (t_elements *elements);
void printBombs         (t_elements *elements);
void printScore         (t_game *game);
int  nDigits            (int n);




int main ()
{
	t_game game;
	t_elements elements;

	if (!setupInput (&game))
		return 1;

	startGame (&game, &elements);

	playGame (&game, &elements);

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
	addStructures (game, elements);

	printScreen (game, elements);
}

void inicializeLists (t_elements *elements)
{
	inicializa_lista (&elements->aliens);
	inicializa_lista (&elements->structures);
	inicializa_lista (&elements->shots);
	inicializa_lista (&elements->bombs);
}

void addAliens (t_elements *elements)
{
	/*TO DO: aliens da primeira fileira devem ser 3x3*/
	int xIni = 7;
	int yIni = 1;
	int xSpacing = 4;
	int ySpacing = 7;

	int xSize = 3;
	int ySize = 5;
	int alienStatus = 0;
	int alienSpeed = 1;

	int i, j;

	for (i = xIni; i < (5 * xSpacing + xIni); i += xSpacing)
		for (j = yIni; j < (11 * ySpacing + yIni); j+= ySpacing)
			insere_fim_lista (i, j, xSize, ySize, alienStatus, alienSpeed, &elements->aliens);
}

void addStructures (t_game *game, t_elements *elements)
{
/*	addSpaceship (game, elements);*/

	addAllBarriers (game, elements);
}

void addAllBarriers (t_game *game, t_elements *elements)
{
	int ySize = 7;
	int xIni = game->maxRows-7;
	int yIni = 1;
	int ySpacing = ((game->maxCols-2) - 4*(ySize))/5;
	int noBarriers = 0;

	int j;

	for (j = (yIni + ySpacing); j < game->maxCols-1; j+= (ySize + ySpacing))
	{
		if (noBarriers < 4)
			addBarrier (xIni, j, elements);

		noBarriers++;
	}
}
	
void addBarrier (int xPos, int yPos, t_elements *elements)
{
	int barrierStatus = 0;
	int barrierSpeed = 0;
	int xSize = 3;
	int ySize = 7;

	int i, j;

	for (i = xPos; i < (xPos + xSize); i++)
		for (j = yPos; j < (yPos + ySize); j++) 
			insere_fim_lista (i, j, 1, 1, barrierStatus, barrierSpeed, &elements->structures);
}

void playGame(t_game *game, t_elements *elements){}

void printScreen (t_game *game, t_elements *elements)
{
	erase();

	printStructures (elements);
	printShots (elements);
	printBombs (elements);
	printAllAliens (elements);
	printBorders (game);
	printScore (game);

	refresh();

	sleep(20);
}

void printBorders (t_game *game)
{
	int i,j;

	for (i=0; i < game->maxRows; i++)
	{
		mvaddch (i, 0, '|');
		mvaddch (i, game->maxCols-1, '|');
	}

	for (j=0; j < game->maxCols; j++)
	{
		mvaddch (0, j, '-'); 
		mvaddch (game->maxRows-1, j, '-');
	}
}

void printAllAliens (t_elements *elements)
{
	if (!inicializa_atual_inicio (&elements->aliens))
		return;

	int xPos, yPos, xSize, ySize, status, speed;	

	while (consulta_item_atual(&xPos, &yPos, &xSize, &ySize, &status, &speed, &elements->aliens))
	{
		if (status == 0)
			printAlien (xPos, yPos, xSize, ySize);
		
		incrementa_atual (&elements->aliens);
	}
}

void printAlien (int xPos, int yPos, int xSize, int ySize)
{
	int i,j;

	for (i=xPos; i<(xPos+xSize); i++)
		for (j=yPos; j<(yPos+ySize); j++)
			mvaddch (i,j,'A');
}

void printStructures (t_elements *elements)
{
	if (!inicializa_atual_inicio (&elements->structures))
		return;

	int xPos, yPos, xSize, ySize, status, speed;	

	while (consulta_item_atual(&xPos, &yPos, &xSize, &ySize, &status, &speed, &elements->structures))
	{
		if (status == 0)
			mvaddch (xPos, yPos, 'B');
		
		incrementa_atual (&elements->structures);
	}
}

void printShots (t_elements *elements)
{
	if (!inicializa_atual_inicio (&elements->shots))
		return;

	int xPos, yPos, xSize, ySize, status, speed;	

	while (consulta_item_atual(&xPos, &yPos, &xSize, &ySize, &status, &speed, &elements->shots))
	{
		if (status == 0)
			mvaddch (xPos, yPos, '|');
		
		incrementa_atual (&elements->shots);
	}
}

void printBombs (t_elements *elements)
{
	if (!inicializa_atual_inicio (&elements->bombs))
		return;

	int xPos, yPos, xSize, ySize, status, speed;	

	while (consulta_item_atual(&xPos, &yPos, &xSize, &ySize, &status, &speed, &elements->bombs))
	{
		if (status == 0)
			mvaddch (xPos, yPos, '$');
		
		incrementa_atual (&elements->bombs);
	}
}

void printScore (t_game *game)
{
	int yPosLevel = (game->maxCols-7)/2;
	int yPosScore = (game->maxCols-nDigits(game->score))/2;

	mvprintw (1, yPosLevel, "Level %d", game->level);
	mvprintw (2, yPosScore, "%d", game->score);
}

int nDigits (int n)
{
	if (n%10 == 0)
		return 0;

	return 1 + (nDigits (n/10));
}
