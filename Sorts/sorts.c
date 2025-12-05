// sorts.c  (mesma API e layout, sem VLA no merge)
#include "sorts.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h> /* memcpy */

static void seed_once(void) {
    static int seeded = 0;
    if (!seeded) { srand((unsigned)time(NULL)); seeded = 1; }
}

static int rand_in_range(int a, int b) {
    int min = a, max = b;
    if (min > max) { int t = min; min = max; max = t; }
    return min + (rand() % (max - min + 1));
}

static int clamp_len(int length) {
    if (length < 0) return 0;
    if ((unsigned long)length > TAM_MAX_NUMS) return TAM_MAX_NUMS;
    return length;
}

Inteiros* inteiros_create(int length, int min, int max) {
    seed_once();

    Inteiros *v = (Inteiros*)malloc(sizeof *v);
    if (!v) { perror("malloc Inteiros"); return NULL; }

    v->length = (unsigned long)clamp_len(length);
    for (unsigned long i = 0; i < v->length; i++) v->nums[i] = rand_in_range(min, max);
    return v;
}

void inteiros_randomizar(Inteiros* v) {
    if (!v) return;
    seed_once();
    unsigned long n = v->length;
    if (n <= 1) return;

    for (unsigned long i = n - 1; i > 0; i--) {
        unsigned long j = (unsigned long)(rand() % (int)(i + 1));
        int tmp = v->nums[i];
        v->nums[i] = v->nums[j];
        v->nums[j] = tmp;
    }
}

Inteiros* inteiros_destroy(Inteiros** lixo) {
    if (!lixo || !*lixo) return NULL;
    free(*lixo); *lixo = NULL; return NULL;
}

/* ===== Buffer unico no heap para o merge ===== */
static int* merge_tmp = NULL;
static unsigned long merge_tmp_cap = 0;

static int ensure_tmp(unsigned long cap) {
    /* por que: alocar 1x tamanho maximo e reutilizar */
    if (merge_tmp && merge_tmp_cap >= cap) return 1;
    free(merge_tmp);
    merge_tmp = (int*)malloc(cap * sizeof *merge_tmp);
    if (!merge_tmp) { perror("malloc merge_tmp"); merge_tmp_cap = 0; return 0; }
    merge_tmp_cap = cap;
    return 1;
}

void merge(int* arr,
            long int inicio,
                long int meio,
                    long int fim)
{
    long int i_right, i_left, i_arr;

    long int tam_left  = meio - inicio + 1;
    long int tam_right = fim  - meio;

    /* copia [inicio..fim] para o buffer unico na mesma faixa
       (por que: manter indices iguais e evitar VLA na pilha) */
    memcpy(&merge_tmp[inicio], &arr[inicio], (size_t)(fim - inicio + 1) * sizeof(int));

    /* "slices" para esquerda/direita dentro do buffer */
    int* left  = &merge_tmp[inicio];
    int* right = &merge_tmp[meio + 1];

    i_left = 0;
    i_right = 0;
    i_arr = inicio;

    while (i_left < tam_left && i_right < tam_right) {
        if (left[i_left] <= right[i_right]) {
            arr[i_arr] = left[i_left];
            i_left++;
        } else {
            arr[i_arr] = right[i_right];
            i_right++;
        }
        i_arr++;
    }

    while (i_left < tam_left) {
        arr[i_arr] = left[i_left];
        i_left++;
        i_arr++;
    }

    while (i_right < tam_right) {
        arr[i_arr] = right[i_right];
        i_right++;
        i_arr++;
    }
}

void merge_sort(int* arr,
                    long int inicio,
                        long int fim)
{
    if (inicio < fim) {
        long int meio = (inicio + fim)/2;

        merge_sort(arr, inicio, meio);
        merge_sort(arr, meio + 1, fim);

        merge(arr, inicio, meio, fim);
    }
}

int inteiros_merge_sort(Inteiros* v)
{
    if (v == NULL)
        return CHAVE_INVALIDA;

    if (v->length == 0)
        return FALHA;

    /* garante buffer unico de O(n) no heap, 1x */
    if (!ensure_tmp(v->length)) return FALHA;

    merge_sort(v->nums, 0, (long int)v->length - 1);

    /* libera ao final desta operacao (mantive local ao sort) */
    free(merge_tmp); merge_tmp = NULL; merge_tmp_cap = 0;

    return SUCESSO;
}

//======= Durma =========//

/*
struct Roda {
    Bucket[bucket_count]  // lista ligada em cada slot
    int bucket_count
    int slot_intervalo  // intervalo do tempo que cada bucket cobre
    int limite_superior  // maior deadline que esta roda consegue representar
}
*/

typedef struct timing_wheel
    {
        int bucket[257];
        unsigned long int qtd_dados;
        long int interval[2];
        struct timing_wheel *prox;
    }Roda;

int inteiros_durma_bem_sort(Inteiros* v) 
{ 
    return 0;
}

