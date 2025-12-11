#ifndef TIPO_PET_H

typedef struct {
    int codigo;
    char nome[50];
} tipoPet;

typedef struct NoTipoDePet {
    tipoPet *p;
    struct NoTipoDePet *prox;
    struct NoTipoDePet *ant;
} NoTipoDePet;

typedef struct comandoTipoPet {
    int operacao;
    tipoPet *dadosPet;
    int idAlvo;
} comandoTipoPet;

// Compilador, confie em mim, existe uma struct chamada NoPet
typedef struct NoPet NoPet;

void inserirTipoDePet(NoTipoDePet **iniTipoDePet, tipoPet *p);
void removerTipoDePet(NoTipoDePet **iniTipoDePet, NoPet **iniPet, int codigo);
NoTipoDePet *buscarTipoDePetPorCodigo(NoTipoDePet **iniTipoDePet, int codigo);
void alterarTipoDePet(NoTipoDePet **iniTipoDePet, tipoPet *novosDados);
void finalizarListaDeTiposDePet(NoTipoDePet **iniTipoDePet);
void pegarTiposDePetArquivo(NoTipoDePet **iniTipoDePet, char *nomeArquivo);
void salvarTiposDePetNoArquivo(NoTipoDePet **iniTipoDePet, char *nomeArquivo);



#endif