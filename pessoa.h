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
    char tipoFiltragem[50];
    char criterioOrdenacao[50];
} comandoPessoa;

typedef struct NoFilaPessoa {
    comandoPessoa info;
    struct NoFilaPessoa *prox;
} NoFilaPessoa;


// Parte de ordenação
typedef struct NoArvorePessoa {
    struct NoArvorePessoa *esq;
    struct NoArvorePessoa *dir;
    pessoa *p;
} NoArvorePessoa;

// Compilador, confie em mim, existe uma struct chamada NoPet
typedef struct NoPet NoPet;


void inserirPessoa(NoPessoa **iniPes, pessoa *p);
void removerPessoa(NoPessoa **iniPes, NoPet **iniPet, int codigo);
void removerPessoaPorCriterio(NoPessoa **iniPes, NoPet **iniPet, comandoPessoa cmd);
void pegarPessoasArquivo(NoPessoa **iniPes, char *nomeArquivo);
NoPessoa *buscarPessoaPorCodigo(NoPessoa **iniPes, int codigo);
void alterarPessoa(NoPessoa **iniPes, pessoa *novosDados);
void alterarPessoaPorCriterio(NoPessoa **iniPes, NoPet **iniPet, comandoPessoa cmd);
void inserirNaArvorePessoa(NoArvorePessoa **raiz, pessoa *p, char *criterioOrdenacao);
void exibirArvorePessoa(NoArvorePessoa *raiz);
void liberarArvorePessoa(NoArvorePessoa *raiz);
void executarSelectPessoa(NoPessoa *lista, comandoPessoa cmd);
void finalizarListaDePessoas(NoPessoa **iniPes);
void pegarPessoasArquivo(NoPessoa **iniPes, char *nomeArquivo);
void processarFilaPessoas(NoFilaPessoa *fila, NoPessoa **iniPes, NoPet **iniPet);
void mostrarPessoas(NoPessoa **iniPes);


#endif