#ifndef PET_H

typedef struct {
    int codigo;
    int codigo_pes;
    char nome[50];
    int codigo_tipo;
} pet;


typedef struct NoPet{
    pet *p;
    struct NoPet *prox;
    struct NoPet *ant;
} NoPet;

typedef struct comandoPet {
    int operacao;
    pet *dadosPet;
    int idAlvo;
} comandoPet;

typedef struct NoFilaPet {
    comandoPet info;
    struct NoFilaPet *prox;
} NoFilaPet;

// Compilador , confie em mim, existe structs chamadas NoPessoa e NoTipoDePet
typedef struct NoPessoa NoPessoa;
typedef struct NoTipoDePet NoTipoDePet;


void inserirPet(NoPet **iniPet,NoPessoa **iniPes, NoTipoDePet **iniTipoDePet, pet *p);
void removerPet(NoPet **iniPet, int codigo);
NoPet *buscarPetPorCodigo(NoPet **iniPet, int codigo);
void alterarPet(NoPet **iniPet, NoPessoa **iniPes, NoTipoDePet **iniTipoDePet, pet *novosDados);
void finalizarListaDePets(NoPet **iniPet);
void pegarPetsArquivo(NoPet **iniPet, NoPessoa **iniPes, NoTipoDePet **iniTipoDePet, char *nomeArquivo);
void salvarPetsNoArquivo(NoPet **iniPet, char *nomeArquivo);
void processarFilaPets(NoFilaPet *fila, NoPet **iniPet, NoPessoa **iniPes, NoTipoDePet **iniTipoDePet);

#endif

