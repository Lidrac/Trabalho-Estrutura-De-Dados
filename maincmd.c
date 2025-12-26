// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include "comando.h"
// #include "pessoa.h"
// #include "pet.h"
// #include "tipoPet.h"

// int main()
// {
//     system("chcp 65001");
//     system("cls");

//     printf("1. programa rodou\n");
//     Fila *minhaFila = criar_fila();
//     printf("2. fila criou\n");
//     ler_arquivo(&minhaFila, "comandos.txt");
//     printf("3. leitura feita\n");
//     if (minhaFila->ini == NULL)
//     {
//         printf("A fila esta vazia! A leitura falhou ou o arquivo esta vazio.\n");
//     }
//     else
//     {
//         printf("4. A fila tem itens. Imprimindo agora:\n");
//         imprimir_fila(&minhaFila);
//     }

//     printf("5. Fim do programa.\n");
//     return 0;
// }
//