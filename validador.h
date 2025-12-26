#ifndef VALIDADOR_H
#define VALIDADOR_H

#include "comando.h"
#include "pessoa.h"
#include "pet.h"
#include "tipoPet.h"

//

void DistribuirComandos(Fila *FilaBruta, NoFilaPessoa **FilaPessoa, NoFilaPet **FilaPet, NoFilaTipoPet **FilaTipo);
void MostrarFilasEspecificas(NoFilaPessoa *FilaPessoa, NoFilaPet *FilaPet, NoFilaTipoPet *FilaTipo);
char* trim(char *str);
void limpar_string(char *destino, char *origem);
int comeca_com(char *linha, char *termo);
int extrair_id_where(char *linha);
void AddFilaPessoa(NoFilaPessoa **fila, comandoPessoa cmd);
void AddFilaPet(NoFilaPet **fila, comandoPet cmd);
void AddFilaTipoPet(NoFilaTipoPet **fila, comandoTipoPet cmd);
int processar_tipo_pet(char *linha, int operacao, NoFilaTipoPet **fila);
int processar_pessoa(char *linha, int operacao, NoFilaPessoa **fila);
int processar_pet(char *linha, int operacao, NoFilaPet **fila);

#endif