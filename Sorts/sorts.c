#include "sorts.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* -----------------------------------------
 * Configurações internas
 * ----------------------------------------- */
#define TAM_MAX_BUCKET 257  /* por que: mapeamento 2D (i,j) com j em [0..256] */

/* -----------------------------------------
 * Utilidades (aleatório/clamp)
 * ----------------------------------------- */
static void seed_once(void) {
    static int seeded = 0;
    if (!seeded) { srand((unsigned)time(NULL)); seeded = 1; }
}
static int clamp_len(int length) {
    if (length < 0) return 0;
    if ((unsigned long)length > TAM_MAX_NUMS) return TAM_MAX_NUMS;
    return length;
}
static int rand_in_range(int a, int b) {
    int mn = a, mx = b;
    if (mn > mx) { int t = mn; mn = mx; mx = t; }
    return mn + (rand() % (mx - mn + 1));
}

/* -----------------------------------------
 * API Inteiros
 * ----------------------------------------- */
Inteiros* inteiros_create(int length, int min, int max) {
    seed_once();

    Inteiros* v = (Inteiros*)malloc(sizeof *v);
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

    /* por que: Fisher–Yates (uniforme) */
    for (unsigned long i = n - 1; i > 0; i--) {
        unsigned long j = (unsigned long)(rand() % (int)(i + 1));
        int t = v->nums[i];
        v->nums[i] = v->nums[j];
        v->nums[j] = t;
    }
}

Inteiros* inteiros_destroy(Inteiros** p) {
    if (!p || !*p) return NULL;
    free(*p);
    *p = NULL;
    return NULL;
}

/* -----------------------------------------
 * Merge Sort (buffer único no heap)
 * ----------------------------------------- */
static int*           merge_tmp     = NULL;
static unsigned long  merge_tmp_cap = 0;

static int ensure_tmp(unsigned long cap) {
    if (merge_tmp && merge_tmp_cap >= cap) return 1;
    free(merge_tmp);
    merge_tmp = (int*)malloc(cap * sizeof *merge_tmp);
    if (!merge_tmp) { perror("malloc merge_tmp"); merge_tmp_cap = 0; return 0; }
    merge_tmp_cap = cap;
    return 1;
}

static void merge_range(int* arr, long l, long m, long r) {
    /* por que: copiar 1x [l..r] simplifica e melhora localidade */
    memcpy(&merge_tmp[l], &arr[l], (size_t)(r - l + 1) * sizeof(int));

    int* left  = &merge_tmp[l];
    int* right = &merge_tmp[m + 1];

    long nL = m - l + 1;
    long nR = r - m;
    long iL = 0, iR = 0, k = l;

    while (iL < nL && iR < nR) {
        /* por que: <= preserva estabilidade */
        if (left[iL] <= right[iR]) arr[k++] = left[iL++];
        else                       arr[k++] = right[iR++];
    }
    while (iL < nL) arr[k++] = left[iL++];
    while (iR < nR) arr[k++] = right[iR++];
}

static void msort_rec(int* arr, long l, long r) {
    if (l >= r) return;
    long m = l + (r - l) / 2;

    msort_rec(arr, l, m);
    msort_rec(arr, m + 1, r);

    /* por que: corta merges desnecessários */
    if (arr[m] <= arr[m + 1]) return;

    merge_range(arr, l, m, r);
}

int inteiros_merge_sort(Inteiros* v) {
    if (!v)           return CHAVE_INVALIDA;
    if (v->length <= 1) return SUCESSO;
    if (!ensure_tmp(v->length)) return FALHA;

    msort_rec(v->nums, 0, (long)v->length - 1);

    free(merge_tmp); merge_tmp = NULL; merge_tmp_cap = 0;
    return SUCESSO;
}

/* -----------------------------------------
 * Timing Wheel (listas encadeadas por slot)
 * ----------------------------------------- */
typedef struct node {
    int            dado;
    struct node*   prox;
} No;

static No* no_create(int dado) {
    No* n = (No*)calloc(1, sizeof(No));
    if (!n) printf("Error: sem espaco na memoria para fazer operacao.\n");
    else    n->dado = dado;
    return n;
}

/* por que: consumir sempre a cabeça evita pular o primeiro nó */
static No* no_destroy(No** lixo) {
    if (lixo == NULL || *lixo == NULL) {
        printf("ERROR ao deletar No do timer!!!\n");
        return NULL;
    }
    No* prox = (*lixo)->prox;
    free(*lixo);
    *lixo = NULL;
    return prox;
}

typedef struct timing_wheel {
    No*                 rodinha[TAM_MAX_BUCKET];
    unsigned long int   qtd_dados;
} Roda;

static Roda* roda_create(void) {
    Roda* r = (Roda*)calloc(1, sizeof(Roda));
    if (!r) printf("Error: sem espaco na memoria para fazer operacao.\n");
    return r;
}
static int roda_destroy(Roda** lixo) {
    if (!lixo || !*lixo) {
        printf("ERROR ao deletar roda do timer!!!\n");
        return FALHA;
    }
    free(*lixo);
    *lixo = NULL;
    return SUCESSO;
}

/* -----------------------------------------
 * Durmam Bem Sort (varre todos os slots)
 * ----------------------------------------- */
int inteiros_durmam_bem_sort(Inteiros* v) {
    if (!v)            return CHAVE_INVALIDA;
    if (v->length == 0) return FALHA;

    int maxv = v->nums[0];
    for (unsigned long i = 0; i < v->length; i++)
        if (maxv < v->nums[i]) maxv = v->nums[i];

    if (maxv < 0) {
        fprintf(stderr, "Valores negativos nao suportados em durmam_bem_sort.\n");
        return FALHA;
    }

    int qtd_rodas = (maxv / TAM_MAX_BUCKET) + 1;

    Roda* rodao[qtd_rodas];
    for (int i = 0; i < qtd_rodas; i++) rodao[i] = NULL;

    /* espalha (inserção na cabeça para não perder corrente) */
    for (unsigned long i = 0; i < v->length; i++) {
        int idx    = v->nums[i];
        int i_roda = idx / TAM_MAX_BUCKET;
        int j_slot = idx % TAM_MAX_BUCKET;

        if (rodao[i_roda] == NULL) {
            rodao[i_roda] = roda_create();
            if (!rodao[i_roda]) return FALHA;
        }
        if (rodao[i_roda]->rodinha[j_slot] == NULL) {
            rodao[i_roda]->rodinha[j_slot] = no_create(v->nums[i]);
            if (!rodao[i_roda]->rodinha[j_slot]) return FALHA;
        } else {
            No* novo = no_create(v->nums[i]);
            if (!novo) return FALHA;
            novo->prox = rodao[i_roda]->rodinha[j_slot];
            rodao[i_roda]->rodinha[j_slot] = novo;
        }
        rodao[i_roda]->qtd_dados++;
    }

    /* dreno varrendo todos os 257 slots (simples, direto) */
    unsigned long k = 0;
    for (int i = 0; i < qtd_rodas; i++) {
        if (rodao[i] != NULL) {
            for (int j = 0; j < TAM_MAX_BUCKET; j++) {
                while (rodao[i]->rodinha[j] != NULL) {
                    v->nums[k++] = rodao[i]->rodinha[j]->dado;
                    rodao[i]->rodinha[j] = no_destroy(&rodao[i]->rodinha[j]);
                    rodao[i]->qtd_dados--;
                }
            }
            roda_destroy(&rodao[i]);
        }
    }
    return SUCESSO;
}

/* -----------------------------------------
 * Sleep Weel Sort (tempo VIRTUAL + linhas/slots ocupados)
 *   - percorre somente linhas usadas (rows_used)
 *   - em cada linha, somente slots ocupados (bitmask 257 bits)
 * ----------------------------------------- */
typedef struct {
    Roda*    roda;
    uint64_t mask[(TAM_MAX_BUCKET + 63) / 64]; /* 257 -> 5 words de 64 bits */
} RodaBits;

/* menor bit set (lsb) — GCC/Clang usam builtin; fallback é portável (lento, mas OK) */
static inline int lsb_index64(uint64_t w) {
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_ctzll(w);
#else
    int idx = 0; while (((w >> idx) & 1ULL) == 0ULL) idx++; return idx;
#endif
}
static inline void set_bit(uint64_t* mask, int j) {
    mask[j >> 6] |= (1ULL << (j & 63));
}

/* comparador de inteiros para qsort (C padrão) */
static int cmp_int_asc(const void* a, const void* b) {
    int A = *(const int*)a, B = *(const int*)b;
    return (A > B) - (A < B);
}

int inteiros_sleep_weel_sort(Inteiros* v, long tick_ns /* ignorado */) {
    (void)tick_ns; /* por que: tempo virtual, mantido por compatibilidade */

    if (!v)            return CHAVE_INVALIDA;
    if (v->length == 0) return FALHA;

    /* suporta negativos via offset (ordem preservada) */
    int minv = v->nums[0], maxv = v->nums[0];
    for (unsigned long i = 1; i < v->length; i++) {
        if (v->nums[i] < minv) minv = v->nums[i];
        if (v->nums[i] > maxv) maxv = v->nums[i];
    }
    long offset = (minv < 0) ? -(long)minv : 0L;
    long range  = (long)maxv + offset;
    if (range < 0) range = 0;

    int qtd_rodas = (int)(range / TAM_MAX_BUCKET) + 1;
    if (qtd_rodas <= 0) qtd_rodas = 1;

    /* por que: heap evita stack overflow em ranges grandes */
    RodaBits* infos     = (RodaBits*)calloc((size_t)qtd_rodas, sizeof(RodaBits));
    unsigned char* seen = (unsigned char*)calloc((size_t)qtd_rodas, sizeof(unsigned char));
    int* rows_used      = (int*)malloc((size_t)qtd_rodas * sizeof(int));
    if (!infos || !seen || !rows_used) {
        free(infos); free(seen); free(rows_used);
        return FALHA;
    }

    int rows_cnt = 0;

    /* espalha: cria roda sob demanda, encadeia no slot e marca bit */
    for (unsigned long idx = 0; idx < v->length; idx++) {
        long adj   = (long)v->nums[idx] + offset;
        int  i_row = (int)(adj / TAM_MAX_BUCKET);
        int  j_col = (int)(adj % TAM_MAX_BUCKET);

        if (infos[i_row].roda == NULL) {
            infos[i_row].roda = roda_create();
            if (!infos[i_row].roda) { free(infos); free(seen); free(rows_used); return FALHA; }
        }

        if (infos[i_row].roda->rodinha[j_col] == NULL) {
            infos[i_row].roda->rodinha[j_col] = no_create(v->nums[idx]);
            if (!infos[i_row].roda->rodinha[j_col]) { free(infos); free(seen); free(rows_used); return FALHA; }
            set_bit(infos[i_row].mask, j_col);  /* marca slot ocupado */
        } else {
            No* novo = no_create(v->nums[idx]);
            if (!novo) { free(infos); free(seen); free(rows_used); return FALHA; }
            novo->prox = infos[i_row].roda->rodinha[j_col];
            infos[i_row].roda->rodinha[j_col] = novo;
        }
        infos[i_row].roda->qtd_dados++;

        if (!seen[i_row]) { seen[i_row] = 1; rows_used[rows_cnt++] = i_row; }
    }

    /* por que: garantir ordem crescente de linhas */
    if (rows_cnt > 1) qsort(rows_used, (size_t)rows_cnt, sizeof(int), cmp_int_asc);

    /* dreno: apenas linhas usadas, apenas slots marcados (j em ordem) */
    unsigned long k = 0;

    for (int r = 0; r < rows_cnt; r++) {
        int i = rows_used[r];
        if (infos[i].roda == NULL) continue;

        for (int wi = 0; wi < (int)((TAM_MAX_BUCKET + 63) / 64); wi++) {
            uint64_t w = infos[i].mask[wi];
            while (w) {
                int bit = lsb_index64(w);
                int j   = wi * 64 + bit;
                if (j >= TAM_MAX_BUCKET) break; /* defesa */

                while (infos[i].roda->rodinha[j] != NULL) {
                    v->nums[k++] = infos[i].roda->rodinha[j]->dado;
                    infos[i].roda->rodinha[j] = no_destroy(&infos[i].roda->rodinha[j]);
                    infos[i].roda->qtd_dados--;
                }
                w &= (w - 1); /* limpa o menor bit set */
            }
        }
        roda_destroy(&infos[i].roda);
    }

    free(rows_used);
    free(seen);
    free(infos);
    return SUCESSO;
}

