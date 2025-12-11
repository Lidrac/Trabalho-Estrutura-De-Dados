#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pessoa.h"
#include "pet.h"
#include "tipoPet.h"

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

    free((*pessoaAlvo).p);
    free(pessoaAlvo);

}

NoPessoa *buscarPessoaPorCodigo(NoPessoa **iniPes, int codigo) {
    NoPessoa *aux = *iniPes;

    while(aux != NULL) {
        if((*aux).p->codigo == codigo) {
            return aux;
        }
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
    FILE *arquivo = fopen(nomeArquivo, "rb");
    
    if(!arquivo) {
        printf("\nArquivo não existe\n");
        return;
    }
    
    // Garantir que o ponteiro está no início do arquivo
    fseek(arquivo, 0, SEEK_SET);
    
    // Para armazenar os valores intermediários
    pessoa temp;
    
    // Se for diferente de 1 , é porque ele chegou ao final do arquivo 
    while(fread(&temp, sizeof(pessoa), 1, arquivo) == 1) {
        
        // Alocando memória para o ponteiro de pessoa
        pessoa *novo = (pessoa *)malloc(sizeof(pessoa));
        *novo = temp;
        
        // Inserindo na lista duplamente encadeada de pessoas
        inserirPessoa(iniPes, novo);
    }
    
    fclose(arquivo);
}

void salvarPessoasNoArquivo(NoPessoa **iniPes, char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "wb");
    
    // Não precisa de if, por que caso o arquivo não exista ele é criado
    NoPessoa *aux = *iniPes;
    
    // Garantir que o ponteiro está no inicio do arquivo
    fseek(arquivo, 0, SEEK_SET);
    
    while(aux != NULL) {
        fwrite((*aux).p, sizeof(pessoa), 1, arquivo);
        
        aux = (*aux).prox;
    }
    
    fclose(arquivo);
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
    if(buscarPessoaPorCodigo(iniPes, p->codigo_pes) && buscarTipoDePetPorCodigo(iniTipoDePet, p->codigo_tipo)) {
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

    free((*petAlvo).p);
    free(petAlvo);

    
}

NoPet *buscarPetPorCodigo(NoPet **iniPet, int codigo) {
    NoPet *aux = *iniPet;

    while(aux != NULL) {
        if((*aux).p->codigo == codigo) {
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
        printf("\nArquivo não existe\n");
        return;
    }
    
    // Garantir que o ponteiro está no início do arquivo
    fseek(arquivo, 0, SEEK_SET);
    
    // Para armazenar os valores intermediários
    pet temp;
    
    // Se for diferente de 1 , é porque ele chegou ao final do arquivo 
    while(fread(&temp, sizeof(pet), 1, arquivo) == 1) {
        
        // Alocando memória para o ponteiro de pet
        pet *novo = (pet *)malloc(sizeof(pet));
        *novo = temp;
        
        // Inserindo na lista duplamente encadeada de pets
        inserirPet(iniPet, iniPes, iniTipoDePet, novo);
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
        fwrite((*aux).p, sizeof(pet), 1, arquivo);
        
        aux = (*aux).prox;
    }
    
    fclose(arquivo);
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

    free((*alvoTipoDePet).p);
    free(alvoTipoDePet);
}

NoTipoDePet *buscarTipoDePetPorCodigo(NoTipoDePet **iniTipoDePet, int codigo) {
    NoTipoDePet *aux = *iniTipoDePet;

    while(aux != NULL) {
        if((*aux).p->codigo == codigo) {
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
        printf("\nArquivo não existe\n");
        return;
    }
    
    // Garantir que o ponteiro está no início do arquivo
    fseek(arquivo, 0, SEEK_SET);
    
    // Para armazenar os valores intermediários
    tipoPet temp;
    
    // Se for diferente de 1 , é porque ele chegou ao final do arquivo 
    while(fread(&temp, sizeof(tipoPet), 1, arquivo) == 1) {
        
        // Alocando memória para o ponteiro de Tipo de Pet
        tipoPet *novo = (tipoPet *)malloc(sizeof(tipoPet));
        *novo = temp;
        
        // Inserindo na lista duplamente encadeada de pets
        inserirTipoDePet(iniTipoDePet, novo);
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
        fwrite((*aux).p, sizeof(tipoPet), 1, arquivo);

        aux = (*aux).prox;
    }

    fclose(arquivo);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FUNÇÕES GERAIS

void finalizar(NoPessoa **iniPes, NoPet **iniPet, NoTipoDePet **iniTipoDePet) {
    finalizarListaDePessoas(iniPes);
    finalizarListaDePets(iniPet);
    finalizarListaDeTiposDePet(iniTipoDePet);
}

void mostrarPessoas(NoPessoa **ini) {
    NoPessoa *aux = *ini;
    printf("\n-------------------------\n");
    while(aux) {
        printf("%d\n", (*aux).p->codigo);
        printf("%s\n", (*aux).p->nome);
        printf("%d\n", (*aux).p->fone);
        printf("%s\n", (*aux).p->endereco);
        printf("%s\n", (*aux).p->dataNascimento);

        aux = (*aux).prox;
    }
    printf("\n-------------------------\n");
}


int main() {

    return 0;
}
