#ifndef TIPO_PET_H

typedef struct {
    int codigo;
    char nome[50];
} tipoPet;

typedef struct NoTipoPet{
    tipoPet *p;
    struct NoTipoDePet *prox;
    struct NoTipoDePet *ant;
} NoTipoDePet;

void inserirTipoDePet(NoTipoDePet **iniTipoDePet, tipoPet *p);

void pegarTiposDePetArquivo(NoTipoDePet **iniTipoDePet, char *nomeArquivo);

void salvarTiposDePetNoArquivo(NoTipoDePet **iniTipoDePet, char *nomeArquivo);



#endif