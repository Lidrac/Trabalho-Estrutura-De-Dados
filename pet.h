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


void inserirPet(NoPet **iniPet, pet *p);
void pegarPetsArquivo(NoPet **iniPet, char *nomeArquivo);
void salvarPetsNoArquivo(NoPet **iniPet, char *nomeArquivo);
void removerPet(NoPet **iniPet, int codigo);
NoPet *buscarPetPorCodigo(NoPet **iniPet, int codigo);

#endif

