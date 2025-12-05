#ifndef SORTS_H
#define SORTS_H

#define SUCESSO 1
#define FALHA -1
#define CHAVE_INVALIDA 0

#define TAM_MAX_NUMS 999999

typedef struct inteiros
{
	int nums[TAM_MAX_NUMS];
	unsigned long int length;
}Inteiros;

Inteiros* inteiros_create(int length, int min, int max);

void inteiros_randomizar(Inteiros* inteiros);

Inteiros* inteiros_destroy(Inteiros** lixo);

int inteiros_durmam_bem_sort(Inteiros* inteiros);
	//Ou Sleep_Well_Sort

int inteiros_merge_sort(Inteiros* inteiros);

#endif