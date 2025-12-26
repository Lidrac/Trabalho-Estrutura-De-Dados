#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "validador.h"

#ifdef _WIN32
    #define strcasecmp _stricmp
    #define strncasecmp _strnicmp
#endif

// ==================================================================================
// FERRAMENTAS AUXILIARES
// ==================================================================================

char* trim(char *str) {
    char *end;
    while(isspace((unsigned char)*str)) str++;
    if(*str == 0) return str;
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    *(end+1) = 0;
    return str;
}

void limpar_string(char *destino, char *origem) {
    int j = 0;
    while (*origem == ' ') origem++;
    for (int i = 0; origem[i] != '\0'; i++) {
        if (origem[i] != '\'' && origem[i] != '\"') {
            destino[j++] = origem[i];
        }
    }
    while (j > 0 && isspace((unsigned char)destino[j-1])) j--;
    destino[j] = '\0';
}

int comeca_com(char *linha, char *termo) {
    while(isspace((unsigned char)*linha)) linha++;
    size_t len = strlen(termo);
    return strncasecmp(linha, termo, len) == 0;
}

int extrair_id_where(char *linha) {
    char *ptrWhere = strstr(linha, "where");
    if (ptrWhere) {
        char *ptrCod = strstr(ptrWhere, "codigo");
        if (ptrCod) {
            char *ptrIgual = strchr(ptrCod, '=');
            if (ptrIgual) return atoi(ptrIgual + 1);
        }
    }
    return -1;
}

char* extrair_campo(char **cursor){
    char *inicio = *cursor;

    // pula espaços em branco
    while(*inicio && (*inicio == ',' || *inicio == ' ' || *inicio == '\t')){
        inicio++;
    }

    if(*inicio == '\0' || *inicio == ')'){
        return NULL;
    }
    int dentro_aspas = 0;
    char *atual = inicio;

    while (*atual != '\0'){
        if(*atual == '\''){
            dentro_aspas = !dentro_aspas;
        }
        else if (!dentro_aspas && (*atual == ',' || *atual == ')')){
            if(*atual == ')'){
                *atual = '\0';
                *cursor = atual;
            } else {
                *atual = '\0';
                *cursor = atual + 1;
            }
            return trim(inicio);
        }
        atual++;
    }
    *cursor = atual;
    return trim(inicio);
}

// Função pra remover aspas, por que a atoi não remove
int atoi_limpo(char *str) {
    char temp[50];
    limpar_string(temp, str); // Remove aspas antes de converter
    return atoi(temp);
}

// =================================================================================
// ADD FILAS
// =================================================================================

void AddFilaPessoa(NoFilaPessoa **fila, comandoPessoa cmd) {
    NoFilaPessoa *novo = (NoFilaPessoa *)malloc(sizeof(NoFilaPessoa));
    novo->info = cmd; novo->prox = NULL;
    if (*fila == NULL) *fila = novo;
    else {
        NoFilaPessoa *aux = *fila;
        while (aux->prox) aux = aux->prox;
        aux->prox = novo;
    }
}
void AddFilaPet(NoFilaPet **fila, comandoPet cmd) {
    NoFilaPet *novo = (NoFilaPet *)malloc(sizeof(NoFilaPet));
    novo->info = cmd; novo->prox = NULL;
    if (*fila == NULL) *fila = novo;
    else {
        NoFilaPet *aux = *fila;
        while (aux->prox) aux = aux->prox;
        aux->prox = novo;
    }
}
void AddFilaTipoPet(NoFilaTipoPet **fila, comandoTipoPet cmd) {
    NoFilaTipoPet *novo = (NoFilaTipoPet *)malloc(sizeof(NoFilaTipoPet));
    novo->info = cmd; novo->prox = NULL;
    if (*fila == NULL) *fila = novo;
    else {
        NoFilaTipoPet *aux = *fila;
        while (aux->prox) aux = aux->prox;
        aux->prox = novo;
    }
}

// =================================================================================
// FUNÇÃO DE ROLLBACK (LIMPEZA DE EMERGÊNCIA)
// =================================================================================
void limpeza (NoFilaPessoa **fPessoa, NoFilaPet **fPet, NoFilaTipoPet **fTipo) {
    // Limpa Pessoa
    while (*fPessoa) {
        NoFilaPessoa *temp = *fPessoa;
        *fPessoa = (*fPessoa)->prox;
        if(temp->info.dadosPessoa) free(temp->info.dadosPessoa);
        free(temp);
    }
    // Limpa Pet
    while (*fPet) {
        NoFilaPet *temp = *fPet;
        *fPet = (*fPet)->prox;
        if(temp->info.dadosPet) free(temp->info.dadosPet);
        free(temp);
    }
    // Limpa Tipo
    while (*fTipo) {
        NoFilaTipoPet *temp = *fTipo;
        *fTipo = (*fTipo)->prox;
        if(temp->info.dadosTipoDePet) free(temp->info.dadosTipoDePet);
        free(temp);
    }
}

// =================================================================================
// PROCESSADORES 
// =================================================================================

int processar_tipo_pet(char *linha, int operacao, NoFilaTipoPet **fila) {
    comandoTipoPet cmd;
    cmd.operacao = operacao;
    cmd.dadosTipoDePet = NULL;
    cmd.idAlvo = -1;

    if (operacao == 1) { // INSERT
        cmd.dadosTipoDePet = (tipoPet *)malloc(sizeof(tipoPet));
        
        char *ptrValues = strstr(linha, "values");
        if(!ptrValues) return 0; // ERRO

        char *valStart = strchr(ptrValues, '(');
        if(!valStart) return 0; // ERRO

        char *val = strtok(valStart + 1, ",)\t\n\r;");
        int contador = 0;
        
        while(val) {
            char *limpo = trim(val);
            if(contador == 0) cmd.dadosTipoDePet->codigo = atoi_limpo(limpo);
            if(contador == 1) limpar_string(cmd.dadosTipoDePet->nome, limpo);
            val = strtok(NULL, ",)\t\n\r;");
            contador++;
        }
        // Validação simples: se não leu pelo menos 2 campos, deu erro
        if(contador < 2) return 0; 

    } else { // DELETE / UPDATE
        cmd.idAlvo = extrair_id_where(linha);
        if(cmd.idAlvo == -1) return 0; // ERRO

        if (operacao == 3) { 
             cmd.dadosTipoDePet = (tipoPet *)malloc(sizeof(tipoPet));
             cmd.dadosTipoDePet->codigo = cmd.idAlvo;
             char *ptrSet = strstr(linha, "set");
             if(ptrSet) {
                 char *ptrCampo = strstr(ptrSet, "nome"); // ou descricao
                 if(!ptrCampo) ptrCampo = strstr(ptrSet, "descricao");
                 
                 if(ptrCampo) {
                     char *ptrIgual = strchr(ptrCampo, '=');
                     if(ptrIgual) {
                         char temp[50]; int k=0; ptrIgual++;
                         while(isspace((unsigned char)*ptrIgual)) ptrIgual++;
                         while(*ptrIgual && *ptrIgual != ' ' && *ptrIgual != ';') {
                            temp[k++] = *ptrIgual++;
                         } 
                         temp[k] = '\0';
                         limpar_string(cmd.dadosTipoDePet->nome, temp);
                    }
                 }
             }
        }
    }
    AddFilaTipoPet(fila, cmd);
    return 1; // SUCESSO (Faltava isso no seu código)
}

int processar_pessoa(char *linha, int operacao, NoFilaPessoa **fila) {
    comandoPessoa cmd;
    cmd.operacao = operacao;
    cmd.dadosPessoa = NULL;
    cmd.idAlvo = -1;
    
    int contador = 0;

    if (operacao == 1) { // INSERT
        cmd.dadosPessoa = (pessoa *)malloc(sizeof(pessoa));
        strcpy(cmd.dadosPessoa->endereco, "N/A");
        strcpy(cmd.dadosPessoa->dataNascimento, "00/00/0000");

        char *ptrValues = strstr(linha, "values");
        if(!ptrValues) return 0;

        char *valStart = strchr(ptrValues, '(');
        if(!valStart) return 0;

        char *cursor_atual = valStart + 1; 
        char *val;
        
        while ((val = extrair_campo(&cursor_atual)) != NULL) {
            char limpo[300];
            limpar_string(limpo, val);
            switch(contador) {
                case 0: cmd.dadosPessoa->codigo = atoi_limpo(limpo); break;
                case 1: limpar_string(cmd.dadosPessoa->nome, limpo); break;
                case 2: cmd.dadosPessoa->fone = atoi_limpo(limpo); break;
                case 3: limpar_string(cmd.dadosPessoa->endereco, limpo); break;
                case 4: limpar_string(cmd.dadosPessoa->dataNascimento, limpo); break;
            }
            contador++;
        }

        if (contador < 2) {
            free(cmd.dadosPessoa); // Libera o malloc feito à toa
            return 0;
        }
    } else if (operacao == 4) { // SELECT
        cmd.dadosPessoa = (pessoa *)malloc(sizeof(pessoa));
        cmd.dadosPessoa->codigo = -1; 
        if (strstr(linha, "nome")) strcpy(cmd.dadosPessoa->nome, "nome");
        else strcpy(cmd.dadosPessoa->nome, "codigo");
    } else { // DELETE / UPDATE
        cmd.idAlvo = extrair_id_where(linha);
        if(cmd.idAlvo == -1) return 0;

        if (operacao == 3) {
            cmd.dadosPessoa = (pessoa *)malloc(sizeof(pessoa));
            cmd.dadosPessoa->codigo = cmd.idAlvo;
            char *ptrSet = strstr(linha, "set");
            if (ptrSet) {
                 char *ptrNome = strstr(ptrSet, "nome");
                 if(ptrNome) {
                     char *ptrIgual = strchr(ptrNome, '=');
                     if(ptrIgual) {
                         char temp[50]; int k=0; ptrIgual++;
                         while(isspace((unsigned char)*ptrIgual)) ptrIgual++;
                         while(*ptrIgual && *ptrIgual != ',' && strncasecmp(ptrIgual, "where", 5) != 0) 
                             temp[k++] = *ptrIgual++;
                         temp[k] = '\0';
                         limpar_string(cmd.dadosPessoa->nome, temp);
                     }
                 }
            }
        }
    }


    AddFilaPessoa(fila, cmd);
    return 1; // SUCESSO
}

int processar_pet(char *linha, int operacao, NoFilaPet **fila) {
    comandoPet cmd;
    cmd.operacao = operacao;
    cmd.dadosPet = NULL;
    cmd.idAlvo = -1;
    
    int contador = 0;

    if (operacao == 1) { // INSERT
        cmd.dadosPet = (pet *)malloc(sizeof(pet));
        
        char *ptrValues = strstr(linha, "values");
        if(!ptrValues) return 0;

        char *valStart = strchr(ptrValues, '(');
        if(!valStart) return 0;

        char *cursor_atual = valStart + 1; 
        char *val;
        while ( (val = extrair_campo(&cursor_atual)) != NULL ) {
            char limpo[300]; 
            limpar_string(limpo, val);
            switch(contador) {
                case 0: cmd.dadosPet->codigo = atoi_limpo(limpo); break;
                case 1: cmd.dadosPet->codigo_pes = atoi_limpo(limpo); break;
                case 2: limpar_string(cmd.dadosPet->nome, limpo); break;
                case 3: cmd.dadosPet->codigo_tipo = atoi_limpo(limpo); break;
            }
            contador++;
        }

        if (contador < 4) {
            free(cmd.dadosPet);
            return 0;
        }
    } else if (operacao == 4) { // SELECT

         cmd.dadosPet = (pet *)malloc(sizeof(pet));
         if (strstr(linha, "nome")) strcpy(cmd.dadosPet->nome, "nome");
         else strcpy(cmd.dadosPet->nome, "codigo");
    } else { // DELETE
        cmd.idAlvo = extrair_id_where(linha);
        if(cmd.idAlvo == -1) return 0;
    }


    AddFilaPet(fila, cmd);
    return 1; // SUCESSO
}

// =================================================================================
// FUNÇÃO PRINCIPAL
// =================================================================================

void DistribuirComandos(Fila *FilaBruta, NoFilaPessoa **FilaPessoa, NoFilaPet **FilaPet, NoFilaTipoPet **FilaTipo) {
    if (!FilaBruta || !FilaBruta->ini) return;

    while (FilaBruta->ini) {
        NoComando *atual = FilaBruta->ini;
        char linha[300];
        strcpy(linha, atual->info.linhacompleta);

        int id_comando = atual->info.id;

        int op = 0;
        if (comeca_com(linha, "insert")) op = 1;
        else if (comeca_com(linha, "delete")) op = 2;
        else if (comeca_com(linha, "update")) op = 3;
        else if (comeca_com(linha, "select")) op = 4;


        int sucesso = 1; // Assume sucesso

        if (op > 0) {
            if (strstr(linha, "tipo_pet")) {
                sucesso = processar_tipo_pet(linha, op, FilaTipo);
            } else if (strstr(linha, "pessoa")) {
                sucesso = processar_pessoa(linha, op, FilaPessoa);
            } else if (strstr(linha, "pet")) {
                sucesso = processar_pet(linha, op, FilaPet);
            }
        } else {
             // Linha não vazia que não é comando
            if(strlen(trim(linha)) > 0) sucesso = 0;
        }

        // --- SISTEMA DE ERRO ---
        if (sucesso == 0) {
            printf("\n[ERRO CRITICO] Erro de sintaxe no comando %d: %s\n", id_comando, linha);
            printf("Cancelando importacao...\n");
            
            // 1. Limpa o resto do arquivo da memória
            while(FilaBruta->ini) {
                NoComando *lixo = FilaBruta->ini;
                FilaBruta->ini = lixo->prox;
                free(lixo);
            }
            FilaBruta->fim = NULL;
            FilaBruta->tam = 0;

            // 2. Limpa o que já tinha sido aceito
            limpeza(FilaPessoa, FilaPet, FilaTipo);
            
            return; // Sai da função
        }

        // Avança a fila
        FilaBruta->ini = atual->prox;
        if (!FilaBruta->ini) FilaBruta->fim = NULL;
        FilaBruta->tam--;
        free(atual);
    }
}

void MostrarFilasEspecificas(NoFilaPessoa *FilaPessoa, NoFilaPet *FilaPet, NoFilaTipoPet *FilaTipo)
{
    printf("\n === VERIFICACAO DAS FILAS ESPECIFICAS === \n");

    printf("\n --- Fila de TIPOS DE PET --- \n");
    NoFilaTipoPet *AtualTipo = FilaTipo;
    while (AtualTipo) {
        printf("Operacao: %d |", AtualTipo->info.operacao);
        if (AtualTipo->info.operacao == 1 && AtualTipo->info.dadosTipoDePet)
            printf("Dados: ID %d, Descricao: %s\n", AtualTipo->info.dadosTipoDePet->codigo, AtualTipo->info.dadosTipoDePet->nome);
        else if (AtualTipo->info.operacao == 2)
            printf("Remover ID: %d\n", AtualTipo->info.idAlvo);
        else if (AtualTipo->info.operacao == 3)
            printf("Update ID: %d, Novo Nome: %s\n", AtualTipo->info.idAlvo, AtualTipo->info.dadosTipoDePet->nome);
        
        AtualTipo = AtualTipo->prox;
    }

    printf("\n--- Fila de PESSOAS ---\n");
    NoFilaPessoa *AtualPessoa = FilaPessoa;
    while (AtualPessoa) {
        printf("Operacao: %d | ", AtualPessoa->info.operacao);
        if (AtualPessoa->info.operacao == 1 && AtualPessoa->info.dadosPessoa)
            printf("Dados: ID %d, Nome: %s, Fone: %d \n", AtualPessoa->info.dadosPessoa->codigo, AtualPessoa->info.dadosPessoa->nome, AtualPessoa->info.dadosPessoa->fone);
        else if (AtualPessoa->info.operacao == 2)
            printf("Remover ID: %d\n", AtualPessoa->info.idAlvo);
        else if (AtualPessoa->info.operacao == 4)
            printf("Select Order By: %s\n", AtualPessoa->info.dadosPessoa->nome);
        
        AtualPessoa = AtualPessoa->prox;
    }

    printf("\n --- Fila de PETS ---\n");
    NoFilaPet *AtualPet = FilaPet;
    while (AtualPet) {
        printf("Operacao: %d |", AtualPet->info.operacao);
        if (AtualPet->info.operacao == 1 && AtualPet->info.dadosPet) {
            printf("Dados: id %d, Dono: %d, Nome: %s, Tipo: %d\n", 
                AtualPet->info.dadosPet->codigo, 
                AtualPet->info.dadosPet->codigo_pes, 
                AtualPet->info.dadosPet->nome, 
                AtualPet->info.dadosPet->codigo_tipo);
        }
        else if (AtualPet->info.operacao == 2) {
            printf("Remover ID: %d\n", AtualPet->info.idAlvo);
        }
        AtualPet = AtualPet->prox;
    }
    printf("================================\n");
}