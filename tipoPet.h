#ifndef TIPO_PET_H
#define TIPO_PET_H

typedef struct {
    int codigo;
    char nome[50];
    int ativo;
    long posicaoNoArquivo;
} tipoPet;

typedef struct NoTipoDePet {
    tipoPet *p;
    struct NoTipoDePet *prox;
    struct NoTipoDePet *ant;
} NoTipoDePet;

typedef struct comandoTipoPet {
    int operacao;
    tipoPet *dadosTipoDePet;
    char tipoFiltragem[50];
    char criterioOrdenacao[50];
} comandoTipoPet;

typedef struct NoFilaTipoPet {
    comandoTipoPet info;
    struct NoFilaTipoPet *prox;
} NoFilaTipoPet;

// Parte de ordenação
typedef struct NoArvoreTipoDePet {
    struct NoArvoreTipoDePet *esq;
    struct NoArvoreTipoDePet *dir;
    tipoPet *p;
} NoArvoreTipoDePet;

// Compilador, confie em mim, existe uma struct chamada NoPet
typedef struct NoPet NoPet;

void inserirTipoDePet(NoTipoDePet **iniTipoDePet, tipoPet *p);
void removerTipoDePet(NoTipoDePet **iniTipoDePet, NoPet **iniPet, int codigo);
void removerTipoDePetPorCriterio(NoTipoDePet **iniTipoDePet, NoPet **iniPet, comandoTipoPet cmd);
NoTipoDePet *buscarTipoDePetPorCodigo(NoTipoDePet **iniTipoDePet, int codigo);
void alterarTipoDePet(NoTipoDePet **iniTipoDePet, tipoPet *novosDados);
void alterarTipoDePetPorCriterio(NoTipoDePet **iniTipoDePet, NoPet **iniPet, comandoTipoPet cmd);
void inserirNaArvoreTipoPet(NoArvoreTipoDePet **raiz, tipoPet *p, char *criterioOrdenacao);
void exibirArvoreTipoPet(NoArvoreTipoDePet *raiz);
void liberarArvoreTipoPet(NoArvoreTipoDePet *raiz);
void executarSelectTipo(NoTipoDePet *lista, comandoTipoPet cmd);
void finalizarListaDeTiposDePet(NoTipoDePet **iniTipoDePet);
void pegarTiposDePetArquivo(NoTipoDePet **iniTipoDePet, char *nomeArquivo);
void salvarTiposDePetNoArquivo(NoTipoDePet **iniTipoDePet, char *nomeArquivo);
void processarFilaTiposDePets(NoFilaTipoPet *fila, NoTipoDePet **iniTipoDePet, NoPet **iniPet);
void mostrarTiposDePet(NoTipoDePet **iniTipoDePet);


#endif