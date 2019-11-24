#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>
#include <string.h>

#include "lib_lista_space.h"

#define CLOCK           20000

#define ALIEN_POINTS          100
#define BARRIER_POINTS       -10
#define BOMB_POINTS           10
#define MOTHERSHIP_POINTS     2000


/* Adjust speeds (range from 1-100) */
#define SHOT_SPEED            50
#define BOMB_SPEED            40
#define ALIEN_BASE_SPEED      25
#define SPACESHIP_SPEED       40
#define MOTHERSHIP_SPEED      40
#define SPEED_LV_UP           5

/* Adjust frequency (range from 1-10000)*/
#define BOMB_FREQUENCY        10
#define MOTHERSHIP_FREQUENCY  15 

/* Body of all objects */
#define ALIEN1_1        " AAA "
#define ALIEN1_2        "AMMMA"
#define ALIEN1_3        "/-X-\\"

#define ALIEN2_1        ".v_v."
#define ALIEN2_2        "}WMW{"
#define ALIEN2_3        " / \\ "

#define ALIEN3_1        " nmn "
#define ALIEN3_2        "dbMdb"
#define ALIEN3_3        "_/-\\_"

#define BARRIER1        'A'
#define BARRIER2        'M'

#define SPACESHIP1      " /^\\ "
#define SPACESHIP2      "MMMMM"

#define SHOT            '|'

#define BOMB            '$'

#define EXPLOSION1      " \\'/ "
#define EXPLOSION2      "-   -"
#define EXPLOSION3      " /,\\ "

#define MOTHERSHIP1     " /MMMMM\\ " 
#define MOTHERSHIP2     "AMoMoMoMA"
#define MOTHERSHIP3     " \\/'-'\\/ "


/* === TABLE OF CONVENTIONS ===

       STATUS:

Aliens          1, 2
Barrier         0
Shot            0
Bomb            0
Spaceship       0
Mothership      0, 1

        TYPE:

Aliens          1, 2, 3
Barrier         4, 5
Shot            6
Bomb            7
Spaceship       8
Mothership      9

*/


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
	t_lista mothership;
	int aliensWay;
} t_elements;

/*
	Prototypes 
*/
int  setupInput                    (t_game *game);
int  validateWindowSize            (t_game *game);
void inicializeLists               (t_elements *elements);
void destroyLists                  (t_elements *elements);
void startGame                     (t_game *game, t_elements *elements);
void playGame                      (t_game *game, t_elements *elements);
void moveObjects                   (t_game *game, t_elements *elements, int clock);
int  testColisions                 (t_game *game, t_elements *elements);
void printScreen                   (t_game *game, t_elements *elements);
int  input                         (t_game *game, t_elements *elements);
void levelUP                       (t_game *game, t_elements *elements);

/* Colisions Lib */
void testShotsColisions            (t_game *game, t_elements *elements);
int  testBombsColisions            (t_game *game, t_elements *elements);
int  testAliensColisions           (t_game *game, t_elements *elements);
int  hitSpaceship                  (t_lista *bombs, t_lista *spaceship);
int  aliensLoseCondition           (t_game *game, t_lista *aliens);
void removeBombsOnBorder           (t_game *game, t_lista *bombs);
void removeShotOnBorder            (t_lista *shots);
int  isAlienAndBarrier             (int alienType, int barrierType);
int  addScore                      (int shotType, int enemyType);
int  listsCrashTest                (t_lista *list1, t_lista *list2);
int  crash                         (int x1, int y1, int x1Size, int y1Size, int x2, int y2, int x2Size, int y2Size);

/* Add elements Lib */
void addShot                       (t_elements *elements);
void addBomb                       (t_elements *elements);
void addBarriers                   (t_game *game, t_elements *elements);
void addSpaceship                  (t_game *game, t_elements *elements);
void addAliens                     (t_game *game, t_elements *elements);
void addMothership                 (t_game *game, t_elements *elements);
void addSingleBarrier              (int i, int j, t_elements *elements);
void getSpaceshipShootingPos       (int *x, int *y, t_elements *elements);
void getAlienShootingPos           (int *x, int *y, t_elements *elements);


/* Moving Lib */
int  canMoveLeft                   (t_game *game, t_lista *list);
int  canMoveRight                  (t_game *game, t_lista *list);
void moveShots                     (t_lista *shots);
void moveBombs                     (t_lista *bombs, int clock);
void moveSpaceshipLeft             (t_game *game, t_elements *elements);
void moveSpaceshipRight            (t_game *game, t_elements *elements);
void moveAliens                    (t_game *game, t_elements *elements, int clock);
void moveAliensRight               (t_elements *elements);
void moveAliensLeft                (t_elements *elements);
void moveAliensDown                (t_elements *elements);
int  aliensCanMoveRight            (t_game *game, t_lista *aliens);
int  aliensCanMoveLeft             (t_game *game, t_lista *aliens);
void moveMothership                (t_game *game, t_elements *elements, int clock);
void resetMothership               (t_game *game, t_elements *elements);


/* Prints Lib */
void printAlien                    (int xPos, int yPos, int type);
void printAllAliens                (t_elements *elements);
void printBarriers                 (t_elements *elements);
void printShots                    (t_elements *elements);
void printBombs                    (t_elements *elements);
void printSpaceship                (t_elements *elements);
void printMothership               (t_elements *elements);
void printBorders                  (t_game *game);
void printScore                    (t_game *game);
void printLose                     (t_game *game);
int  nDigits                       (int n);




int main ()
{
	t_game game;
	t_elements elements;

	srand (time(0));

	if (!setupInput (&game))
		return 1;

	startGame (&game, &elements);

	playGame (&game, &elements);

	destroyLists (&elements);

	endwin();
	return 0;
}

int setupInput (t_game *game)
{
	initscr();
	cbreak();
	noecho();
	nodelay(stdscr, TRUE);
	keypad(stdscr, TRUE);
	curs_set(FALSE);

	if (!validateWindowSize (game))
		return 0;

	return 1;
}

int validateWindowSize (t_game *game)
{
	getmaxyx (stdscr, game->maxRows, game->maxCols);

	if (game->maxRows < 38 || game->maxCols < 100)
	{
		endwin();
		printf ("Window is too small.\nMinimum size: 38x100.\nCurrent size: %dx%d\n",game->maxRows,game->maxCols);
		return 0;
	}

	return 1;	
}

void startGame(t_game *game, t_elements *elements)
{
	game->score = 0;
	game->level = 1;
	elements->aliensWay = 1;

	inicializeLists (elements);

	addAliens     (game, elements);
	addBarriers   (game, elements);
	addSpaceship  (game, elements);
	addMothership (game, elements);

	printScreen   (game, elements);
}

void inicializeLists (t_elements *elements)
{
	inicializa_lista (&elements->aliens);
	inicializa_lista (&elements->barriers);
	inicializa_lista (&elements->shots);
	inicializa_lista (&elements->bombs);
	inicializa_lista (&elements->spaceship);
	inicializa_lista (&elements->mothership);
}

void addAliens (t_game *game, t_elements *elements)
{
	/*TO DO: aliens da primeira fileira devem ser 3x3*/
	int xIni = 7;
	int yIni = 1;
	int xSize = 3;
	int ySize = 5;
	int xSpacing = 4;
	int ySpacing = 7;
	int alienStatus = 1;
	int alienType;

	int i, j, row = 1;

	for (i = xIni; i < (5 * xSpacing + xIni); i += xSpacing)
	{
		for (j = yIni; j < (11 * ySpacing + yIni); j+= ySpacing)
		{
			if (row == 1)
				alienType = 1;
			if (row == 2 || row == 3)
				alienType = 2;
			if (row == 4 || row == 5)
				alienType = 3;

			insere_fim_lista (i, j, xSize, ySize, alienType, alienStatus, ALIEN_BASE_SPEED + (game->level * SPEED_LV_UP), &elements->aliens);
		}

		row++;
	}
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
	int barrierStatus = 5;
	int barrierSpeed = 0;
	int xSize = 3;
	int ySize = 7;
	int barrierType;

	int i, j, count = 1;

	for (i = xPos; i < (xPos + xSize); i++)
		for (j = yPos; j < (yPos + ySize); j++) 
		{
			/* Make the barrier gaps */
			if (count != 1 && count != 7 && count != 17 && count != 18 && count != 19)
			{
				if (count == 2 || count == 6 || count == 8 || count == 14)
					barrierType = 4; 
				else 
					barrierType = 5;

				insere_fim_lista (i, j, 1, 1, barrierType, barrierStatus, barrierSpeed, &elements->barriers);
			}

			count++;
		}
}

void addMothership (t_game *game, t_elements *elements)
{
	int xSize = 3;
	int ySize = 9;
	int xIni = 3;
	int yIni = 1;
	int mothershipType = 9;
	int mothershipStatus = 0;

	insere_fim_lista (xIni, yIni, xSize, ySize, mothershipType, mothershipStatus, MOTHERSHIP_SPEED, &elements->mothership);
}

void addSpaceship (t_game *game, t_elements *elements)
{
	int xSize = 2;
	int ySize = 5;
	int xIni = game->maxRows-xSize-1;
	int yIni = (game->maxCols-ySize)/2;
	int spaceshipType = 8;
	int spaceshipStatus = 0;

	insere_fim_lista (xIni, yIni, xSize, ySize, spaceshipType, spaceshipStatus, SPACESHIP_SPEED, &elements->spaceship);
}

void playGame(t_game *game, t_elements *elements)
{
	int clockCounter = 1;

	while (1)
	{
		if (!input (game, elements))
			return;

		moveObjects (game, elements, clockCounter);

		if (!testColisions (game, elements))
		{
			printLose (game);
			sleep (5);
			return;
		}

		printScreen (game, elements);

		usleep(CLOCK);		

		if (lista_vazia (&elements->aliens))
			levelUP (game, elements);

		clockCounter++;
	}
}

void levelUP (t_game *game, t_elements *elements)
{
	game->level++;	

	destroyLists    (elements);
	inicializeLists (elements);

	addAliens     (game, elements);
	addBarriers   (game, elements);
	addSpaceship  (game, elements);
	addMothership (game, elements);

	sleep(2);
}

void destroyLists (t_elements *elements)
{
	destroi_lista (&elements->aliens);
	destroi_lista (&elements->barriers);
	destroi_lista (&elements->shots);
	destroi_lista (&elements->bombs);
	destroi_lista (&elements->spaceship);
	destroi_lista (&elements->mothership);
}

int testColisions (t_game *game, t_elements *elements)
{
	if (!testAliensColisions (game, elements))
 		return 0;

	if (!testBombsColisions  (game, elements))
		return 0;

	testShotsColisions  (game, elements);

	return 1;
}

void testShotsColisions (t_game *game, t_elements *elements)
{
	int score = 0;

	score += listsCrashTest (&elements->shots, &elements->aliens);
	score += listsCrashTest (&elements->shots, &elements->barriers);
	score += listsCrashTest (&elements->shots, &elements->bombs);
	score += listsCrashTest (&elements->shots, &elements->mothership);

	game->score += score;	

	removeShotOnBorder (&elements->shots);
}

void removeShotOnBorder (t_lista *shots)
{
	if (!inicializa_atual_inicio (shots))
		return;

	int xPos, yPos, xSize, ySize, type, status, speed;

	while (consulta_item_atual (&xPos, &yPos, &xSize, &ySize, &type, &status, &speed, shots))
	{
		if (xPos <= 0)
			remove_item_atual (shots);

		incrementa_atual (shots);
	}
}

void removeBombsOnBorder (t_game *game, t_lista *bombs)
{
	if (!inicializa_atual_inicio (bombs))
		return;

	int xPos, yPos, xSize, ySize, type, status, speed;

	while (consulta_item_atual (&xPos, &yPos, &xSize, &ySize, &type, &status, &speed, bombs))
	{
		if (xPos >= game->maxRows-1)
			remove_item_atual (bombs);

		incrementa_atual (bombs);
	}
}

int testBombsColisions (t_game *game, t_elements *elements)
{
	listsCrashTest (&elements->bombs, &elements->barriers);
	removeBombsOnBorder (game, &elements->bombs);

	if (hitSpaceship (&elements->bombs, &elements->spaceship))
		return 0;

	return 1;
}

int hitSpaceship (t_lista *bombs, t_lista *spaceship)
{
	inicializa_atual_inicio (bombs);
	inicializa_atual_inicio (spaceship);

	int xPosB, yPosB, xSizeB, ySizeB, typeB, statusB, speedB;
	int xPosS, yPosS, xSizeS, ySizeS, typeS, statusS, speedS;

	consulta_item_atual (&xPosS, &yPosS, &xSizeS, &ySizeS, &typeS, &statusS, &speedS, spaceship);

	while (consulta_item_atual (&xPosB, &yPosB, &xSizeB, &ySizeB, &typeB, &statusB, &speedB, bombs))
	{
		if (crash (xPosB, yPosB, xSizeB, ySizeB, xPosS, yPosS, xSizeS, ySizeS))
			return 1;

		incrementa_atual (bombs);
	}

	return 0;	
}

int testAliensColisions (t_game *game, t_elements *elements)
{
	listsCrashTest (&elements->aliens, &elements->barriers);
	if (aliensLoseCondition (game, &elements->aliens))
 		return 0;

	return 1;
}

int aliensLoseCondition (t_game *game, t_lista *aliens)
{
	inicializa_atual_inicio (aliens);

	int xPos, yPos, xSize, ySize, type, status, speed;

	while (consulta_item_atual (&xPos, &yPos, &xSize, &ySize, &type, &status, &speed, aliens))
	{
		if ((xPos+xSize-1) >= (game->maxRows-3))
			return 1;
		
		incrementa_atual (aliens);
	}

	return 0;	
}


/* Return the sum of the score generated by the colisions */
int listsCrashTest (t_lista *list1, t_lista *list2)
{
	if (!inicializa_atual_inicio (list1))
		return 0;

	int score = 0i;

	int xPos1, yPos1, xSize1, ySize1, type1, status1, speed1;
	int xPos2, yPos2, xSize2, ySize2, type2, status2, speed2;

	while (consulta_item_atual (&xPos1, &yPos1, &xSize1, &ySize1, &type1, &status1, &speed1, list1))
	{
		if (!inicializa_atual_inicio (list2))
			return 0;

		while (consulta_item_atual (&xPos2, &yPos2, &xSize2, &ySize2, &type2, &status2, &speed2, list2))
		{
			if (crash (xPos1, yPos1, xSize1, ySize1, xPos2, yPos2, xSize2, ySize2))
			{
				/* If a alien colide with a barrier, the alien doesnt die */	
				if (! isAlienAndBarrier (type1, type2) )
					remove_item_atual (list1);

				remove_item_atual (list2);

				score += addScore(type1, type2);
			}
			
			else
				incrementa_atual (list2);
		}

		incrementa_atual (list1);
	}

	return score;
}

int isAlienAndBarrier (int alienType, int barrierType)
{
	if (alienType == 1 || alienType == 2 || alienType == 3)
		if (barrierType == 4 || barrierType == 5)
			return 1;

	return 0;
}

int addScore (int shotType, int enemyType)
{
	if (shotType == 6)
	{
		if (enemyType == 1 || enemyType == 2 || enemyType == 3)
			return ALIEN_POINTS;

		if (enemyType == 4 || enemyType == 5)
			return BARRIER_POINTS;

		if (enemyType == 7)
			return BOMB_POINTS;

		if (enemyType == 9)
			return MOTHERSHIP_POINTS;
	}

	return 0;
}

/* Check if some object is overlapping another object */
int crash (int x1, int y1, int x1Size, int y1Size, int x2, int y2, int x2Size, int y2Size)
{
	int i,j,k,l;

	for (i = x1; i < x1 + x1Size; i++)
		for (j = y1; j< y1 + y1Size; j++)
			for (k = x2; k < x2 + x2Size; k++)
				for (l = y2; l < y2 + y2Size; l++)
					if (i == k && j == l)
						return 1;

	return 0;
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

/* 
 * ================ ADD ELEMENTS ===============
*/
void addShot (t_elements *elements)
{
	int xPos, yPos;
	int xSize = 1;
	int ySize = 1;
	int type = 6;
	int status = 0;

	getSpaceshipShootingPos (&xPos, &yPos, elements);

	insere_fim_lista (xPos, yPos, xSize, ySize, type, status, SHOT_SPEED, &elements->shots);
}

void getSpaceshipShootingPos (int *x, int *y, t_elements *elements)
{
	inicializa_atual_inicio (&elements->spaceship);

	int xPos, yPos, xSize, ySize, type, status, speed;
	consulta_item_atual (&xPos, &yPos, &xSize, &ySize, &type, &status, &speed, &elements->spaceship);

	*x = xPos;
	*y = yPos + (ySize/2);
}

void addBomb (t_elements *elements)
{
	int xPos, yPos;
	int xSize = 1;
	int ySize = 1;
	int type = 7;
	int status = 0;

	getAlienShootingPos (&xPos, &yPos, elements);

	insere_fim_lista (xPos, yPos, xSize, ySize, type, status, BOMB_SPEED, &elements->bombs);
}

void getAlienShootingPos (int *x, int *y, t_elements *elements)
{
	int xPos, yPos, xSize, ySize, type, status, speed;
	consulta_item_atual (&xPos, &yPos, &xSize, &ySize, &type, &status, &speed, &elements->aliens);

	*x = xPos + xSize;
	*y = yPos + ySize/2;
}


/*
	=================== MOVE ====================
*/

void moveSpaceshipLeft (t_game *game, t_elements *elements)
{
	inicializa_atual_inicio (&elements->spaceship);

	if (canMoveLeft (game, &elements->spaceship))
		decrementa_y_atual (&elements->spaceship);	
}

void moveSpaceshipRight (t_game *game, t_elements *elements)
{
	inicializa_atual_inicio (&elements->spaceship);

	if (canMoveRight (game, &elements->spaceship))
		incrementa_y_atual (&elements->spaceship);	
}

int canMoveLeft (t_game *game, t_lista *list)
{
	int xPos, yPos, xSize, ySize, type, status, speed;
	consulta_item_atual (&xPos, &yPos, &xSize, &ySize, &type, &status, &speed, list);
	
	if (yPos > 1)
		return 1;

	return 0;
}

int canMoveRight (t_game *game, t_lista *list)
{
	int xPos, yPos, xSize, ySize, type, status, speed;
	consulta_item_atual (&xPos, &yPos, &xSize, &ySize, &type, &status, &speed, list);
	
	if (yPos < game->maxCols -ySize -1)
		return 1;

	return 0;
}

void moveObjects (t_game *game, t_elements *elements, int clock)
{
	moveShots (&elements->shots);

	moveAliens (game, elements, clock);

	moveBombs (&elements->bombs, clock);

	moveMothership (game, elements, clock);	
}

void moveMothership (t_game *game, t_elements *elements, int clock)
{
	if (!inicializa_atual_inicio (&elements->mothership))
		return;

	int x, y, xSize, ySize, mothershipType, mothershipStatus, mothershipSpeed;
	consulta_item_atual (&x, &y, &xSize, &ySize, &mothershipType, &mothershipStatus, &mothershipSpeed, &elements->mothership);

	if (mothershipStatus == 0)
	{
		if (rand()%(10000/MOTHERSHIP_FREQUENCY) == 0)
			muda_status_atual (1, &elements->mothership);
	}

	else
	{
		if (mothershipSpeed > 0 && mothershipSpeed <= 100)
		{
			if (clock % (100/MOTHERSHIP_SPEED) == 0)
			{
				if (canMoveRight(game, &elements->mothership))
					incrementa_y_atual (&elements->mothership);

				else
					resetMothership (game, elements);	
			}
		}
	}
}

void resetMothership (t_game *game, t_elements *elements)
{
	remove_item_atual (&elements->mothership);
	addMothership (game, elements);
}

void moveAliens (t_game *game, t_elements *elements, int clock)
{
	if (!inicializa_atual_inicio (&elements->aliens))
		return;

	if (lista_vazia(&elements->aliens))
		return;

	int x, y, xSize, ySize, alienType, alienStatus, alienSpeed;
	consulta_item_atual (&x, &y, &xSize, &ySize, &alienType, &alienStatus, &alienSpeed, &elements->aliens);
	
	if ( clock % (100/alienSpeed) == 0 )
	{
		if (elements->aliensWay == 1)
		{
			if (aliensCanMoveRight (game, &elements->aliens))
				moveAliensRight (elements);
			else
				moveAliensDown (elements);
		}

		if (elements->aliensWay == -1)
		{
			if (aliensCanMoveLeft (game, &elements->aliens))
				moveAliensLeft (elements);
			else
				moveAliensDown (elements);
		}
	}
}

int aliensCanMoveRight (t_game *game, t_lista *aliens)
{
	if (!inicializa_atual_inicio (aliens))
		return 0;

	int xPos, yPos, xSize, ySize, type, status, speed;

	while (consulta_item_atual (&xPos, &yPos, &xSize, &ySize, &type, &status, &speed, aliens))
	{
		if (!canMoveRight(game, aliens))
			return 0;

		incrementa_atual (aliens);
	}

	return 1;
}

void moveAliensRight (t_elements *elements)
{
	inicializa_atual_inicio (&elements->aliens);

	while (incrementa_y_atual(&elements->aliens))
	{
		if (rand()%(10000/BOMB_FREQUENCY) == 0)
			addBomb (elements);

		incrementa_atual (&elements->aliens);
	}
}

int aliensCanMoveLeft (t_game *game, t_lista *aliens)
{
	if (!inicializa_atual_inicio (aliens))
		return 0;

	int xPos, yPos, xSize, ySize, type, status, speed;

	while (consulta_item_atual (&xPos, &yPos, &xSize, &ySize, &type, &status, &speed, aliens))
	{
		if (!canMoveLeft(game, aliens))
			return 0;

		incrementa_atual (aliens);
	}

	return 1;
}

void moveAliensLeft (t_elements *elements)
{
	if (!inicializa_atual_inicio (&elements->aliens))
		return;

	while (decrementa_y_atual(&elements->aliens))
	{
		if (rand()%(10000/BOMB_FREQUENCY) == 0)
			addBomb (elements);

		incrementa_atual (&elements->aliens);
	}
}

void moveAliensDown (t_elements *elements)
{
	if (!inicializa_atual_inicio (&elements->aliens))
		return;

	while (incrementa_x_atual(&elements->aliens))
	{
		incrementa_speed_atual (1, &elements->aliens);
		incrementa_atual (&elements->aliens);
	}

	/* Change the orientation of the aliens */
	elements->aliensWay *= -1;	
}

void moveShots (t_lista *shots)
{
	if (!inicializa_atual_inicio (shots))
		return;

	while (decrementa_x_atual (shots))
		incrementa_atual (shots);
}

void moveBombs (t_lista *bombs, int clock)
{
	if (clock % (100/BOMB_SPEED) == 0)
	{
		if (!inicializa_atual_inicio (bombs))
			return;

		while (incrementa_x_atual (bombs))
			incrementa_atual (bombs);
	}
}








/*
	=============== PRINTS =================
*/

void printScreen (t_game *game, t_elements *elements)
{
	erase();

	printBarriers   (elements);
	printShots      (elements);
	printBombs      (elements);
	printAllAliens  (elements);
	printSpaceship  (elements);
	printMothership (elements);
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
		printAlien (xPos, yPos, type);

		incrementa_atual (&elements->aliens);
	}
}

void printAlien (int xPos, int yPos, int type)
{
	switch (type)
	{
		case 1:
			mvprintw (xPos  , yPos, ALIEN1_1);
			mvprintw (xPos+1, yPos, ALIEN1_2);
			mvprintw (xPos+2, yPos, ALIEN1_3);
		break;

		case 2:
			mvprintw (xPos  , yPos, ALIEN2_1);
			mvprintw (xPos+1, yPos, ALIEN2_2);
			mvprintw (xPos+2, yPos, ALIEN2_3);
		break;

		case 3:
			mvprintw (xPos  , yPos, ALIEN3_1);
			mvprintw (xPos+1, yPos, ALIEN3_2);
			mvprintw (xPos+2, yPos, ALIEN3_3);
		break;
	}

}

void printBarriers (t_elements *elements)
{
	if (!inicializa_atual_inicio (&elements->barriers))
		return;

	int xPos, yPos, xSize, ySize, type, status, speed;	

	while (consulta_item_atual(&xPos, &yPos, &xSize, &ySize, &type, &status, &speed, &elements->barriers))
	{
		if (type == 4)
			mvaddch (xPos, yPos, BARRIER1);
		if (type == 5)
			mvaddch (xPos, yPos, BARRIER2);
		
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
		mvaddch (xPos, yPos, SHOT);
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
		mvaddch (xPos, yPos, BOMB);
		incrementa_atual (&elements->bombs);
	}
}

void printSpaceship (t_elements *elements)
{
	if (!inicializa_atual_inicio (&elements->spaceship))
		return;

	int xPos, yPos, xSize, ySize, type, status, speed;

	consulta_item_atual (&xPos, &yPos, &xSize, &ySize, &type, &status, &speed, &elements->spaceship);

	mvprintw (xPos  , yPos, SPACESHIP1);
	mvprintw (xPos+1, yPos, SPACESHIP2);
}

void printMothership (t_elements *elements)
{
	if (!inicializa_atual_inicio (&elements->mothership))
		return;

	int xPos, yPos, xSize, ySize, type, status, speed;

	consulta_item_atual (&xPos, &yPos, &xSize, &ySize, &type, &status, &speed, &elements->mothership);

	if (status == 1)
	{
		mvprintw (xPos  , yPos, MOTHERSHIP1);
		mvprintw (xPos+1, yPos, MOTHERSHIP2);
		mvprintw (xPos+2, yPos, MOTHERSHIP3);
	}
}

void printScore (t_game *game)
{
	int yPosLevel = (game->maxCols-8)/2;
	int yPosScore = (game->maxCols-10)/2;

	mvprintw (1, yPosLevel, "Level %d", game->level);
	mvprintw (2, yPosScore, "%7d", game->score);
}

int nDigits (int n)
{
	if (n%10 == 0)
		return 0;

	return 1 + (nDigits (n/10));
}

/* arrumar */
void printLose (t_game *game)
{
	char loseMessege[20] = "AAAAAAAAAAAAAAAAAAA";
	int messegeSize = strlen (loseMessege);

	mvprintw (4, (game->maxCols - messegeSize)/2, "%s", loseMessege);

}
