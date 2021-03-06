struct t_nodo {
    struct t_nodo *prox;
    struct t_nodo *prev;
    int pos_x;
    int pos_y;
    int tam_x;
    int tam_y;
    int tipo;
    int status;
    int speed;
};
typedef struct t_nodo t_nodo;

struct t_lista {
    t_nodo *ini;
    t_nodo *atual;
    t_nodo *fim;
    int tamanho;
};
typedef struct t_lista t_lista;

/*
  Cria uma lista vazia. Ela eh duplamente encadeada e tem sentinelas no
  inicio e no final. Tambem tem um apontador para um elemento qualquer.
*/
int inicializa_lista(t_lista *l);

/*
  Retorna 1 se a lista está vazia e zero caso contrário.
*/
int lista_vazia(t_lista *l);

/*
  Remove todos os elementos da lista e faz com que ela aponte para NULL.
*/
void destroi_lista(t_lista *l);

/*
  Retorna o tamanho da lista em *tam.
  Retorna 1 se a operação foi bem sucedida e zero caso contrário.
*/
int tamanho_lista(int *tam, t_lista *l);

/*
  Insere o elemento item no final da lista.
  Retorna 1 se a operação foi bem sucedida e zero caso contrário.
*/
int insere_fim_lista(int pos_x, int pos_y, int tam_x, int tam_y, int tipo, int status, int speed, t_lista *l);

/* 
  Inicializa o ponteiro atual para o primeiro elemento da lista.
  Retorna 1 se a operação foi bem sucedida e zero caso contrário.
*/
int inicializa_atual_inicio(t_lista *l);

/*
  Faz o ponteiro atual apontar para o próximo nodo da lista l e retorna 
  este ponteiro. Se atual estiver apontando para o último, isto é, não 
  tem próximo, retorna NULL.
*/
void incrementa_atual(t_lista *l);

/*
  Retorna em *item o valor contido na chave apontada pelo ponteiro atual. 
  Se atual não for válido a função retorna zero senão retorna 1.
*/
int consulta_item_atual(int *pos_x, int *pos_y, int *tam_x, int *tam_y, int *tipo, int *status, int *speed, t_lista *atual);

/*
  Remove o elemento apontado por atual da lista l e o retorna em *item.
  Faz o atual apontar para o sucessor do nodo removido.
  Retorna 1 se houve sucesso e zero caso contrário.
*/
int remove_item_atual(t_lista *l);

/*
  Incrementa/decrementa o pos_x ou pos_y do nodo atual.
*/
int incrementa_y_atual (t_lista *l);
int decrementa_y_atual (t_lista *l);
int incrementa_x_atual (t_lista *l);
int decrementa_x_atual (t_lista *l);

/*
  Muda o status do nodo atual.
*/
int muda_status_atual (int status, t_lista *l);

/*
  Incrementa a speed do nodo atual em speed_inc.
*/
void incrementa_speed_atual (int speed_inc, t_lista *l);
