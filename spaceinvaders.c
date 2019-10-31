#include <stdio.h>
#include <unistd.h>
#include <ncurses.h>

int  validateWindowSize (int *rows, int *cols);
void setupInput         ();
void printScreen        (int rows, int cols);

int main ()
{
	initscr ();

	int rows, cols;
	if (validateWindowSize (&rows, &cols))
		return 1;
	
	/*setupInput ();*/

	printScreen ();

	endwin();
	return 0;
}

int validateWindowSize (int *rows, int *cols)
{
	getmaxyx (stdscr, *rows, *cols);

	if (*rows < 38 || *cols < 100)
	{
		printf ("The terminal window is too small.\n");
		endwin();
		return 0;
	}

	return 1;	
}

void setupInput ()
{
	cbreak();               /* desabilita o buffer de entrada */
	noecho();               /* não mostra os caracteres digitados */
	nodelay(stdscr, TRUE);  /* faz com que getch não aguarde a digitação */
	keypad(stdscr, TRUE);   /* permite a leitura das setas */
	curs_set(FALSE);        /* não mostra o cursor na tela */
}

void printScreen (int rows, int cols)
{
	clear();

	int i, j;
	for ( i=0; i < rows; i++ )
		for ( j=0; j < cols; j++ )
		{
			move(i,j);
			addch (m.[i][j]);
		}
}
