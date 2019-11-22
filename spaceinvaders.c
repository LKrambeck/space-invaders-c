#include <stdio.h>
#include <unistd.h>
#include <ncurses.h>

#include "lib_lista_space.h"

#define CLOCK 10000
#define SHOT_SPEED 50

typedef struct Game {
	int score;
	int level;
	int maxRows;
	int maxCols;	
} t_game;

typedef struct Elements {
	t_lista aliens;
	t_lista barriers;
	t_lista shots;
	t_lista bombs;
	t_lista spaceship;
	int aliensWay;
} t_elements;

/*
	Prototypes 
*/
int  setupInput                    (t_game *game);
int  validateWindowSize            (int *rows, int *cols);
void startGame                     (t_game *game, t_elements *elements);
void inicializeLists               (t_elements *elements);
void addAliens                     (t_elements *elements);
void addBarriers                   (t_game *game, t_elements *elements);
void addSingleBarrier              (int i, int j, t_elements *elements);
void addSpaceship                  (t_game *game, t_elements *elements);
void playGame                      (t_game *game, t_elements *elements);
int  input                         (t_game *game, t_elements *elements);
void addShot                       (t_elements *elements);
void getSpaceshipShootingPos       (int *x, int *y, t_elements *elements);
void moveSpaceshipLeft             (t_game *game, t_elements *elements);
void moveSpaceshipRight            (t_game *game, t_elements *elements);
int  canMoveLeft                   (t_game *game, t_lista *list);
int  canMoveRight                  (t_game *game, t_lista *list);
void moveObjects                   (t_game *game, t_elements *elements);
void moveAliens                    (t_game *game, t_elements *elements);
void printScreen                   (t_game *game, t_elements *elements);
void printBorders                  (t_game *game);
void printAllAliens                (t_elements *elements);
void printAlien                    (int xPos, int yPos, int xSize, int ySize);
void printBarriers                 (t_elements *elements);
void printShots                    (t_elements *elements);
void printBombs                    (t_elements *elements);
void printSpaceship                (t_elements *elements);
void printScore                    (t_game *game);
int  nDigits                       (int n);




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
	addBarriers (game, elements);
	addSpaceship (game, elements);

	printScreen (game, elements);
}

void inicializeLists (t_elements *elements)
{
	inicializa_lista (&elements->aliens);
	inicializa_lista (&elements->barriers);
	inicializa_lista (&elements->shots);
	inicializa_lista (&elements->bombs);
	inicializa_lista (&elements->spaceship);
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
			insere_fim_lista (i, j, xSize, ySize, 0, alienStatus, alienSpeed, &elements->aliens);
}

void addBarriers (t_game *game, t_elements *elements)
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
			addSingleBarrier (xIni, j, elements);

		noBarriers++;
	}
}
	
void addSingleBarrier (int xPos, int yPos, t_elements *elements)
{
	int barrierStatus = 0;
	int barrierSpeed = 0;
	int xSize = 3;
	int ySize = 7;

	int i, j;

	for (i = xPos; i < (xPos + xSize); i++)
		for (j = yPos; j < (yPos + ySize); j++) 
			insere_fim_lista (i, j, 1, 1, 0, barrierStatus, barrierSpeed, &elements->barriers);
}

void addSpaceship (t_game *game, t_elements *elements)
{
	int xSize = 2;
	int ySize = 5;
	int xIni = game->maxRows-xSize-1;
	int yIni = (game->maxCols-ySize)/2;
	int spaceshipType = 0;
	int spaceshipStatus = 0;
	int spaceshipSpeed = 4;

	insere_fim_lista (xIni, yIni, xSize, ySize, spaceshipType, spaceshipStatus, spaceshipSpeed, &elements->spaceship);

	inicializa_atual_inicio (&elements->spaceship);
}

void playGame(t_game *game, t_elements *elements)
{
	while (1)
	{
		if (!input (game, elements))
			return;

		moveObjects (game, elements);
/*
		testColisions (elements);
*/
		printScreen (game, elements);

		usleep(CLOCK);		
	}

}

int input (t_game *game, t_elements *elements)
{
	int key = getch();

	if(key == ' ')
		addShot (elements);

	else if(key == KEY_LEFT)
		moveSpaceshipLeft (game, elements);

	else if (key == KEY_RIGHT) 
		moveSpaceshipRight (game, elements);

	else if (key == 'q')
		return 0;

	return 1;	
}

void addShot (t_elements *elements)
{
	int xPos, yPos;
	int shotStatus = 0;

	getSpaceshipShootingPos (&xPos, &yPos, elements);

	insere_fim_lista (xPos, yPos, 1, 1, 0, shotStatus, SHOT_SPEED, &elements->shots);
}

void getSpaceshipShootingPos (int *x, int *y, t_elements *elements)
{
	int xPos, yPos, xSize, ySize, type, status, speed;
	consulta_item_atual (&xPos, &yPos, &xSize, &ySize, &type, &status, &speed, &elements->spaceship);

	*x = xPos - 1;
	*y = yPos + 2;
}

void moveSpaceshipLeft (t_game *game, t_elements *elements)
{
	if (canMoveLeft (game, &elements->spaceship))
		decrementa_y_atual (&elements->spaceship);	
}

void moveSpaceshipRight (t_game *game, t_elements *elements)
{
	if (canMoveRight (game, &elements->spaceship))
		incrementa_y_atual (&elements->spaceship);	
}

int canMoveLeft (t_game *game, t_lista *list)
{
	int xPos, yPos, xSize, ySize, type, status, speed;
	if (!consulta_item_atual (&xPos, &yPos, &xSize, &ySize, &type, &status, &speed, list))
		return 0;
	
	if (yPos > 1)
		return 1;

	return 0;
}

int canMoveRight (t_game *game, t_lista *list)
{
	int xPos, yPos, xSize, ySize, type, status, speed;
	if (!consulta_item_atual (&xPos, &yPos, &xSize, &ySize, &type, &status, &speed, list))
		return 0;
	
	if (yPos < game->maxCols -ySize -1)
		return 1;

	return 0;
}

void moveObjects (t_game *game, t_elements *elements)
{
/*	if (teste de tempo com contador % elements->aliens->speed)*/
	moveAliens (game, elements);

/*	if (teste de tempo com contador % elements->aliens->speed)*/
/*	moveMothership (game, elements);	*/
}

void moveAliens (t_game *game, t_elements *elements)
{
	if (!inicializa_atual_inicio (&elements->aliens))
		return;

	int xPos, yPos, xSize, ySize, type, status, speed;	

	if (elements->aliensWay == 1)
		while (canMoveLeft (game, &elements->aliens))
		{
			incrementa_y_atual (&elements->aliens);
			incrementa_atual (&elements->aliens);
		}

	else
		while (canMoveLeft (game, &elements->aliens))
		{
			decrementa_y_atual (&elements->aliens);
			incrementa_atual (&elements->aliens);
		}
}









void printScreen (t_game *game, t_elements *elements)
{
	erase();

	printBarriers   (elements);
	printShots      (elements);
	printBombs      (elements);
	printAllAliens  (elements);
	printSpaceship  (elements);
	printBorders    (game);
	printScore      (game);

	refresh();
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

	int xPos, yPos, xSize, ySize, type, status, speed;	

	while (consulta_item_atual(&xPos, &yPos, &xSize, &ySize, &type, &status, &speed, &elements->aliens))
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

void printBarriers (t_elements *elements)
{
	if (!inicializa_atual_inicio (&elements->barriers))
		return;

	int xPos, yPos, xSize, ySize, type, status, speed;	

	while (consulta_item_atual(&xPos, &yPos, &xSize, &ySize, &type, &status, &speed, &elements->barriers))
	{
		if (status == 0)
			mvaddch (xPos, yPos, 'B');
		
		incrementa_atual (&elements->barriers);
	}
}

void printShots (t_elements *elements)
{
	if (!inicializa_atual_inicio (&elements->shots))
		return;


	int xPos, yPos, xSize, ySize, type, status, speed;	

	while (consulta_item_atual(&xPos, &yPos, &xSize, &ySize, &type, &status, &speed, &elements->shots))
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

	int xPos, yPos, xSize, ySize, type, status, speed;	

	while (consulta_item_atual(&xPos, &yPos, &xSize, &ySize, &type, &status, &speed, &elements->bombs))
	{
		if (status == 0)
			mvaddch (xPos, yPos, '$');
		
		incrementa_atual (&elements->bombs);
	}
}

void printSpaceship (t_elements *elements)
{
	int xPos, yPos, xSize, ySize, type, status, speed;

	consulta_item_atual (&xPos, &yPos, &xSize, &ySize, &type, &status, &speed, &elements->spaceship);

	int i, j;

	for (i=xPos; i < (xPos+xSize); i++)
		for (j=yPos; j < (yPos+ySize); j++)
			mvaddch (i,j,'S');
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
