// =========================
// File: test_sorts.c
// =========================
#include "./sorts.h"
#include <stdio.h>

static void print_all(const Inteiros* v) {
    if (!v) { puts("(null)"); return; }
    if (v->length == 0) { puts("(vazio)"); return; }
    for (unsigned long i = 0; i < v->length; i++) {
        printf("%d ", v->nums[i]);
        if ((i + 1) % 20 == 0) puts("");
    }
    if (v->length % 20 != 0) puts("");
}

static int is_sorted(const Inteiros* v) {
    if (!v) return 0;
    for (unsigned long i = 1; i < v->length; i++)
        if (v->nums[i - 1] > v->nums[i]) return 0;
    return 1;
}

int main(void) {
    /* ajuste os parametros conforme desejar */
    const int LEN = 30;
    const int MINV = 0;
    const int MAXV = 99;

    Inteiros* v = inteiros_create(LEN, MINV, MAXV); /* cria aleatorio */
    if (!v) return 1;

    puts("=== Desordenado ===");
    print_all(v); /* imprime todos */

    int rc = inteiros_merge_sort(v);               /* ordena */
    printf("Merge sort retorno: %d\n", rc);

    int ok = is_sorted(v);                         /* confere ordenacao */
    printf("Ordenado? %s\n", ok ? "SIM" : "NAO");

    puts("=== Ordenado ===");
    print_all(v);

    inteiros_randomizar(v);                        /* randomiza de novo */
    puts("Randomizado novamente (preview de 20):");
    /* preview curto so para visual: */
    for (unsigned long i = 0; i < v->length && i < 20; i++) {
        printf("%d ", v->nums[i]);
    }
    puts("");

    inteiros_destroy(&v);
    return 0;
}
