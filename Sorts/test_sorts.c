#include "./sorts.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* -----------------------------------------
 * Validação e medição
 * ----------------------------------------- */
static int is_sorted(const Inteiros* v) {
    if (!v) return 0;
    for (unsigned long i = 1; i < v->length; i++)
        if (v->nums[i - 1] > v->nums[i]) return 0;
    return 1;
}
static inline uint64_t now_ns(void) {
    struct timespec ts; clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ull + (uint64_t)ts.tv_nsec;
}
typedef int (*sort_fn)(Inteiros*);
static uint64_t bench(const char* name, sort_fn fn, Inteiros* v) {
    uint64_t t0 = now_ns();
    int rc      = fn(v);
    uint64_t t1 = now_ns();

    int ok = is_sorted(v);
    printf("%s: rc=%d, sorted=%s, ns=%" PRIu64 " (%.3f ms)\n",
           name, rc, ok ? "SIM" : "NAO", (t1 - t0), (t1 - t0) / 1e6);
    return (t1 - t0);
}
/* wrapper: assinatura compatível com bench */
static int run_sleep_weel(Inteiros* v) { return inteiros_sleep_weel_sort(v, 0L); }

/* -----------------------------------------
 * Main
 * ----------------------------------------- */
int main(int argc, char** argv) {
    long reqN = (argc >= 2) ? strtol(argv[1], NULL, 10) : (long)TAM_MAX_NUMS;
    if (reqN < 0) reqN = 0;
    if ((unsigned long)reqN > TAM_MAX_NUMS) reqN = TAM_MAX_NUMS;
    printf("Config: N=%ld (max=%d)\n", reqN, TAM_MAX_NUMS);

    Inteiros* base = inteiros_create((int)reqN, 0, 999999);
    if (!base) { fprintf(stderr, "Falha ao alocar base\n"); return 1; }

    Inteiros* a = inteiros_create((int)reqN, 0, 0);
    Inteiros* b = inteiros_create((int)reqN, 0, 0);
    Inteiros* c = inteiros_create((int)reqN, 0, 0);
    if (!a || !b || !c) {
        fprintf(stderr, "Falha ao alocar copias\n");
        inteiros_destroy(&base); inteiros_destroy(&a); inteiros_destroy(&b); inteiros_destroy(&c);
        return 1;
    }
    if (base->length) {
        memcpy(a->nums, base->nums, base->length * sizeof base->nums[0]);
        memcpy(b->nums, base->nums, base->length * sizeof base->nums[0]);
        memcpy(c->nums, base->nums, base->length * sizeof base->nums[0]);
    }
    a->length = base->length; b->length = base->length; c->length = base->length;

    uint64_t t_merge = bench("merge_sort",      inteiros_merge_sort,      a);
    uint64_t t_durm  = bench("durmam_bem_sort", inteiros_durmam_bem_sort, b);
    uint64_t t_sleep = bench("sleep_weel_sort", run_sleep_weel,           c);

    printf("Comparativo (ns): merge=%" PRIu64 ", durmam=%" PRIu64 ", sleep_weel=%" PRIu64 "\n",
           t_merge, t_durm, t_sleep);

    inteiros_destroy(&base); inteiros_destroy(&a); inteiros_destroy(&b); inteiros_destroy(&c);
    return 0;
}