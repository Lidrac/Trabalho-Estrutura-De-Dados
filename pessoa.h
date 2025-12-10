#ifndef PESSOA_H
#define PESSOA_H


typedef struct {
    int codigo;
    char nome[50];
    int fone;
    char endereco[50];
    char dataNascimento[30];
} pessoa;

typedef struct NoPessoa {
    pessoa *p;
    struct NoPessoa *prox;
    struct NoPessoa *ant;
} NoPessoa;


void inserirPessoa(NoPessoa **iniPes, pessoa *p);
void pegarPessoasArquivo(NoPessoa **iniPes, char *nomeArquivo);
void pegarPessoasArquivo(NoPessoa **iniPes, char *nomeArquivo);
void removerPessoa(NoPessoa **iniPes, NoPet **iniPet, int codigo);
NoPessoa *buscarPessoaPorCodigo(NoPessoa **iniPes, int codigo);

#endif