// ==============================
// File: main.c  (corrigido com protótipos)
// ==============================
#include "persona.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_MAX_SALVOS 1000000

typedef struct simulacao_banco_de_dados {
    Persona* salvos[TAM_MAX_SALVOS];
    int em_uso[TAM_MAX_SALVOS];
    int length;
} Banco_de_dados;

/* ---------- Estado global do app ---------- */
static Banco_de_dados banco;
static Iniciativas* combate = NULL;
static int guerra_ativa = 0;
static int turno_idx = 0;        /* quem joga agora (0-based) */
static int rodada_nova = 0;      /* resort só no início da rodada */
static SortMethod metodo_atual = SORT_MERGE;

/* ---------- PROTÓTIPOS (evita implicit-function-declaration) ---------- */
static void hr(void);
static void sp(void);
static void title(const char* t);
static void section(const char* t);
static int  read_line(char *buf, size_t cap);
static int  read_int(const char *prompt, int *out);

void persona_config(void);
static void config(void);
static int  buscar_persona(char* nome);
static void perguntar_nome(char novo_nome[TAM_MAX_NOME]);

static void listar_iniciativa_com_turno(void);
static int  iniciar_combate(void);
static int  encerrar_combate(void);

static int  criar_personagem(void);
static char* apagar_personagem(void);

static int  adicionar_persona_combate_sorted(Iniciativas* g);
static int  remover_persona_combate_por_indice(Iniciativas* g, int indice1);

static void escolher_metodo(void);
static void loop_preparo(void);
static void loop_turnos(void);

/* ---------- UI helpers ---------- */
static void hr(void) { printf("====================================\n"); }
static void sp(void) { printf("\n"); }
static void title(const char* t) { sp(); hr(); printf(" %s\n", t); hr(); sp(); }
static void section(const char* t) { sp(); printf("-- %s --\n", t); sp(); }

/* ---------- IO ---------- */
static int read_line(char *buf, size_t cap) {
    if (!fgets(buf, (int)cap, stdin)) return 0;
    size_t n = strlen(buf);
    if (n && buf[n-1] == '\n') buf[n-1] = '\0';
    return 1;
}
static int read_int(const char *prompt, int *out) {
    char s[64];
    for (;;) {
        if (prompt && *prompt) printf("%s", prompt);
        if (!read_line(s, sizeof s)) return 0;
        char *end = NULL;
        long v = strtol(s, &end, 10);
        while (end && *end && isspace((unsigned char)*end)) end++;
        if (end && *end == '\0') { *out = (int)v; return 1; }
        printf("Entrada invalida. Tente novamente.\n");
    }
}

/* ---------- Banco ---------- */
static void config(void) { banco.length = 0; persona_config(); }
static int buscar_persona(char* nome) {
    for (int i = 0; i < banco.length; i++)
        if (strcmp(banco.salvos[i]->nome, nome) == 0)
            return i;
    return FALHA;
}
static void perguntar_nome(char novo_nome[TAM_MAX_NOME]) {
    printf("Nome do personagem: ");
    do { if (!read_line(novo_nome, TAM_MAX_NOME)) { novo_nome[0] = '\0'; continue; } }
    while (strlen(novo_nome) == 0);
}

/* ---------- Combate: visual ---------- */
static void listar_iniciativa_com_turno(void) {
    int n = iniciativas_get_tamanho(combate);
    if (n <= 0) { printf("Iniciativa vazia.\n"); return; }
    section("Iniciativa");
    for (int i = 1; i <= n; i++) {
        Persona *p = iniciativas_get_persona(combate, i);
        printf("%s %2d) %s  [ini=%d]\n", (i-1)==turno_idx ? "=>" : "  ", i, p->nome, p->iniciativa_atual);
    }
    sp();
    Persona *atual = iniciativas_get_persona(combate, turno_idx + 1);
    printf("Jogando agora: [%d] %s\n", turno_idx + 1, atual ? atual->nome : "(nenhum)");
    sp();
}

/* ---------- Combate: ciclo de vida ---------- */
static int iniciar_combate(void) {
    if (combate) return SUCESSO;
    combate = iniciativas_create();
    guerra_ativa = 0;
    turno_idx = 0;
    rodada_nova = 1;
    return combate ? SUCESSO : FALHA;
}
static int encerrar_combate(void) {
    int r = iniciativas_destroy(&combate);
    if (r == SUCESSO) {
        for (int i = 0; i < banco.length; i++) banco.em_uso[i] = 0;
        combate = NULL; guerra_ativa = 0; turno_idx = 0; rodada_nova = 0;
        printf("Combate encerrado.\n"); sp(); hr(); sp();
    }
    return r;
}

/* ---------- CRUD ---------- */
static int criar_personagem(void) {
    if (banco.length >= TAM_MAX_SALVOS) { printf("Nao ha como salvar mais.\n"); sp(); return FALHA; }
    char novo_nome[TAM_MAX_NOME]; int novo_nlv=0; int novo_dado = D0;

    section("Criar personagem");
    perguntar_nome(novo_nome);
    if (buscar_persona(novo_nome) != FALHA) { printf("Esse personagem ja existe.\n"); sp(); return FALHA; }

    do { read_int("Nivel (0-20): ", &novo_nlv); } while (novo_nlv < 0 || novo_nlv > 20);
    do {
        read_int("Dado de iniciativa (4/6/8/10/12/20): ", &novo_dado);
        switch (novo_dado) { case 4: case 6: case 8: case 10: case 12: case 20: break; default: novo_dado = D0; }
    } while (novo_dado == D0);

    Persona* novo = persona_create(novo_nome, novo_nlv, (Dado)novo_dado);
    if (!novo) { printf("Falha ao criar personagem!\n"); sp(); return FALHA; }

    banco.salvos[banco.length] = novo; banco.em_uso[banco.length] = 0; banco.length++;
    sp(); printf("Personagem salvo:\n"); persona_print(novo, 3); sp(); hr(); sp();
    return SUCESSO;
}
static char* apagar_personagem(void) {
    static char nome[TAM_MAX_NOME];
    section("Apagar personagem");
    perguntar_nome(nome);
    int end = buscar_persona(nome);
    if (end == FALHA) { printf("Nao encontrado.\n"); sp(); return NULL; }
    if (banco.em_uso[end] == 1) { printf("Em combate; nao pode ser removido.\n"); sp(); return NULL; }

    for (int i = end; i < banco.length - 1; i++) { banco.salvos[i] = banco.salvos[i + 1]; banco.em_uso[i] = banco.em_uso[i + 1]; }
    banco.salvos[banco.length - 1] = NULL; banco.em_uso[banco.length - 1] = 0; banco.length--;
    printf("Removido: %s\n", nome); sp(); hr(); sp();
    return nome;
}

/* ---------- Combate: add/rem ---------- */
static int adicionar_persona_combate_sorted(Iniciativas* g) {
    if (banco.length == 0) { printf("Nenhum personagem salvo.\n"); sp(); return FALHA; }
    char nome[TAM_MAX_NOME];
    section("Adicionar ao combate"); perguntar_nome(nome);

    int end = buscar_persona(nome);
    if (end == FALHA) { printf("Nao encontrado.\n"); sp(); return FALHA; }
    if (banco.em_uso[end] != 0) { printf("Ja esta na iniciativa.\n"); sp(); return FALHA; }

    Persona* p = banco.salvos[end];
    if (iniciativas_add_persona_sorted(g, p) == SUCESSO) {
        banco.em_uso[end] = 1;
        int n = iniciativas_get_tamanho(g), pos = n;
        for (int i = 1; i <= n; i++) if (iniciativas_get_persona(g, i) == p) { pos = i; break; }
        printf("'%s' foi adicionado na posicao %d.\n", nome, pos);
        sp(); hr(); sp();
        return pos;
    }
    return FALHA;
}
static int remover_persona_combate_por_indice(Iniciativas* g, int indice1) {
    if (indice1 <= 0 || indice1 > iniciativas_get_tamanho(g)) return FALHA;
    char *removido = iniciativa_remove_persona(g, indice1);
    if (!removido || !removido[0]) return FALHA;
    for (int i=0;i<banco.length;i++) if (banco.salvos[i] && strcmp(banco.salvos[i]->nome, removido)==0) banco.em_uso[i]=0;
    if (turno_idx >= iniciativas_get_tamanho(g)) turno_idx = 0;
    printf("Removido: %s\n", removido); sp(); hr(); sp();
    return SUCESSO;
}

/* ---------- Escolha do método ---------- */
static void escolher_metodo(void) {
    sp();
    printf("== Escolher metodo de ordenacao ==\n");
    printf("1 - Merge Sort (estavel)\n");
    printf("2 - Durmam Bem Sort (buckets)\n");
    printf("Escolha: ");
    int op=0; while (!read_int("", &op) || (op<1 || op>2)) { printf("Opcao invalida. Escolha 1 ou 2: "); }
    metodo_atual = (op==1)? SORT_MERGE : SORT_DURMAM;
    rodada_nova = 1; /* resort na proxima rodada */
    printf("Metodo selecionado: %s\n", (metodo_atual==SORT_MERGE? "Merge" : "Durmam"));
    sp(); hr(); sp();
}

/* ---------- Fluxos ---------- */
static void loop_preparo(void) {
    for (;;) {
        int len = iniciativas_get_tamanho(combate);

        if (len == 0) {
            printf("\n"); hr(); printf(" Combate iniciado - Preparacao\n"); hr(); printf("\n");
            printf("1 - Inserir UM personagem no combate\n");
            printf("2 - Inserir TODOS os personagens no combate\n");
            printf("4 - Escolher metodo de ordenacao (atual: %s)\n", (metodo_atual==SORT_MERGE?"Merge":"Durmam"));
            printf("0 - Cancelar combate\n\nEscolha: ");
            int op; read_int("", &op);
            if (op == 0) { encerrar_combate(); return; }
            if (op == 1) { adicionar_persona_combate_sorted(combate); continue; }
            if (op == 2) {
                for (int i = 0; i < banco.length; i++)
                    if (banco.em_uso[i] == 0 && banco.salvos[i])
                        if (iniciativas_add_persona_sorted(combate, banco.salvos[i]) == SUCESSO) banco.em_uso[i] = 1;
                int after = iniciativas_get_tamanho(combate);
                if (after >= 2) { printf("Todos inseridos. Iniciando GUERRA!\n"); sp(); hr(); sp(); guerra_ativa = 1; rodada_nova = 1; return; }
                else { printf("E necessario pelo menos 2 personagens.\n"); sp(); continue; }
            }
            if (op == 4) { escolher_metodo(); continue; }
        } else {
            printf("\n"); hr(); printf(" Combate iniciado - Preparacao\n"); hr(); printf("\n");
            printf("1 - Inserir UM personagem no combate\n");
            printf("3 - Remover personagem pelo indice\n");
            if (len >= 2) printf("4 - reAlizar GUEERAAAA\n");
            printf("5 - Escolher metodo de ordenacao (atual: %s)\n", (metodo_atual==SORT_MERGE?"Merge":"Durmam"));
            printf("0 - Cancelar combate\n\nEscolha: ");
            int op; read_int("", &op);
            if (op == 0) { encerrar_combate(); return; }
            if (op == 1) { adicionar_persona_combate_sorted(combate); continue; }
            if (op == 3) { iniciativas_print(combate); int idx; printf("Indice para remover: "); read_int("", &idx); remover_persona_combate_por_indice(combate, idx); continue; }
            if (op == 4 && len >= 2) { guerra_ativa = 1; rodada_nova = 1; return; }
            if (op == 5) { escolher_metodo(); continue; }
        }
    }
}

static void loop_turnos(void) {
    for (;;) {
        int n = iniciativas_get_tamanho(combate);
        if (n < 2) { printf("Guerra requer pelo menos 2 personagens. Voltando a preparacao.\n"); sp(); guerra_ativa = 0; return; }

        if (rodada_nova) {
            iniciativas_recalcular_e_ordenar(combate, metodo_atual);
            turno_idx = 0;
            rodada_nova = 0;
        } else if (turno_idx >= n) {
            turno_idx = n - 1;
        }

        title("GUERRA");
        printf("Metodo atual: %s\n\n", (metodo_atual==SORT_MERGE?"Merge":"Durmam"));
        listar_iniciativa_com_turno();

        printf("1 - Proximo turno\n");
        printf("2 - Adicionar UM personagem\n");
        printf("3 - Remover personagem pelo indice\n");
        printf("5 - Trocar metodo de ordenacao\n");
        printf("0 - Encerrar combate\n\nEscolha: ");

        int op; read_int("", &op);
        if (op == 0) { encerrar_combate(); return; }

        if (op == 1) {
            turno_idx++;
            if (turno_idx >= iniciativas_get_tamanho(combate)) {
                rodada_nova = 1; /* nova rodada no próximo loop */
            }
            continue;
        }
        if (op == 2) { adicionar_persona_combate_sorted(combate); continue; }
        if (op == 3) { iniciativas_print(combate); int idx; printf("Indice para remover: "); read_int("", &idx); remover_persona_combate_por_indice(combate, idx); continue; }
        if (op == 5) { escolher_metodo(); continue; }
    }
}

/* ---------- Loop principal ---------- */
int main(void) {
    config();

    for (;;) {
        if (combate) { if (!guerra_ativa) { loop_preparo(); } else { loop_turnos(); } continue; }

        if (banco.length == 0) {
            printf("\n"); hr(); printf(" SISTEMA DE INICIATIVA RPG\n"); hr(); printf("\n");
            printf("1 - Criar personagem\n");
            printf("0 - Sair\n\nEscolha: ");
            int op; read_int("", &op);
            if (op == 0) break;
            if (op == 1) criar_personagem();
            continue;
        }

        if (banco.length == 1) {
            printf("\n1 - Criar personagem\n");
            printf("2 - Apagar personagem\n");
            printf("0 - Sair\n\nEscolha: ");
            int op; read_int("", &op);
            if (op == 0) break;
            if (op == 1) criar_personagem();
            if (op == 2) apagar_personagem();
            continue;
        }

        printf("\n1 - Criar personagem\n");
        printf("2 - Apagar personagem\n");
        printf("3 - Iniciar combate\n");
        printf("0 - Sair\n\nEscolha: ");
        int op; read_int("", &op);
        if (op == 0) break;
        if (op == 1) criar_personagem();
        if (op == 2) apagar_personagem();
        if (op == 3) {
            if (iniciar_combate() == SUCESSO) { sp(); printf("Combate iniciado!\n"); sp(); hr(); sp(); }
            else { sp(); printf("Falha ao iniciar combate.\n"); sp(); }
        }
    }

    title("Encerrado");
    return 0;
}
