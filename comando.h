#ifndef COMANDO_H
#define COMANDO_H

//define tamanho do comando
#define tam_linha 300

//estrutura que guarda dados
typedef struct
{
    char linhacompleta[tam_linha];
    int id;
} Comando;

// estrutura do nó da fila
typedef struct NoComando
{
    Comando info;
    struct NoComando *prox;
    struct NoComando *ant;
} NoComando;

//controle da fila inicio e fim
typedef struct
{
    NoComando *ini;
    NoComando *fim;
    int tam;
} Fila;

//funçoes
Fila *criar_fila();
void fila_cmd(Fila **fila, char *texto);
void ler_arquivo(Fila **fila, char *nome_arquivo, long *cursor);
void imprimir_fila(Fila **fila);

#endif