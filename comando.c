#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "comando.h"

Fila *criar_fila()
{
    Fila *f = (Fila *)malloc(sizeof(Fila));

    f->ini = NULL;
    f->fim = NULL;
    f->tam = 0;
    return f;
}

void fila_cmd(Fila **fila, char *texto)
{
    NoComando *novo = (NoComando *)malloc(sizeof(NoComando));

    texto[strcspn(texto, "\n")] = 0;
    strcpy(novo->info.linhacompleta, texto);

    novo->info.id = (*fila)->tam + 1;
    novo->prox = NULL;

    if ((*fila)->ini == NULL)
    {
        novo->ant = NULL;
        (*fila)->ini = novo;
        (*fila)->fim = novo;
    }
    else
    {
        (*fila)->fim->prox = novo;
        novo->ant = (*fila)->fim;
        (*fila)->fim = novo;
    }
    (*fila)->tam++;
}

void ler_arquivo(Fila **fila, char *nome_arquivo, long *cursor)
{
    FILE *arquivo = fopen(nome_arquivo, "r");

    if (arquivo == NULL)
    {
        arquivo = fopen(nome_arquivo, "w");
        if(arquivo) {
            fclose(arquivo);
        }
        return;
    }

    fseek(arquivo, *cursor, SEEK_SET);

    char buffer[tam_linha];
    int indice = 0;
    char c;

    // iniciando leitura do arquivo.
    while ( (c = fgetc(arquivo)) != EOF) 
    {

        if(c == ';') {
            buffer[indice] = '\0';

            if(indice > 0) {
                fila_cmd(fila, buffer);
            }
            indice = 0;
        } else if (c == '\n' || c == '\t' || c == '\r') {

            if(indice > 0 && indice < tam_linha - 1 ) {
                    buffer[indice] = ' ';
                    indice++;
            }
        } else {
            if(indice < tam_linha - 1) {
                buffer[indice] = c;
                indice++;
            }
        }
    }

    *cursor = ftell(arquivo);

    fclose(arquivo);
}

void imprimir_fila(Fila **fila)
{
    if (fila == NULL || *fila == NULL)
    {
        return;
    }

    NoComando *atual = (*fila)->ini;
    printf("\nConteudo da FILA\n");
    while (atual != NULL)
    {
        printf("[%d] %s\n", atual->info.id, atual->info.linhacompleta);
        atual = atual->prox;
    }
}
