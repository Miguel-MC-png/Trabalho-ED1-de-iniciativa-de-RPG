#ifndef SORTS_H
#define SORTS_H

#include <stddef.h>

/* Limite de elementos do vetor */
#define TAM_MAX_NUMS 999999

/* Códigos de retorno */
#ifndef SUCESSO
#define SUCESSO 1
#endif
#ifndef FALHA
#define FALHA -1
#endif
#ifndef CHAVE_INVALIDA
#define CHAVE_INVALIDA 0
#endif

/* Vetor de inteiros com tamanho fixo e comprimento lógico */
typedef struct inteiros {
    int nums[TAM_MAX_NUMS];
    unsigned long int length;
} Inteiros;

/* Criação/aleatorização/destruição */
Inteiros* inteiros_create(int length, int min, int max);
void      inteiros_randomizar(Inteiros* inteiros);
Inteiros* inteiros_destroy(Inteiros** lixo);

/* Sorts */
int inteiros_merge_sort(Inteiros* inteiros);
int inteiros_durmam_bem_sort(Inteiros* inteiros);

/* Tempo VIRTUAL + endereçamento direto (linhas/slots ocupados) */
int inteiros_sleep_weel_sort(Inteiros* inteiros, long tick_ns /* ignorado */);

#endif