#include <stdio.h>
#include <stdlib.h>

#include "lib_lista_space.h"

int inicializa_lista(t_lista *l)
{
	t_nodo *sentinel;

	sentinel = (t_nodo*) malloc (sizeof (t_nodo));
	if (sentinel == NULL)
		return 0;

	sentinel->prox = NULL;
	sentinel->prev = NULL;

	l->ini = sentinel;
	
	sentinel = (t_nodo*) malloc (sizeof (t_nodo));
	if (sentinel == NULL)
		return 0;

	sentinel->prox = NULL;
	sentinel->prev = l->ini;

	l->ini->prox = sentinel;
	l->fim = sentinel;

	l->atual = NULL;
	l->tamanho = 0;

	return 1;
}

int lista_vazia(t_lista *l)
{
	if (l->tamanho == 0)
		return 1;

	return 0;
}

void destroi_lista(t_lista *l)
{
	if (l->ini == NULL && l->atual == NULL && l->fim == NULL && l->tamanho == 0)
		return;

	t_nodo *p;

	while (l->ini != NULL)
	{
		p = l->ini;
		l->ini = p->prox;
		free (p);
	}

	l->atual = NULL;
	l->fim = NULL;
	l->tamanho = 0;
}

int tamanho_lista(int *tam, t_lista *l)
{
	if (l->ini == NULL && l->fim == NULL)
		return 0;

	*tam = l->tamanho;
	return 1;
}

int insere_fim_lista(int pos_x, int pos_y, int tam_x, int tam_y, int tipo, int status, int speed, t_lista *l)
{
	t_nodo *new;
	new = (t_nodo*) malloc (sizeof (t_nodo));
	if (new == NULL)
		return 0;

	new->pos_x = pos_x;
	new->pos_y = pos_y;
	new->tam_x = tam_x;
	new->tam_y = tam_y;
	new->tipo = tipo;
	new->status = status;
	new->speed = speed;
	new->prox = l->fim;
	new->prev = l->fim->prev;

	l->fim->prev->prox = new;
	l->fim->prev = new;

	l->tamanho++;

	return 1;		
}

int inicializa_atual_inicio(t_lista *l)
{
	if (lista_vazia(l))
		return 0;

	l->atual = l->ini->prox;
	return 1;
}

void incrementa_atual(t_lista *l)
{
	if (lista_vazia(l) || l->atual == NULL) 
		return;

	if (l->atual == l->fim)
		return;

	l->atual = l->atual->prox;
}

int consulta_item_atual(int *pos_x, int *pos_y, int *tam_x, int *tam_y, int *tipo, int *status, int *speed, t_lista *atual)
{
	if (atual->atual == NULL) 
		return 0;

	if (atual->atual == atual->ini || atual->atual == atual->fim)
		return 0;

	*pos_x = atual->atual->pos_x;
	*pos_y = atual->atual->pos_y;
	*tam_x = atual->atual->tam_x;
	*tam_y = atual->atual->tam_y;
	*tipo = atual->atual->tipo;
	*status = atual->atual->status;
	*speed = atual->atual->speed;
	return 1;
}

int remove_item_atual(t_lista *l)
{
	if (l->atual == NULL)
		return 0;

	if (l->atual == l->ini || l->atual == l->fim)
		return 0;

	t_nodo *p = l->atual;
	p->prev->prox = p->prox;
	p->prox->prev = p->prev;

	l->atual = p->prox;

	free (p);

	l->tamanho--;

	return 1;
}

int incrementa_y_atual (t_lista *l)
{
	if (l->atual == NULL)
		return 0;

	if (l->atual == l->ini || l->atual == l->fim)
		return 0;

	l->atual->pos_y++;

	return 1;
}

int decrementa_y_atual (t_lista *l)
{
	if (l->atual == NULL)
		return 0;

	if (l->atual == l->ini || l->atual == l->fim)
		return 0;

	l->atual->pos_y--;	

	return 1;
}

int incrementa_x_atual (t_lista *l)
{
	if (l->atual == NULL)
		return 0;

	if (l->atual == l->ini || l->atual == l->fim)
		return 0;

	l->atual->pos_x++;	

	return 1;
}

int decrementa_x_atual (t_lista *l)
{
	if (l->atual == NULL)
		return 0;

	if (l->atual == l->ini || l->atual == l->fim)
		return 0;

	l->atual->pos_x--;	

	return 1;
}

int muda_status_atual (int status, t_lista *l)
{
	if (l->atual == NULL)
		return 0;

	if (l->atual == l-> ini || l->atual == l->fim)
		return 0;

	l->atual->status = status;

	return 1;
}

void incrementa_speed_atual (int speed_inc, t_lista *l)
{
	if (l->atual == NULL)
		return;

	if (l->atual == l-> ini || l->atual == l->fim)
		return;

	l->atual->speed += speed_inc;
}
