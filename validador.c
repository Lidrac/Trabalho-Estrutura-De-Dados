#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "validador.h"

#ifdef _WIN32
    #define strcasecmp _stricmp
    #define strncasecmp _strnicmp
#endif

// =================================================================================
// FERRAMENTAS AUXILIARES
// =================================================================================

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

char* extrair_campo(char **cursor) {
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

    if (operacao == 1) { // INSERT
        cmd.dadosTipoDePet = (tipoPet *)malloc(sizeof(tipoPet));
        
        char *ptrValues = strstr(linha, "values");
        
        // ERRO
        if(!ptrValues) {
            free(cmd.dadosTipoDePet);
            return 0; 
        } 

        char *valStart = strchr(ptrValues, '(');
        
        // ERRO
        if(!valStart) {
            free(cmd.dadosTipoDePet);
            return 0;
        }
        
        char *cursor_atual = valStart + 1;
        char *val;
        int contador = 0;
        
        while ((val = extrair_campo(&cursor_atual)) != NULL) {
            char limpo[300];
            limpar_string(limpo, val); // Remove aspas e espaços

            if(contador == 0) cmd.dadosTipoDePet->codigo = atoi_limpo(limpo);
            if(contador == 1) limpar_string(cmd.dadosTipoDePet->nome, limpo); // Copia o nome
            
            contador++;
        }

        // ERRO
        if(contador < 1) {
            free(cmd.dadosTipoDePet);
            return 0; 
        }
    }else if (operacao == 4 || operacao == 2) {

        cmd.dadosTipoDePet = (tipoPet *)malloc(sizeof(tipoPet));
        cmd.dadosTipoDePet->codigo = -1;
        strcpy(cmd.dadosTipoDePet->nome, "");

        char *ptrWhere = strstr(linha, "where");
        

        if(ptrWhere) {
            if(strstr(ptrWhere, "nome")) {
                char *ptrIgual = strchr(ptrWhere, '=');
                if(ptrIgual) {
                    char temp[50];
                    limpar_string(temp, ptrIgual + 1);

                    strcpy(cmd.dadosTipoDePet->nome, temp);
                    strcpy(cmd.tipoFiltragem, "nome");
                }
            } else if (strstr(ptrWhere, "codigo")) {
                char *ptrIgual = strchr(ptrWhere, '=');

                if(ptrIgual) {
                    char temp[50];

                    cmd.dadosTipoDePet->codigo = atoi(ptrIgual + 1);
                    strcpy(cmd.tipoFiltragem, "codigo");
                }
            }
        }

        if(operacao == 4) {
            char *ptrOrder = strstr(linha, "order");
            if(ptrOrder) {
                char *ptrBy = strstr(ptrOrder, "by");

                if(ptrBy) {
                    if(strstr(ptrBy, "nome")) {
                        strcpy(cmd.criterioOrdenacao, "nome");
                    }
                }
            }
        }

    } else if (operacao == 3) { // UPDATE

            cmd.dadosTipoDePet = (tipoPet *)malloc(sizeof(tipoPet));

            cmd.dadosTipoDePet->codigo = -1;
            strcpy(cmd.dadosTipoDePet->nome, "");

            char *ptrSet = strstr(linha, "set");

            if(!ptrSet) {
                free(cmd.dadosTipoDePet);
                return 0; 
            }

            if (ptrSet) {
                if(strstr(ptrSet, "nome")) {
                    char *ptrCampo = strstr(ptrSet, "nome");

                    char *ptrIgual = strchr(ptrCampo, '=');
                    if(ptrIgual) {
                        char temp[50]; 
                        int k=0; 
                        ptrIgual++;

                        while(isspace((unsigned char)*ptrIgual)) ptrIgual++;
                        while(*ptrIgual && *ptrIgual != ',' && strncasecmp(ptrIgual, "where", 5) != 0) 
                            temp[k++] = *ptrIgual++;
                        temp[k] = '\0';
                        limpar_string(cmd.dadosTipoDePet->nome, temp);
                    }
                }
            }

            char *ptrWhere = strstr(linha, "where");
            if(ptrWhere) {
                if(strstr(ptrWhere, "codigo")) {
                    char *ptrIgual = strchr(ptrWhere, '=');
                    if(ptrIgual) {
                        cmd.dadosTipoDePet->codigo = atoi_limpo(ptrIgual + 1);
                        strcpy(cmd.tipoFiltragem, "codigo");
                    }
                }

                if(strstr(ptrWhere, "nome")) {
                    char *ptrIgual = strchr(ptrWhere, '=');
                    if(ptrIgual) {
                        char temp[50]; 
                        int k=0; 
                        ptrIgual++;

                        while(isspace((unsigned char)*ptrIgual)) ptrIgual++;
                        while(*ptrIgual && *ptrIgual != ',' && strncasecmp(ptrIgual, "where", 5) != 0) 
                            temp[k++] = *ptrIgual++;
                        temp[k] = '\0';
                        limpar_string(cmd.dadosTipoDePet->nome, temp);

                        strcpy(cmd.tipoFiltragem, "nome");
                    }
                }
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
    
    int contador = 0;

    if (operacao == 1) { // INSERT
        cmd.dadosPessoa = (pessoa *)malloc(sizeof(pessoa));
        strcpy(cmd.dadosPessoa->endereco, "N/A");
        strcpy(cmd.dadosPessoa->dataNascimento, "00/00/0000");

        char *ptrValues = strstr(linha, "values");
        
        if(!ptrValues) {
            free(cmd.dadosPessoa);
            return 0; 
        } 

        char *valStart = strchr(ptrValues, '(');
        
        if(!ptrValues) {
            free(cmd.dadosPessoa);
            return 0; 
        } 

        char *cursor_atual = valStart + 1;
        char *val;
        int contador = 0;
        
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
            free(cmd.dadosPessoa);
            return 0;
        }
    } else if (operacao == 4 || operacao == 2) { // SELECT

        cmd.dadosPessoa = (pessoa *)malloc(sizeof(pessoa));
        cmd.dadosPessoa->codigo = -1;
        cmd.dadosPessoa->fone = -1;
        strcpy(cmd.dadosPessoa->nome, "");
        strcpy(cmd.dadosPessoa->dataNascimento, "");
        strcpy(cmd.dadosPessoa->endereco, "");

        char *ptrWhere = strstr(linha, "where");

        if(ptrWhere) {

            if(strstr(ptrWhere, "nome")) {
                char *ptrIgual = strchr(ptrWhere, '=');
                if(ptrIgual) {
                    char temp[50];
                    limpar_string(temp, ptrIgual + 1);

                    strcpy(cmd.dadosPessoa->nome, temp);
                    strcpy(cmd.tipoFiltragem, "nome");
                }
            } else if (strstr(ptrWhere, "codigo")) {
                char *ptrIgual = strchr(ptrWhere, '=');

                if(ptrIgual) {
                    char temp[50];

                    cmd.dadosPessoa->codigo = atoi(ptrIgual + 1);
                    strcpy(cmd.tipoFiltragem, "codigo");
                }
            }else if (strstr(ptrWhere, "fone")) {
                char *ptrIgual = strchr(ptrWhere, '=');

                if(ptrIgual) {
                    char temp[50];

                    cmd.dadosPessoa->fone = atoi(ptrIgual + 1);
                    strcpy(cmd.tipoFiltragem, "fone");
                }
            } else if (strstr(ptrWhere, "data_nascimento")) {
                char *ptrIgual = strchr(ptrWhere, '=');

                if(ptrIgual) {
                    char temp[50];
                    limpar_string(temp, ptrIgual + 1);
                    strcpy(cmd.dadosPessoa->dataNascimento, temp);

                    strcpy(cmd.tipoFiltragem, "data");
                }
            } else if (strstr(ptrWhere, "endereco")) {
                char *ptrIgual = strchr(ptrWhere, '=');

                if(ptrIgual) {
                    char temp[50];
                    limpar_string(temp, ptrIgual + 1);
                    strcpy(cmd.dadosPessoa->endereco, temp);

                    strcpy(cmd.tipoFiltragem, "endereco");
                }
            }
        }

        if(operacao == 4) {
            char *ptrOrder = strstr(linha, "order");
            if(ptrOrder) {
                char *ptrBy = strstr(ptrOrder, "by");
    
    
                if(ptrBy) {
                    if(strstr(ptrBy, "nome")) {
                        strcpy(cmd.criterioOrdenacao, "nome");
                    } else if (strstr(ptrBy, "data_nascimento")) {
                        strcpy(cmd.criterioOrdenacao, "data");
                    } else if(strstr(ptrBy, "fone")) {
                        strcpy(cmd.criterioOrdenacao, "fone");
                    } else if(strstr(ptrBy, "endereco")) {
                        strcpy(cmd.criterioOrdenacao, "endereco");
                    }
                }
            }
        }

    } else if (operacao == 3) { // UPDATE

            cmd.dadosPessoa = (pessoa *)malloc(sizeof(pessoa));

            cmd.dadosPessoa->codigo = -1;
            cmd.dadosPessoa->fone = -1;
            strcpy(cmd.dadosPessoa->nome, "");
            strcpy(cmd.dadosPessoa->endereco, "");
            strcpy(cmd.dadosPessoa->dataNascimento, "");

            char *ptrSet = strstr(linha, "set");

            if(!ptrSet) {
                free(cmd.dadosPessoa);
                return 0; 
            } 

            if (ptrSet) {
                if(strstr(ptrSet, "nome")) {
                    char *ptrCampo = strstr(ptrSet, "nome");

                    char *ptrIgual = strchr(ptrCampo, '=');
                    if(ptrIgual) {
                        char temp[50]; 
                        int k=0; 
                        ptrIgual++;

                        while(isspace((unsigned char)*ptrIgual)) ptrIgual++;
                        while(*ptrIgual && *ptrIgual != ',' && strncasecmp(ptrIgual, "where", 5) != 0) 
                            temp[k++] = *ptrIgual++;
                        temp[k] = '\0';
                        limpar_string(cmd.dadosPessoa->nome, temp);

                    }
                }
                
                if (strstr(ptrSet, "data_nascimento")) {
                    char *ptrCampo = strstr(ptrSet, "data_nascimento");

                    char *ptrIgual = strchr(ptrCampo, '=');
                    if(ptrIgual) {
                        char temp[50]; 
                        int k=0; 
                        ptrIgual++;

                        while(isspace((unsigned char)*ptrIgual)) ptrIgual++;
                        while(*ptrIgual && *ptrIgual != ',' && strncasecmp(ptrIgual, "where", 5) != 0) 
                            temp[k++] = *ptrIgual++;
                        temp[k] = '\0';
                        limpar_string(cmd.dadosPessoa->dataNascimento, temp);
                    }
                }
                
                if(strstr(ptrSet, "endereco")) {
                    char *ptrCampo = strstr(ptrSet, "endereco");

                    char *ptrIgual = strchr(ptrCampo, '=');
                    if(ptrIgual) {
                        char temp[50]; 
                        int k=0; 
                        ptrIgual++;

                        while(isspace((unsigned char)*ptrIgual)) ptrIgual++;
                        while(*ptrIgual && *ptrIgual != ',' && strncasecmp(ptrIgual, "where", 5) != 0) 
                            temp[k++] = *ptrIgual++;
                        temp[k] = '\0';
                        limpar_string(cmd.dadosPessoa->endereco, temp);
                    }
                }

                if(strstr(ptrSet, "fone")) {
                    char *ptrCampo = strstr(ptrSet, "fone");

                    char *ptrIgual = strchr(ptrCampo, '=');
                    if(ptrIgual) {
                        char temp[50]; 
                        int k=0; 
                        ptrIgual++;

                        while(isspace((unsigned char)*ptrIgual)) ptrIgual++;
                        while(*ptrIgual && *ptrIgual != ',' && strncasecmp(ptrIgual, "where", 5) != 0) 
                            temp[k++] = *ptrIgual++;
                        temp[k] = '\0';
                        cmd.dadosPessoa->fone = atoi_limpo(temp);
                    }
                }
            }

        char *ptrWhere = strstr(linha, "where");
            if(ptrWhere) {
                if(strstr(ptrWhere, "codigo")) {
                    char *ptrIgual = strchr(ptrWhere, '=');
                    if(ptrIgual) {
                        cmd.dadosPessoa->codigo = atoi_limpo(ptrIgual + 1);
                        strcpy(cmd.tipoFiltragem, "codigo");
                    }
                }

                if(strstr(ptrWhere, "nome")) {
                    char *ptrIgual = strchr(ptrWhere, '=');
                    if(ptrIgual) {
                        char temp[50]; 
                        int k=0; 
                        ptrIgual++;

                        while(isspace((unsigned char)*ptrIgual)) ptrIgual++;
                        while(*ptrIgual && *ptrIgual != ',' && strncasecmp(ptrIgual, "where", 5) != 0) 
                            temp[k++] = *ptrIgual++;
                        temp[k] = '\0';
                        limpar_string(cmd.dadosPessoa->nome, temp);

                        strcpy(cmd.tipoFiltragem, "nome");
                    }
                }

                if(strstr(ptrWhere, "data_nascimento")) {
                    char *ptrIgual = strchr(ptrWhere, '=');
                    if(ptrIgual) {
                        char temp[50]; 
                        int k=0; 
                        ptrIgual++;

                        while(isspace((unsigned char)*ptrIgual)) ptrIgual++;
                        while(*ptrIgual && *ptrIgual != ',' && strncasecmp(ptrIgual, "where", 5) != 0) 
                            temp[k++] = *ptrIgual++;
                        temp[k] = '\0';
                        limpar_string(cmd.dadosPessoa->dataNascimento, temp);

                        strcpy(cmd.tipoFiltragem, "data");
                    }
                }


                if(strstr(ptrWhere, "endereco")) {
                    char *ptrIgual = strchr(ptrWhere, '=');
                    if(ptrIgual) {
                        char temp[50]; 
                        int k=0; 
                        ptrIgual++;

                        while(isspace((unsigned char)*ptrIgual)) ptrIgual++;
                        while(*ptrIgual && *ptrIgual != ',' && strncasecmp(ptrIgual, "where", 5) != 0) 
                            temp[k++] = *ptrIgual++;
                        temp[k] = '\0';
                        limpar_string(cmd.dadosPessoa->endereco, temp);

                        strcpy(cmd.tipoFiltragem, "endereco");
                    }
                }

                if(strstr(ptrWhere, "fone")) {
                    char *ptrIgual = strchr(ptrWhere, '=');
                    if(ptrIgual) {
                        cmd.dadosPessoa->fone = atoi_limpo(ptrIgual + 1);

                        strcpy(cmd.tipoFiltragem, "fone");
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

    strcpy(cmd.tipoFiltragem, "");
    strcpy(cmd.criterioOrdenacao, "");
    
    int contador = 0;

    if (operacao == 1) { // INSERT
        cmd.dadosPet = (pet *)malloc(sizeof(pet));
        
        char *ptrValues = strstr(linha, "values");
        if(!ptrValues) {
            free(cmd.dadosPet);
            return 0; 
        } 

        char *valStart = strchr(ptrValues, '(');
        if(!ptrValues) {
            free(cmd.dadosPet);
            return 0; 
        } 

        char *cursor_atual = valStart + 1;
        char *val;
        int contador = 0;
        
        while ((val = extrair_campo(&cursor_atual)) != NULL) {
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
    } else if (operacao == 4 || operacao == 2) { // SELECT

        cmd.dadosPet = (pet *)malloc(sizeof(pet));
        cmd.dadosPet->codigo = -1;
        cmd.dadosPet->codigo_pes = -1;
        cmd.dadosPet->codigo_tipo = -1;
        strcpy(cmd.dadosPet->nome, "");

        char *ptrWhere = strstr(linha, "where");

        if(ptrWhere) {

            if(strstr(ptrWhere, "nome")) {
                char *ptrIgual = strchr(ptrWhere, '=');
                if(ptrIgual) {
                    char temp[50];
                    limpar_string(temp, ptrIgual + 1);

                    strcpy(cmd.dadosPet->nome, temp);
                    strcpy(cmd.tipoFiltragem, "nome");
                }
            } else if (strstr(ptrWhere, "codigo")) {
                char *ptrIgual = strchr(ptrWhere, '=');

                if(ptrIgual) {
                    char temp[50];

                    cmd.dadosPet->codigo = atoi(ptrIgual + 1);
                    strcpy(cmd.tipoFiltragem, "codigo");
                }
            } else if (strstr(ptrWhere, "codigo_pes")) {
                char *ptrIgual = strchr(ptrWhere, '=');

                if(ptrIgual) {
                    cmd.dadosPet->codigo_pes = atoi(ptrIgual + 1);

                    strcpy(cmd.tipoFiltragem, "codigo_pes");
                }
            } else if (strstr(ptrWhere, "codigo_tipo")) {
                char *ptrIgual = strchr(ptrWhere, '=');

                if(ptrIgual) {
                    cmd.dadosPet->codigo_tipo = atoi(ptrIgual + 1);

                    strcpy(cmd.tipoFiltragem, "codigo_tipo");
                }
            }
        }

        if(operacao == 4) {
            char *ptrOrder = strstr(linha, "order");
            if(ptrOrder) {
                char *ptrBy = strstr(ptrOrder, "by");
    
    
                if(ptrBy) {
                    if(strstr(ptrBy, "nome")) {
                        strcpy(cmd.criterioOrdenacao, "nome");
                    } else if (strstr(ptrBy, "data_nascimento")) {
                        strcpy(cmd.criterioOrdenacao, "data");
                    } else if(strstr(ptrBy, "fone")) {
                        strcpy(cmd.criterioOrdenacao, "fone");
                    } else if(strstr(ptrBy, "endereco")) {
                        strcpy(cmd.criterioOrdenacao, "endereco");
                    }
                }
            }
        }

    } else if (operacao == 3) { // UPDATE

            cmd.dadosPet = (pet *)malloc(sizeof(pet));

            cmd.dadosPet->codigo = -1;
            cmd.dadosPet->codigo_pes = -1;
            cmd.dadosPet->codigo_tipo = -1;
            strcpy(cmd.dadosPet->nome, "");

            char *ptrSet = strstr(linha, "set");

            if(!ptrSet) {
                free(cmd.dadosPet);
                return 0; 
            }

            if (ptrSet) {
                if(strstr(ptrSet, "nome")) {
                    char *ptrCampo = strstr(ptrSet, "nome");

                    char *ptrIgual = strchr(ptrCampo, '=');
                    if(ptrIgual) {
                        char temp[50]; 
                        int k=0; 
                        ptrIgual++;

                        while(isspace((unsigned char)*ptrIgual)) ptrIgual++;
                        while(*ptrIgual && *ptrIgual != ',' && strncasecmp(ptrIgual, "where", 5) != 0) 
                            temp[k++] = *ptrIgual++;
                        temp[k] = '\0';
                        limpar_string(cmd.dadosPet->nome, temp);
                    }
                }
                
                if (strstr(ptrSet, "codigo_pes")) {
                    char *ptrCampo = strstr(ptrSet, "codigo_pes");

                    char *ptrIgual = strchr(ptrCampo, '=');
                    if(ptrIgual) {
                        char temp[50]; 
                        int k=0; 
                        ptrIgual++;

                        while(isspace((unsigned char)*ptrIgual)) ptrIgual++;
                        while(*ptrIgual && *ptrIgual != ',' && strncasecmp(ptrIgual, "where", 5) != 0) 
                            temp[k++] = *ptrIgual++;
                        temp[k] = '\0';

                        cmd.dadosPet->codigo_pes = atoi_limpo(temp);
                    }
                }
                
                if(strstr(ptrSet, "codigo_tipo")) {
                    char *ptrCampo = strstr(ptrSet, "codigo_tipo");

                    char *ptrIgual = strchr(ptrCampo, '=');
                    if(ptrIgual) {
                        char temp[50]; 
                        int k=0; 
                        ptrIgual++;

                        while(isspace((unsigned char)*ptrIgual)) ptrIgual++;
                        while(*ptrIgual && *ptrIgual != ',' && strncasecmp(ptrIgual, "where", 5) != 0) 
                            temp[k++] = *ptrIgual++;
                        temp[k] = '\0';
                        
                        cmd.dadosPet->codigo_tipo = atoi_limpo(temp);
                    }
                }
            }

            char *ptrWhere = strstr(linha, "where");
            if(ptrWhere) {
                if(strstr(ptrWhere, "codigo")) {
                    char *ptrIgual = strchr(ptrWhere, '=');
                    if(ptrIgual) {
                        cmd.dadosPet->codigo = atoi_limpo(ptrIgual + 1);
                        strcpy(cmd.tipoFiltragem, "codigo");
                    }
                }

                if(strstr(ptrWhere, "nome")) {
                    char *ptrIgual = strchr(ptrWhere, '=');
                    if(ptrIgual) {
                        char temp[50]; 
                        int k=0; 
                        ptrIgual++;

                        while(isspace((unsigned char)*ptrIgual)) ptrIgual++;
                        while(*ptrIgual && *ptrIgual != ',' && strncasecmp(ptrIgual, "where", 5) != 0) 
                            temp[k++] = *ptrIgual++;
                        temp[k] = '\0';
                        limpar_string(cmd.dadosPet->nome, temp);

                        strcpy(cmd.tipoFiltragem, "nome");
                    }
                }

                if(strstr(ptrWhere, "codigo_pes")) {
                    char *ptrIgual = strchr(ptrWhere, '=');
                    if(ptrIgual) {
                        cmd.dadosPet->codigo_pes = atoi_limpo(ptrIgual + 1);

                        strcpy(cmd.tipoFiltragem, "codigo_pes");
                    }
                }


                if(strstr(ptrWhere, "codigo_tipo")) {
                    char *ptrIgual = strchr(ptrWhere, '=');
                    if(ptrIgual) {
                        cmd.dadosPet->codigo_tipo = atoi_limpo(ptrIgual + 1);

                        strcpy(cmd.tipoFiltragem, "codigo_tipo");
                    }
                }
            }
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

