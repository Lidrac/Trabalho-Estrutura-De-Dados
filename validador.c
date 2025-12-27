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

// Função pra remover aspas, por que a atoi não remove
int atoi_limpo(char *str) {
    char temp[50];
    limpar_string(temp, str); // Remove aspas antes de converter
    return atoi(temp);
}

// ==================================================================================
// FERRAMENTAS DE PARSING AVANÇADO (TOKENIZAÇÃO SEGURA)
// ==================================================================================

int separar_colunas_valores (char *linha, char *bufferColumns, char *bufferValues){
    char linhaCopy[300];
    strcpy(linhaCopy, linha);

    char linhaLower[300];
    for(int i = 0; linhaCopy[i]; i++){
        linhaLower[i] = tolower(linhaCopy[i]);
    }

    char *ptrValues = strstr(linhaLower, "values");
    if(!ptrValues) return 0;

    int offsetValues = ptrValues - linhaLower;

    // achar parênteses das colunas (antes do valores)
    char *EndColumns = linhaCopy + offsetValues;
    char *StartColumns = strchr(linhaCopy, '(');

    if (!StartColumns || StartColumns > EndColumns) return 0;

    char *FecharColumns = strchr(StartColumns, ')');
    if (!FecharColumns || FecharColumns > EndColumns) return 0;

    strncpy(bufferColumns, StartColumns + 1, FecharColumns - StartColumns - 1);
    bufferColumns[FecharColumns - StartColumns - 1] = '\0';

    // achar parênteses dos valores (depois do valores)
    char *ptrStartValues = linhaCopy + offsetValues + 6;
    char *StartValues = strchr(ptrStartValues, '(');
    if(!StartValues) return 0;

    char *FecharValues = strrchr(StartValues, ')');
    if(!FecharValues) return 0;

    strncpy(bufferValues, StartValues + 1, FecharValues - StartValues - 1);
    bufferValues[FecharValues - StartValues - 1] = '\0';

    return 1;

}

int separar_tokens_seguro (char *entrada, char **saida, int max){
    int quantidade = 0;
    int aspas = 0;
    char *inicio = entrada;

    for(int i=0; entrada[i]; i++){
        if(entrada[i] == '\'' || entrada [i] == '\"') {
            aspas = !aspas;
    }

        if(entrada[i] == ',' && !aspas){
            entrada[i] = '\0';
            saida[quantidade++] = trim(inicio);
            inicio = &entrada[i+1];
            if(quantidade >= max){
                return quantidade;
            }
        }
    }
    if(*inicio){
        saida[quantidade++] = trim(inicio);
    } 
    return quantidade;
}

// Busca valores em comandos UPDATE/DELETE/SELECT (onde temos chave=valor)
int buscar_valor(char *linha, char *nomeCampo, char *destino) {
    char linhaLower[300];
    char campoBusca[50];

    // Simplificacao: Busca substring case insensitive
    strcpy(linhaLower, linha);
    for(int i =0; linhaLower[i]; i++){
        linhaLower[i] = tolower(linha[i]);
    }

    strcpy(campoBusca, nomeCampo);
    for(int i =0; campoBusca[i]; i++){
        campoBusca[i] = tolower(campoBusca[i]);
    }

    char *ptrCampo = strstr(linhaLower, campoBusca);
    if(!ptrCampo) return 0;

    // Calcula offset para pegar no ponteiro original (que tem Case original)
    int offset = ptrCampo - linhaLower;
    char *ptrOriginal = linha + offset;

    char *ptrIgual = strchr(ptrOriginal, '=');
    if(!ptrIgual) return 0;

    char temp[300];
    int k = 0;
    ptrIgual++; // pula o =

    while (isspace(*ptrIgual)) ptrIgual++;// pula espaços

    int dentroAspas = 0;
    while(*ptrIgual) {
        if(*ptrIgual == '\''){
            dentroAspas = !dentroAspas;
        }

        if(!dentroAspas && (*ptrIgual == ',' || strncasecmp(ptrIgual, "where", 5) == 0 || *ptrIgual == ';')){
            break;
        }
        temp[k++] = *ptrIgual++;
    }
    temp[k] = '\0';

    limpar_string(destino, temp);
    return 1;
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
    strcpy(cmd.tipoFiltragem, "");
    strcpy(cmd.criterioOrdenacao, "");

    if (operacao == 1) { // INSERT DINÂMICO
        cmd.dadosTipoDePet = (tipoPet *)malloc(sizeof(tipoPet));
        cmd.dadosTipoDePet->codigo = 0;
        strcpy(cmd.dadosTipoDePet->nome, "");

        char strCols[300] = {0}, strVals[300] = {0};

        if (!separar_colunas_valores(linha, strCols, strVals)) {
            free(cmd.dadosTipoDePet); return 0; 
        }

        char *cols[10]; char *vals[10]; 
        int qtd = separar_tokens_seguro(strCols, cols, 10);
        int qtdV = separar_tokens_seguro(strVals, vals, 10);

        if (qtd != qtdV) { free(cmd.dadosTipoDePet); return 0; } 

        for (int i = 0; i < qtd; i++) {
            if (strcasecmp(cols[i], "codigo") == 0) 
                cmd.dadosTipoDePet->codigo = atoi_limpo(vals[i]);
            else if (strcasecmp(cols[i], "descricao") == 0 || strcasecmp(cols[i], "nome") == 0)
                limpar_string(cmd.dadosTipoDePet->nome, vals[i]);
        }

    } else { // UPDATE / DELETE / SELECT (Usa buscar_valor)
        cmd.dadosTipoDePet = (tipoPet *)malloc(sizeof(tipoPet));
        cmd.dadosTipoDePet->codigo = -1;
        strcpy(cmd.dadosTipoDePet->nome, "");

        char buffer[300];

        // Lógica do UPDATE
        if(operacao == 3) {
            char *ptrSet = strstr(linha, "set"); 
            if(!ptrSet) ptrSet = strstr(linha, "SET"); 

            if(ptrSet && (buscar_valor(ptrSet, "nome", buffer) || buscar_valor(ptrSet, "descricao", buffer))) {
                limpar_string(cmd.dadosTipoDePet->nome, buffer);
            }
        }

        // Lógica do WHERE
        char *ptrWhere = strstr(linha, "where");
        if(!ptrWhere) ptrWhere = strstr(linha, "WHERE");

        if(ptrWhere) {
            if(buscar_valor(ptrWhere, "codigo", buffer)) {
                cmd.dadosTipoDePet->codigo = atoi_limpo(buffer);
                strcpy(cmd.tipoFiltragem ,"codigo");
            } else if (buscar_valor(ptrWhere, "nome", buffer) || buscar_valor(ptrWhere, "descricao", buffer)) {
                strcpy(cmd.dadosTipoDePet->nome ,buffer);
                strcpy(cmd.tipoFiltragem ,"nome");
            }
        }

        // Lógica ORDER BY
        if(operacao == 4) {
            if(strstr(linha, "descricao") || strstr(linha, "nome")) strcpy(cmd.criterioOrdenacao, "nome");
            else if(strstr(linha, "codigo")) strcpy(cmd.criterioOrdenacao, "codigo");
        }
    }
    AddFilaTipoPet(fila, cmd);
    return 1;
}

int processar_pessoa(char *linha, int operacao, NoFilaPessoa **fila) {
    comandoPessoa cmd;
    cmd.operacao = operacao;
    cmd.dadosPessoa = NULL;
    strcpy(cmd.tipoFiltragem, "");
    strcpy(cmd.criterioOrdenacao, "");

    if (operacao == 1) { // INSERT DINÂMICO
        cmd.dadosPessoa = (pessoa *)malloc(sizeof(pessoa));
        // Defaults
        cmd.dadosPessoa->codigo = 0;
        strcpy(cmd.dadosPessoa->nome, "");
        strcpy(cmd.dadosPessoa->fone, "");
        strcpy(cmd.dadosPessoa->endereco, "");
        strcpy(cmd.dadosPessoa->dataNascimento, "");

        char strCols[300] = {0}, strVals[300] = {0};
        
        if (!separar_colunas_valores(linha, strCols, strVals)) {
            free(cmd.dadosPessoa); return 0;
        }

        char *cols[10]; char *vals[10];
        int qtd = separar_tokens_seguro(strCols, cols, 10);
        int qtdV = separar_tokens_seguro(strVals, vals, 10);

        if (qtd != qtdV) { free(cmd.dadosPessoa); return 0; }

        for (int i = 0; i < qtd; i++) {
            if (strcasecmp(cols[i], "codigo") == 0)
                cmd.dadosPessoa->codigo = atoi_limpo(vals[i]);
            else if (strcasecmp(cols[i], "nome") == 0)
                limpar_string(cmd.dadosPessoa->nome, vals[i]);
            else if (strcasecmp(cols[i], "fone") == 0 || strcasecmp(cols[i], "telefone") == 0)
                limpar_string(cmd.dadosPessoa->fone, vals[i]); 
            else if (strcasecmp(cols[i], "endereco") == 0)
                limpar_string(cmd.dadosPessoa->endereco, vals[i]);
            else if (strcasecmp(cols[i], "data_nascimento") == 0 || strcasecmp(cols[i], "data") == 0)
                limpar_string(cmd.dadosPessoa->dataNascimento, vals[i]);
        }

    } else { // SELECT UPDATE REMOVE 
        cmd.dadosPessoa = (pessoa *)malloc(sizeof(pessoa));
        cmd.dadosPessoa->codigo = -1;
        strcpy(cmd.dadosPessoa->fone, "");
        strcpy(cmd.dadosPessoa->nome, "");
        strcpy(cmd.dadosPessoa->dataNascimento, "");
        strcpy(cmd.dadosPessoa->endereco, "");

        char buffer[300];

        // SET
        if(operacao == 3) {
            char *ptrSet = strstr(linha, "set");
            if(!ptrSet) ptrSet = strstr(linha, "SET");
            if(ptrSet) {
                if(buscar_valor(ptrSet, "nome", buffer)) strcpy(cmd.dadosPessoa->nome, buffer);
                if(buscar_valor(ptrSet, "fone", buffer)) strcpy(cmd.dadosPessoa->fone, buffer);
                if(buscar_valor(ptrSet, "endereco", buffer)) strcpy(cmd.dadosPessoa->endereco, buffer);
                if(buscar_valor(ptrSet, "data_nascimento", buffer)) strcpy(cmd.dadosPessoa->dataNascimento, buffer);
            }
        }

        // WHERE
        char *ptrWhere = strstr(linha, "where");
        if(!ptrWhere) ptrWhere = strstr(linha, "WHERE");
        if(ptrWhere) {
            if(buscar_valor(ptrWhere, "codigo", buffer)) {
                cmd.dadosPessoa->codigo = atoi_limpo(buffer);
                strcpy(cmd.tipoFiltragem, "codigo");
            } else if(buscar_valor(ptrWhere, "nome", buffer)) {
                strcpy(cmd.dadosPessoa->nome, buffer);
                strcpy(cmd.tipoFiltragem, "nome");
            } else if(buscar_valor(ptrWhere, "fone", buffer)) {
                strcpy(cmd.dadosPessoa->fone, buffer);
                strcpy(cmd.tipoFiltragem, "fone");
            } else if(buscar_valor(ptrWhere, "endereco", buffer)) {
                strcpy(cmd.dadosPessoa->endereco, buffer);
                strcpy(cmd.tipoFiltragem, "endereco");
            }
        }

        // ORDER
        if(operacao == 4) {
             if(strstr(linha, "nome")) strcpy(cmd.criterioOrdenacao, "nome");
             else if(strstr(linha, "data")) strcpy(cmd.criterioOrdenacao, "data");
             else if(strstr(linha, "fone")) strcpy(cmd.criterioOrdenacao, "fone");
             else if(strstr(linha, "codigo")) strcpy(cmd.criterioOrdenacao, "codigo");
        }
    } 
    AddFilaPessoa(fila, cmd);
    return 1;
}

int processar_pet(char *linha, int operacao, NoFilaPet **fila) {
    comandoPet cmd;
    cmd.operacao = operacao;
    cmd.dadosPet = NULL;
    strcpy(cmd.tipoFiltragem, "");
    strcpy(cmd.criterioOrdenacao, "");

    if (operacao == 1) { // INSERT DINÂMICO
        cmd.dadosPet = (pet *)malloc(sizeof(pet));
        cmd.dadosPet->codigo = 0;
        cmd.dadosPet->codigo_pes = 0;
        cmd.dadosPet->codigo_tipo = 0;
        strcpy(cmd.dadosPet->nome, "");

        char strCols[300] = {0}, strVals[300] = {0};
        
        if (!separar_colunas_valores(linha, strCols, strVals)) {
            free(cmd.dadosPet); return 0;
        }

        char *cols[10]; char *vals[10];
        int qtd = separar_tokens_seguro(strCols, cols, 10);
        int qtdV = separar_tokens_seguro(strVals, vals, 10);

        if (qtd != qtdV) { free(cmd.dadosPet); return 0; }

        for (int i = 0; i < qtd; i++) {
            if (strcasecmp(cols[i], "codigo") == 0)
                cmd.dadosPet->codigo = atoi_limpo(vals[i]);
            else if (strcasecmp(cols[i], "codigo_cli") == 0 || strcasecmp(cols[i], "codigo_pes") == 0 || strcasecmp(cols[i], "dono") == 0)
                cmd.dadosPet->codigo_pes = atoi_limpo(vals[i]);
            else if (strcasecmp(cols[i], "nome") == 0)
                limpar_string(cmd.dadosPet->nome, vals[i]);
            else if (strcasecmp(cols[i], "codigo_tipo") == 0 || strcasecmp(cols[i], "tipo") == 0)
                cmd.dadosPet->codigo_tipo = atoi_limpo(vals[i]);
        }

    } else { // SELECT UPDATE REMOVER
        cmd.dadosPet = (pet *)malloc(sizeof(pet));
        cmd.dadosPet->codigo = -1;
        cmd.dadosPet->codigo_pes = -1;
        cmd.dadosPet->codigo_tipo = -1;
        strcpy(cmd.dadosPet->nome, "");

        char buffer[300];

        // SET
        if(operacao == 3) {
            char *ptrSet = strstr(linha, "set");
            if(!ptrSet) ptrSet = strstr(linha, "SET");
            if(ptrSet) {
                if(buscar_valor(ptrSet, "nome", buffer)) strcpy(cmd.dadosPet->nome, buffer);
                if(buscar_valor(ptrSet, "codigo_pes", buffer)) cmd.dadosPet->codigo_pes = atoi(buffer);
                if(buscar_valor(ptrSet, "codigo_tipo", buffer)) cmd.dadosPet->codigo_tipo = atoi(buffer);
            }
        }

        // WHERE
        char *ptrWhere = strstr(linha, "where");
        if(!ptrWhere) ptrWhere = strstr(linha, "WHERE");
        if(ptrWhere) {
            if(buscar_valor(ptrWhere, "codigo_pes", buffer)) {
                cmd.dadosPet->codigo_pes = atoi_limpo(buffer);
                strcpy(cmd.tipoFiltragem, "codigo_pes");
            } else if(buscar_valor(ptrWhere, "codigo_tipo", buffer)) {
                cmd.dadosPet->codigo_tipo = atoi_limpo(buffer);
                strcpy(cmd.tipoFiltragem, "codigo_tipo");
            } else if(buscar_valor(ptrWhere, "nome", buffer)) {
                strcpy(cmd.dadosPet->nome, buffer);
                strcpy(cmd.tipoFiltragem, "nome");
            } else if(buscar_valor(ptrWhere, "codigo", buffer)) {
                cmd.dadosPet->codigo = atoi_limpo(buffer);
                strcpy(cmd.tipoFiltragem, "codigo");
            }
        }

        // ORDER
        if(operacao == 4) {
            if(strstr(linha, "nome")) strcpy(cmd.criterioOrdenacao, "nome");
            else if(strstr(linha, "codigo_pes")) strcpy(cmd.criterioOrdenacao, "codigo_pes");
            else if(strstr(linha, "codigo_tipo")) strcpy(cmd.criterioOrdenacao, "codigo_tipo");
            else if(strstr(linha, "codigo")) strcpy(cmd.criterioOrdenacao, "codigo");
        }
    }

    AddFilaPet(fila, cmd);
    return 1; 
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
            
            // Limpa o resto do arquivo da memória
            while(FilaBruta->ini) {
                NoComando *lixo = FilaBruta->ini;
                FilaBruta->ini = lixo->prox;
                free(lixo);
            }
            FilaBruta->fim = NULL;
            FilaBruta->tam = 0;

            // Limpa o que já tinha sido aceito
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

