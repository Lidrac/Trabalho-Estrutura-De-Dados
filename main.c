#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pessoa.h"
#include "pet.h"
#include "tipoPet.h"
#include "comando.h"
#include "validador.h"

// FUNÇÕES PARA PESSOAS

void inserirPessoa(NoPessoa **iniPes, pessoa *p) {
    // Para verificar se já existe alguém com esse código
    if(buscarPessoaPorCodigo(iniPes, (*p).codigo)) {
        printf("\n Já existe uma pessoa com esse código, não é possível inserir \n");
        return;
    }

    NoPessoa *elementoInserido = (NoPessoa *) malloc(sizeof(NoPessoa));

    if(elementoInserido == NULL) {
        return;
    }

    NoPessoa *aux = *iniPes;

    while(aux && (*aux).prox != NULL) {
        aux = (*aux).prox;
    }

    if(*iniPes == NULL) {
        (*elementoInserido).ant = NULL;
        *iniPes = elementoInserido;
    } else {
        (*aux).prox = elementoInserido;
        (*elementoInserido).ant = aux;
    }
    (*elementoInserido).prox = NULL;
    (*elementoInserido).p = p;


    // Alteração no arquivo
    p->ativo = 1;

    // ab para escrever sempre no final do arquivo
    FILE *arquivo = fopen("ArquivosBinarios/pessoas.bin", "ab");

    if(arquivo) {
        p->posicaoNoArquivo = ftell(arquivo);

        fwrite(p, sizeof(pessoa), 1, arquivo);

        fclose(arquivo);
    }
}

void removerPessoa(NoPessoa **iniPes, NoPet **iniPet ,int codigo) {
    if(*iniPes == NULL) {
        printf("\n A lista está vazia \n");
        return;
    }
    NoPessoa *pessoaAlvo = buscarPessoaPorCodigo(iniPes, codigo);

    if(pessoaAlvo == NULL) {
        return;
    }

    NoPet *auxPet = *iniPet;

    // Para verificar se essa Pessoa tem algm pet, caso ela tenha a  gente remove os pets
    while(auxPet) {
        NoPet *proximoPet = (*auxPet).prox;
        if((*auxPet).p->codigo_pes == codigo) {
            removerPet(iniPet, (*auxPet).p->codigo);
        }
        auxPet = proximoPet;
    }

    // Aqui acontecerá a remoção da pessoa

    // Nesse caso é a  primeira pessoa da lista
    if((*pessoaAlvo).ant == NULL) {
        *iniPes = (*pessoaAlvo).prox;
        // Verificar se a lista tem apenas um elemento, pra NULL não ser acessado 
        if((*pessoaAlvo).prox){
            pessoaAlvo->prox->ant = NULL;
        }
    } else {
        pessoaAlvo->ant->prox = pessoaAlvo->prox;

        // caso ele esteja no meio
        if((*pessoaAlvo).prox) {
            pessoaAlvo->prox->ant = pessoaAlvo->ant;
        }
    }

    // A condição acima pode ser feita com o bloco de if para fora do if else

    // Alteração do Arquivo

    // Mudar o campo ativo no arquivo
    pessoaAlvo->p->ativo = 0;

    FILE *arquivo = fopen("ArquivosBinarios/pessoas.bin", "r+b");

    if(arquivo) {
        fseek(arquivo, pessoaAlvo->p->posicaoNoArquivo, SEEK_SET);

        // Sobescreve oo registro do arquivo antigo
        fwrite(pessoaAlvo->p, sizeof(pessoa), 1, arquivo);
        fclose(arquivo);
    }

    free((*pessoaAlvo).p);
    free(pessoaAlvo);

}

NoPessoa *buscarPessoaPorCodigo(NoPessoa **iniPes, int codigo) {
    NoPessoa *aux = *iniPes;

    while(aux != NULL) {
        if( aux->p->ativo == 1 && (*aux).p->codigo == codigo) {
            return aux;
        }

        aux = (*aux).prox;
    }

    return NULL;
}

void alterarPessoa(NoPessoa **iniPes, pessoa *novosDados) {
    NoPessoa *alvoPessoa = buscarPessoaPorCodigo(iniPes, (*novosDados).codigo);

    if(!alvoPessoa) {
        printf("\n Pessoa não encontrada !!!\n");
        return;
    }

    strcpy(alvoPessoa->p->nome, novosDados->nome);
    strcpy(alvoPessoa->p->endereco, novosDados->endereco);
    strcpy(alvoPessoa->p->dataNascimento, novosDados->dataNascimento);
    alvoPessoa->p->fone = novosDados->fone;

    // Alteração do arquivo
    FILE *arquivo = fopen("ArquivosBinarios/pessoas.bin", "r+b");

    if(arquivo) {
        fseek(arquivo, alvoPessoa->p->posicaoNoArquivo, SEEK_SET);

        // Sobescreve o arquivo com os dados já alterados
        fwrite(alvoPessoa->p, sizeof(pessoa), 1, arquivo);

        fclose(arquivo);
    }
    
}

void finalizarListaDePessoas(NoPessoa **iniPes) {
    if(*iniPes == NULL) {
        printf("\nA lista está vazia\n");
    }

    NoPessoa *aux = *iniPes;
    NoPessoa *ant = NULL;

    while(aux) {
        ant = aux;
        aux = (*aux).prox;
        free((*ant).p);
        free(ant);
    }
}

void pegarPessoasArquivo(NoPessoa **iniPes, char *nomeArquivo) {
    // Abre o arquivo para leitura
    FILE *arquivo = fopen(nomeArquivo, "r+b");
    
    if(!arquivo) {
        return;
    }

    
    // Garantir que o ponteiro está no início do arquivo
    fseek(arquivo, 0, SEEK_SET);
    
    // Para armazenar os valores intermediários
    pessoa temp;

    long posicaoAtual = ftell(arquivo);
    
    *iniPes = NULL;

    // Se for diferente de 1 , é porque ele chegou ao final do arquivo 
    while(fread(&temp, sizeof(pessoa), 1, arquivo) == 1) {
        
        if(temp.ativo == 1) {
            NoPessoa *novo = (NoPessoa *)malloc(sizeof(NoPessoa));
            pessoa *p = (pessoa*)malloc(sizeof(pessoa));
            *p = temp;
            p->posicaoNoArquivo = posicaoAtual;

            novo->p = p;
            novo->prox = NULL;
            novo->ant = NULL;
            
            if(*iniPes == NULL) {
                *iniPes = novo;
            } else {
                NoPessoa *aux = *iniPes;
                while(aux->prox) {
                    aux = aux->prox;
                }
                aux->prox = novo;
                novo->ant = aux;
            }
        }
        posicaoAtual = ftell(arquivo);
        
    }
    
    fclose(arquivo);
}

// Funciona como um compactador para eliminar os ativos = 0
void salvarPessoasNoArquivo(NoPessoa **iniPes, char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "wb");
    
    // Não precisa de if, por que caso o arquivo não exista ele é criado
    NoPessoa *aux = *iniPes;
    
    // Garantir que o ponteiro está no inicio do arquivo
    fseek(arquivo, 0, SEEK_SET);
    
    while(aux != NULL) {
        if(aux->p->ativo == 1) {
            fwrite((*aux).p, sizeof(pessoa), 1, arquivo);
        }
        aux = (*aux).prox;
    }
    
    fclose(arquivo);
}

void mostrarPessoas(NoPessoa **iniPes) {
    NoPessoa *aux = *iniPes;
    printf("\n-------------------------\n");
    while(aux) {
        printf("Código: %d\n", (*aux).p->codigo);
        printf("nome: %s\n", (*aux).p->nome);
        printf("fone: %d\n", (*aux).p->fone);
        printf("endereco: %s\n", (*aux).p->endereco);
        printf("data de nascimento: %s\n", (*aux).p->dataNascimento);

        aux = (*aux).prox;
    }
    printf("\n-------------------------\n");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// FUNÇÕES PARA PET

void inserirPet(NoPet **iniPet,NoPessoa **iniPes, NoTipoDePet **iniTipoDePet, pet *p) {
    // Verificar se já tem algum pet com esse código
    if(buscarPetPorCodigo(iniPet, (*p).codigo)) {
        printf("\nJá existe um pet com esse código, não é possível inserir\n");
        return;
    };


    // Verificar se ele tem dono e se o tipo de pet já está cadastrado
    if(!buscarPessoaPorCodigo(iniPes, p->codigo_pes) || !buscarTipoDePetPorCodigo(iniTipoDePet, p->codigo_tipo)) {
        printf("\n Não é possível inserir o Pet, ou o Dono ou o seu tipo não estão no sistema");
        return;
    };

    // Caso esteja tudo tranquilo no sistema, ele insere o pet
    NoPet *elementoInserido = (NoPet *) malloc(sizeof(NoPet));


    if(elementoInserido == NULL) {
        return;
    }

    NoPet *aux = *iniPet;

    while(aux && (*aux).prox != NULL) {
        aux = (*aux).prox;
    }
    
    if(*iniPet == NULL) {
        (*elementoInserido).ant = NULL;
        *iniPet = elementoInserido;
    } else {
        (*aux).prox = elementoInserido;
        (*elementoInserido).ant = aux;
    }
    (*elementoInserido).prox = NULL;
    (*elementoInserido).p = p;

    // Alteração no arquivo

    p->ativo = 1;

    // ab para escrever sempre no final do arquivo
    FILE *arquivo = fopen("ArquivosBinarios/pet.bin", "ab");

    if(arquivo) {
        p->posicaoNoArquivo = ftell(arquivo);

        fwrite(p, sizeof(pet), 1 , arquivo);

        fclose(arquivo);
    }
}

void removerPet(NoPet **iniPet, int codigo) {
    if(*iniPet == NULL) {
        printf("\nA lista está vazia!!!\n");
        return;
    }
    NoPet *petAlvo = buscarPetPorCodigo(iniPet, codigo);

    if(petAlvo == NULL) {
        return;
    }

    // Caso esteja no começo da lista
    if((*petAlvo).ant == NULL){
        *iniPet = (*petAlvo).prox;

        // Caso não seja apenas um elemento
        if((*petAlvo).prox) {
            petAlvo->prox->ant = NULL;
        }
    }else {
        petAlvo->ant->prox = petAlvo->prox;

        // Caso não seja o ultimo elemento
        if((*petAlvo).prox) {
            petAlvo->prox->ant = petAlvo->ant;
        }
    }

    // A condição acima pode ser feita com o bloco if fora do if else

    // Alteração do arquivo

    // Mudar o campo de ativo
    petAlvo->p->ativo = 0;

    FILE *arquivo = fopen("ArquivosBinarios/pet.bin", "r+b");

    if(arquivo) {
        fseek(arquivo, petAlvo->p->posicaoNoArquivo, SEEK_SET);

        // Sobescreve os dados, para ativo = 0;
        fwrite(petAlvo->p, sizeof(pet), 1, arquivo);
        fclose(arquivo);
    }

    free((*petAlvo).p);
    free(petAlvo);

}

NoPet *buscarPetPorCodigo(NoPet **iniPet, int codigo) {
    NoPet *aux = *iniPet;

    while(aux != NULL) {
        if( aux->p->ativo == 1 && (*aux).p->codigo == codigo) {
            return aux;
        }
        aux = (*aux).prox;
    }

    return NULL;
}

void alterarPet(NoPet **iniPet, NoPessoa **iniPes, NoTipoDePet **iniTipoDePet, pet *novosDados) {
    NoPet *alvoPet = buscarPetPorCodigo(iniPet, (*novosDados).codigo);

    if(alvoPet == NULL) {
        printf("\n Não foi possível encontrar o pet \n");
        return;
    }

    // Se o código do dono mudou, verificar se o novo dono existe
    if(alvoPet->p->codigo_pes != novosDados->codigo_pes) {
        // caso o código da pessoa não exista
        if(!buscarPessoaPorCodigo(iniPes, novosDados->codigo_pes)) {
            printf("\nO novo dono não está cadastrado!!!\n");
            return;
        }
    }

    // Se o código do tipo mudou, verificar se o novo tipo existe
    if(alvoPet->p->codigo_tipo != novosDados->codigo_tipo) {
        // Caso o código do tipo não exista
        if(!buscarTipoDePetPorCodigo(iniTipoDePet, novosDados->codigo_tipo)) {
            printf("\nO tipo do pet não está cadastrado!!!\n");
            return;
        }
    }

    alvoPet->p->codigo_pes = novosDados->codigo_pes;
    alvoPet->p->codigo_tipo = novosDados->codigo_tipo;
    strcpy(alvoPet->p->nome, novosDados->nome);

    // Alteração no arquivo

    FILE *arquivo = fopen("ArquivosBinarios/pet.bin", "r+b");

    if(arquivo) {
        fseek(arquivo, alvoPet->p->posicaoNoArquivo, SEEK_SET);

        // Sobescreve o arquivo com os dados já alterados
        fwrite(alvoPet->p, sizeof(pet), 1, arquivo);

        fclose(arquivo);
    }

}

void finalizarListaDePets(NoPet **iniPet) {
    if(*iniPet == NULL) {
        printf("\nA lista está vazia\n");
    }

    NoPet *aux = *iniPet;
    NoPet *ant = NULL;

    while(aux) {
        ant = aux;
        aux = (*aux).prox;
        free((*ant).p);
        free(ant);
    }
}

void pegarPetsArquivo(NoPet **iniPet, NoPessoa **iniPes, NoTipoDePet **iniTipoDePet, char *nomeArquivo) {
    // Abre o arquivo para leitura
    FILE *arquivo = fopen(nomeArquivo, "rb");
    
    if(!arquivo) {
        return;
    }
    
    // Garantir que o ponteiro está no início do arquivo
    fseek(arquivo, 0, SEEK_SET);
    
    // Para armazenar os valores intermediários
    pet temp;

    long posicaoAtual = ftell(arquivo);

    *iniPet = NULL;
    
    // Se for diferente de 1 , é porque ele chegou ao final do arquivo 
    while(fread(&temp, sizeof(pet), 1, arquivo) == 1) {      
        if(temp.ativo == 1) {
            NoPet *novo = (NoPet *)malloc(sizeof(NoPet));
            pet *p = (pet*)malloc(sizeof(pet));
            *p = temp;
            p->posicaoNoArquivo = posicaoAtual;

            novo->p = p;
            novo->prox = NULL;
            novo->ant = NULL;

            if(*iniPet == NULL) {
                *iniPet = novo;
            } else {
                NoPet *aux = *iniPet;
                while(aux->prox) {
                    aux = aux->prox;
                }
                aux->prox = novo;
                novo->ant = aux;
            }
        }
        posicaoAtual = ftell(arquivo);
    }
    
    fclose(arquivo);
}

void salvarPetsNoArquivo(NoPet **iniPet, char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "wb");

    // Não precisa de if, por que caso o arquivo não exista ele é criado
    NoPet *aux = *iniPet;
    
    // Garantir que o ponteiro está no inicio do arquivo
    fseek(arquivo, 0, SEEK_SET);
    
    while(aux != NULL) {
        if(aux->p->ativo == 1) {
            fwrite((*aux).p, sizeof(pet), 1, arquivo);
        }
        
        aux = (*aux).prox;
    }
    
    fclose(arquivo);
}

void mostrarPets(NoPet **iniPet) {
    NoPet *aux = *iniPet;
    printf("\n-------------------------\n");
    while(aux) {
        printf("Código: %d\n", (*aux).p->codigo);
        printf("Código do dono: %s\n", (*aux).p->codigo_pes);
        printf("nome: %d\n", (*aux).p->nome);
        printf("Código do tipo: %s\n", (*aux).p->codigo_tipo);

        aux = (*aux).prox;
    }
    printf("\n-------------------------\n");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// FUNÇÕES PARA TIPO DE PET

void inserirTipoDePet(NoTipoDePet **iniTipoDePet, tipoPet *p) {
    // Para verificar se já existe um tipo de pet com esse código
    if(buscarTipoDePetPorCodigo(iniTipoDePet, (*p).codigo)) {
        printf("\nJá existe um tipo de pet com esse código, não é possível inserir \n");
        return;
    }
    NoTipoDePet *elementoInserido = (NoTipoDePet *) malloc(sizeof(NoTipoDePet));

    if(elementoInserido == NULL) {
        return;
    }

    NoTipoDePet *aux = *iniTipoDePet;

    while(aux && (*aux).prox != NULL) {
        aux = (*aux).prox;
    }
    
    if(*iniTipoDePet == NULL) {
        (*elementoInserido).ant = NULL;
        *iniTipoDePet = elementoInserido;
    } else {
        (*aux).prox = elementoInserido;
        (*elementoInserido).ant = aux;
    }
    (*elementoInserido).prox = NULL;
    (*elementoInserido).p = p;

    // Alteração no arquivo

    p->ativo = 1;

    // ab para escrever sempre no final do arquivo
    FILE *arquivo = fopen("ArquivosBinarios/tiposDePet.bin", "ab");

    if(arquivo) {
        p->posicaoNoArquivo = ftell(arquivo);

        fwrite(p, sizeof(tipoPet), 1 , arquivo);

        fclose(arquivo);
    }
}

void removerTipoDePet(NoTipoDePet **iniTipoDePet, NoPet **iniPet, int codigo) {
    if(*iniTipoDePet == NULL) {
        printf("\nA lista está vazia!!!\n");
        return;
    }
    NoTipoDePet *alvoTipoDePet = buscarTipoDePetPorCodigo(iniTipoDePet, codigo);

    if(alvoTipoDePet == NULL) {
        printf("\n O tipo de código não existe\n");
        return;
    }

    // Percorrer a lista de pets, caso exista algum pet com aquele tipo, ele também deverá ser apagado
    // Verificar se tem algum pet com aquele tipo

    NoPet *auxPet = *iniPet;

    while(auxPet) {
        NoPet *proximoPet = (*auxPet).prox;
        if((*auxPet).p->codigo_tipo == codigo) {
            removerPet(iniPet, (*auxPet).p->codigo);
        }
        auxPet = proximoPet;
    }

    // Aqui acontecerá a remoção do tipo de pet

    // Caso esteja na primeira posição
    if((*alvoTipoDePet).ant == NULL) {
        *iniTipoDePet = (*alvoTipoDePet).prox;

        // Verificar caso tenha apenas um elemento
        if((*alvoTipoDePet).prox) {
            alvoTipoDePet->prox->ant = NULL;
        }
    } else {
        alvoTipoDePet->ant->prox = alvoTipoDePet->prox;

        // Caso não seja o último elemento
        if(alvoTipoDePet->prox) {
            alvoTipoDePet->prox->ant = alvoTipoDePet->ant;
        }
    }

    // Alteração do arquivo

    // Mudar o campo de ativo
    alvoTipoDePet->p->ativo = 0;

    FILE *arquivo = fopen("ArquivosBinarios/tiposDePet.bin", "r+b");

    if(arquivo) {
        fseek(arquivo, alvoTipoDePet->p->posicaoNoArquivo, SEEK_SET);

        // Sobescreve os dados, para ativo = 0;
        fwrite(alvoTipoDePet->p, sizeof(tipoPet), 1, arquivo);
        fclose(arquivo);
    }

    free((*alvoTipoDePet).p);
    free(alvoTipoDePet);
}

NoTipoDePet *buscarTipoDePetPorCodigo(NoTipoDePet **iniTipoDePet, int codigo) {
    NoTipoDePet *aux = *iniTipoDePet;

    while(aux != NULL) {
        if( aux->p->ativo == 1 && (*aux).p->codigo == codigo) {
            return aux;
        }
        aux = (*aux).prox;
    }

    return NULL;
}

void alterarTipoDePet(NoTipoDePet **iniTipoDePet, tipoPet *novosDados) {
    NoTipoDePet *alvoTipoDePet = buscarTipoDePetPorCodigo(iniTipoDePet, (*novosDados).codigo);

    if(!alvoTipoDePet) {
        printf("\nO tipo de pet não foi encontrado!!!\n");
        return;
    }

    strcpy(alvoTipoDePet->p->nome, novosDados->nome);

    // Alteração no arquivo

    FILE *arquivo = fopen("ArquivosBinarios/tiposDePet.bin", "r+b");

    if(arquivo) {
        fseek(arquivo, alvoTipoDePet->p->posicaoNoArquivo, SEEK_SET);

        // Sobescreve o arquivo com os dados já alterados
        fwrite(alvoTipoDePet->p, sizeof(tipoPet), 1, arquivo);

        fclose(arquivo);
    }
}

void finalizarListaDeTiposDePet(NoTipoDePet **iniTipoDePet) {
    if(*iniTipoDePet == NULL) {
        printf("\nA lista está vazia\n");
    }

    NoTipoDePet *aux = *iniTipoDePet;
    NoTipoDePet *ant = NULL;

    while(aux) {
        ant = aux;
        aux = (*aux).prox;
        free((*ant).p);
        free(ant);
    }
}

void pegarTiposDePetArquivo(NoTipoDePet **iniTipoDePet, char *nomeArquivo) {
    // Abre o arquivo para leitura
    FILE *arquivo = fopen(nomeArquivo, "rb");
    
    if(!arquivo) {
        return;
    }
    
    // Garantir que o ponteiro está no início do arquivo
    fseek(arquivo, 0, SEEK_SET);
    
    // Para armazenar os valores intermediários
    tipoPet temp;

    long posicaoAtual = ftell(arquivo);

    *iniTipoDePet = NULL;
    
    // Se for diferente de 1 , é porque ele chegou ao final do arquivo 
    while(fread(&temp, sizeof(tipoPet), 1, arquivo) == 1) {

        if(temp.ativo == 1) {
            NoTipoDePet *novo = (NoTipoDePet *)malloc(sizeof(NoTipoDePet));
            tipoPet *p = (tipoPet*)malloc(sizeof(tipoPet));
            *p = temp;
            p->posicaoNoArquivo = posicaoAtual;

            novo->p = p;
            novo->prox = NULL;
            novo->ant = NULL;

            if(*iniTipoDePet == NULL) {
                *iniTipoDePet = novo;
            } else {
                NoTipoDePet *aux = *iniTipoDePet;
                while(aux && aux->prox) {
                    aux = aux->prox;
                }
                aux->prox = novo;
                novo->ant = aux;
            }
        }
        posicaoAtual = ftell(arquivo);
    }
    
    fclose(arquivo);
}

void salvarTiposDePetNoArquivo(NoTipoDePet **iniTipoDePet, char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "wb");

    // Não precisa de if, por que caso o arquivo não exista ele é criado
    NoTipoDePet *aux = *iniTipoDePet;

    // Garantir que o ponteiro está no inicio do arquivo
    fseek(arquivo, 0, SEEK_SET);

    while(aux != NULL) {
        if(aux->p->ativo == 1) {
            fwrite((*aux).p, sizeof(tipoPet), 1, arquivo);
        }

        aux = (*aux).prox;
    }

    fclose(arquivo);
}

void mostrarTiposDePet(NoTipoDePet **iniTipoDePet) {
    NoTipoDePet *aux = *iniTipoDePet;
    printf("\n-------------------------\n");
    while(aux) {
        printf("Código: %d\n", (*aux).p->codigo);
        printf("nome: %s\n", (*aux).p->nome);

        aux = (*aux).prox;
    }
    printf("\n-------------------------\n");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FUNÇÕES GERAIS

void finalizar(NoPessoa **iniPes, NoPet **iniPet, NoTipoDePet **iniTipoDePet) {
    finalizarListaDePessoas(iniPes);
    finalizarListaDePets(iniPet);
    finalizarListaDeTiposDePet(iniTipoDePet);
}

void processarFilaTiposDePets(NoFilaTipoPet *fila, NoTipoDePet **iniTipoDePet, NoPet **iniPet) {
    while(fila) {
        int op = fila->info.operacao;

        switch(op) {
            case 1:
               inserirTipoDePet(iniTipoDePet, fila->info.dadosTipoDePet);
               break;
            case 2:
               removerTipoDePet(iniTipoDePet, iniPet, fila->info.idAlvo);
               break;
            case 3:
               alterarTipoDePet(iniTipoDePet, fila->info.dadosTipoDePet);
               // Liberar o ponteiro de novosDados
               if(fila->info.dadosTipoDePet) free(fila->info.dadosTipoDePet);
               break;
        }

        NoFilaTipoPet *temp = fila;
        fila = (*fila).prox;
        free(temp);
    }
}

void processarFilaPessoas(NoFilaPessoa *fila, NoPessoa **iniPes, NoPet **iniPet) {
    while(fila) {
        int op = fila->info.operacao;

        switch(op) {
            case 1:
               inserirPessoa(iniPes, fila->info.dadosPessoa);
               break;
            case 2:
               removerPessoa(iniPes, iniPet, fila->info.idAlvo);
               break;
            case 3:
                alterarPessoa(iniPes, fila->info.dadosPessoa);
                // Liberar o ponteiro de novos Dados
                if(fila->info.dadosPessoa) free(fila->info.dadosPessoa);    
               break;
        }

        NoFilaPessoa *temp = fila;
        fila = (*fila).prox;
        free(temp);
    }
}

void processarFilaPets(NoFilaPet *fila, NoPet **iniPet, NoPessoa **iniPes, NoTipoDePet **iniTipoDePet) {
    while(fila) {
        int op = fila->info.operacao;

        switch(op) {
            case 1:
               inserirPet(iniPet, iniPes, iniTipoDePet, fila->info.dadosPet);
               break;
            case 2:
               removerPet(iniPet, fila->info.idAlvo);
               break;
            case 3:
               alterarPet(iniPet, iniPes, iniTipoDePet, fila->info.dadosPet);
               // Liberar o ponteiro de novos dados
               if(fila->info.dadosPet) free(fila->info.dadosPet);
               break;
        }

        NoFilaPet *temp = fila;
        fila = (*fila).prox;
        free(temp);
    }
}

void adicionarComandoNoTXT() {
    char comando[300];
    fflush(stdin); 
    printf("\nDigite o novo comando SQL:\n> ");
    fgets(comando, 300, stdin);
    comando[strcspn(comando, "\n")] = 0;

    FILE *arq = fopen("comandos.txt", "a");
    if (!arq) return;

    fprintf(arq, "\n%s", comando);
    
    fclose(arq);
    printf("[SUCESSO] Comando gravado.\n");
}

void processarNovosComandos(long *cursorTXT, NoPessoa **listaPessoas, NoPet **listaPets, NoTipoDePet **listaTipos) {
    
    // 1. Cria fila vazia
    NoFilaPessoa *filaPes = NULL;
    NoFilaPet *filaPet = NULL;
    NoFilaTipoPet *filaTipo = NULL;
    Fila *filaBruta = criar_fila();

    // 2. Lê APENAS O QUE É NOVO (usando o cursor)
    ler_arquivo(&filaBruta, "comandos.txt", cursorTXT);

    if (filaBruta->ini == NULL) {
        printf("\n[INFO] Nao ha novos comandos no arquivo para processar.\n");
    } else {
        printf("\n[SISTEMA] Processando %d novos comandos...\n", filaBruta->tam);
        
        // 3. Distribui e Executa (usando inserirPessoa, inserirPet...)
        DistribuirComandos(filaBruta, &filaPes, &filaPet, &filaTipo);
        
        processarFilaPessoas(filaPes, listaPessoas, listaPets);
        processarFilaTiposDePets(filaTipo, listaTipos, listaPets);
        processarFilaPets(filaPet, listaPets, listaPessoas, listaTipos);

        // O salvamento no binário já acontece dentro das funções inserir...
        printf("[SUCESSO] Novos comandos executados!\n");
    }
    
    free(filaBruta);
}

void exibir(NoPessoa **listaPessoas, NoPet **listaPets, NoTipoDePet **listaTipos) {
    printf("\n=== BANCO DE DADOS ATUAL ===\n");
    mostrarPessoas(listaPessoas);
    printf("\n");
    mostrarTiposDePet(listaTipos);
    printf("\n");
    mostrarPets(listaPets);
    printf("============================\n");
    system("pause");
}

int main() {

    NoPessoa *listaPessoas = NULL;
    NoPet *listaPets = NULL;
    NoTipoDePet *listaTiposDePet = NULL;

    NoFilaPessoa *filaDeComandosDePessoas = NULL;
    NoFilaPet *filaDeComandosDePets = NULL;
    NoFilaTipoPet *filaDeComandosDeTiposDePets = NULL;

    long cursorTXT = 0;

    pegarPessoasArquivo(&listaPessoas, "ArquivosBinarios/pessoas.bin");
    pegarTiposDePetArquivo(&listaTiposDePet, "ArquivosBinarios/tiposDePet.bin");
    pegarPetsArquivo(&listaPets, &listaPessoas, &listaTiposDePet, "ArquivosBinarios/pet.bin");


    int opcao = 0;
    do {
        system("cls");
        printf("\n=== MENU PET SHOP ===\n");
        printf("1. Ver Dados\n");
        printf("2. Digitar Novo Comando\n");
        printf("3. Processar Novos Comandos\n");
        printf("0. Sair\n");
        printf("Escolha: ");
        scanf("%d", &opcao);

        switch(opcao) {
            case 1:
                exibir(&listaPessoas, &listaPets, &listaTiposDePet);
                break;
            case 2:
                adicionarComandoNoTXT();
                break;
            case 3:
                // Passamos o endereço do cursor para ele atualizar
                processarNovosComandos(&cursorTXT, &listaPessoas, &listaPets, &listaTiposDePet);
                system("pause");
                break;
        }
    } while(opcao != 0);

    finalizar(&listaPessoas, &listaPets, &listaTiposDePet);

    return 0;
}
