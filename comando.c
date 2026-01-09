#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "comando.h"

Fila *criar_fila()
{
    //aloca espaço para a fila
    Fila *f = (Fila *)malloc(sizeof(Fila));

    //inicializar valores
    f->ini = NULL;
    f->fim = NULL;
    f->tam = 0;
    return f;
}

void fila_cmd(Fila **fila, char *texto)
{
    //cria novo nó
    NoComando *novo = (NoComando *)malloc(sizeof(NoComando));

    //busca se tem "enter" (\n) no comando e remove
    texto[strcspn(texto, "\n")] = 0;

    //copia comando recebido para dentro do novo nó
    strcpy(novo->info.linhacompleta, texto);

    novo->info.id = (*fila)->tam + 1;
    novo->prox = NULL;

    //verifica se fila ta vazia
    if ((*fila)->ini == NULL)
    {
        novo->ant = NULL;
        (*fila)->ini = novo;
        (*fila)->fim = novo;
    }
    //verifica se fila tem elementos
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

    //se o arquivo nao existir, cria um novo arquivo vazio
    if (arquivo == NULL)
    {
        arquivo = fopen(nome_arquivo, "w");
        if(arquivo) {
            fclose(arquivo);
        }
        return;
    }
    //o 'cursor' indica a posicao onde a leitura parou e o fseek pula tudo que ja foi lido
    //e vai direto ao ponto novo.
    fseek(arquivo, *cursor, SEEK_SET);

    char buffer[tam_linha]; //variavel temporaria para montar o comando
    int indice = 0;//posição atual dentro do buffer
    char c;

    //iniciando leitura do arquivo
    while ( (c = fgetc(arquivo)) != EOF) 
    {
        //se encontrar ';', finaliza o comando
        if(c == ';') {
            buffer[indice] = '\0';

            //se leu algo valido, adiciona na fila
            if(indice > 0) {
                fila_cmd(fila, buffer);
            }
            indice = 0; //reseta o indice para o proximo comando

        } else if (c == '\n' || c == '\t' || c == '\r') {

            //se houver espaço, quebra de linha, tab, substitui por espaço
            if(indice > 0 && indice < tam_linha - 1 ) {
                    buffer[indice] = ' ';
                    indice++;
            }
            //se for letra ou numero, adiciona ao buffer
        } else {
            if(indice < tam_linha - 1) {
                buffer[indice] = c;
                indice++;
            }
        }
    }
    //atualiza o cursor para a posicao atual do arquivo
    *cursor = ftell(arquivo);

    fclose(arquivo);
}

void imprimir_fila(Fila **fila)
{
    //verifica se a fila é nula, caso for retorna
    if (fila == NULL || *fila == NULL)
    {
        return;
    }
    //cria ponteiro auxiliar começando do inicio
    NoComando *atual = (*fila)->ini;
    printf("\nConteudo da FILA\n");
    while (atual != NULL)
    {
        //imprime o comando atual e avança para o próximo
        printf("[%d] %s\n", atual->info.id, atual->info.linhacompleta);
        atual = atual->prox;
    }
}
