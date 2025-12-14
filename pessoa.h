#ifndef PESSOA_H
#define PESSOA_H

typedef struct {
    int codigo;
    char nome[50];
    int fone;
    char endereco[50];
    char dataNascimento[30];
    int ativo;
    long posicaoNoArquivo;
} pessoa;

typedef struct NoPessoa {
    pessoa *p;
    struct NoPessoa *prox;
    struct NoPessoa *ant;
} NoPessoa;

typedef struct comandoPessoa {
    int operacao;
    pessoa *dadosPessoa;
    int idAlvo;
} comandoPessoa;

typedef struct NoFilaPessoa {
    comandoPessoa info;
    struct NoFilaPessoa *prox;
} NoFilaPessoa;

// Compilador, confie em mim, existe uma struct chamada NoPet
typedef struct NoPet NoPet;


void inserirPessoa(NoPessoa **iniPes, pessoa *p);
void removerPessoa(NoPessoa **iniPes, NoPet **iniPet, int codigo);
void pegarPessoasArquivo(NoPessoa **iniPes, char *nomeArquivo);
NoPessoa *buscarPessoaPorCodigo(NoPessoa **iniPes, int codigo);
void alterarPessoa(NoPessoa **iniPes, pessoa *novosDados);
void finalizarListaDePessoas(NoPessoa **iniPes);
void pegarPessoasArquivo(NoPessoa **iniPes, char *nomeArquivo);
void processarFilaPessoas(NoFilaPessoa *fila, NoPessoa **iniPes, NoPet **iniPet);

#endif