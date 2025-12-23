#ifndef PET_H
#define PET_H

typedef struct {
    int codigo;
    int codigo_pes;
    char nome[50];
    int codigo_tipo;
    int ativo;
    long posicaoNoArquivo;
} pet;


typedef struct NoPet{
    pet *p;
    struct NoPet *prox;
    struct NoPet *ant;
} NoPet;

typedef struct comandoPet {
    int operacao;
    pet *dadosPet;
    char tipoFiltragem[50];
    char criterioOrdenacao[50];
} comandoPet;

typedef struct NoFilaPet {
    comandoPet info;
    struct NoFilaPet *prox;
} NoFilaPet;

// Parte de ordenação
typedef struct NoArvorePet {
    struct NoArvorePet *esq;
    struct NoArvorePet *dir;
    pet *p;
} NoArvorePet;

// Compilador , confie em mim, existe structs chamadas NoPessoa e NoTipoDePet
typedef struct NoPessoa NoPessoa;
typedef struct NoTipoDePet NoTipoDePet;


void inserirPet(NoPet **iniPet,NoPessoa **iniPes, NoTipoDePet **iniTipoDePet, pet *p);
void removerPet(NoPet **iniPet, int codigo);
void removerPetPorCriterio(NoPet **iniPet, comandoPet cmd);
NoPet *buscarPetPorCodigo(NoPet **iniPet, int codigo);
void alterarPet(NoPet **iniPet, NoPessoa **iniPes, NoTipoDePet **iniTipoDePet, pet *novosDados);
void alterarPetPorCriterio(NoPet **iniPet, NoPessoa **iniPes, NoTipoDePet **iniTipoDePet, comandoPet cmd);
void inserirNaArvorePet(NoArvorePet **raiz, pet *p, char *criterioOrdenacao);
void exibirArvorePet(NoArvorePet *raiz);
void liberarArvorePet(NoArvorePet *raiz);
void executarSelectPet(NoPet *lista, comandoPet cmd);
void finalizarListaDePets(NoPet **iniPet);
void pegarPetsArquivo(NoPet **iniPet, NoPessoa **iniPes, NoTipoDePet **iniTipoDePet, char *nomeArquivo);
void salvarPetsNoArquivo(NoPet **iniPet, char *nomeArquivo);
void processarFilaPets(NoFilaPet *fila, NoPet **iniPet, NoPessoa **iniPes, NoTipoDePet **iniTipoDePet);
void mostrarPets(NoPet **iniPet);


#endif

