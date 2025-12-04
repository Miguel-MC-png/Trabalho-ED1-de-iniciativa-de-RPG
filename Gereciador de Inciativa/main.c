#include "persona.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define TAM_MAX_SALVOS 1000000

typedef struct simulacao_banco_de_dados {
    Persona* salvos[TAM_MAX_SALVOS];
    int em_uso[TAM_MAX_SALVOS];
    int length;
} Banco_de_dados;

static Banco_de_dados banco;
static Iniciativas* combate = NULL;
static int guerra_ativa = 0; // 0=preparo; 1=turnos
static int turno_idx = 0;

/* ---------- Helpers visuais ---------- */
static void hr(void) { printf("====================================\n"); }
static void sp(void) { printf("\n"); }
static void title(const char* t) { sp(); hr(); printf(" %s\n", t); hr(); sp(); }
static void section(const char* t) { sp(); printf("-- %s --\n", t); sp(); }

/* ---------- I/O ---------- */
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
void config(void) { banco.length = 0; persona_config(); }
int buscar_persona(char* nome) {
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

/* ---------- Turno: helpers para manter quem joga ---------- */
static Persona* turno_guardar_atual(void) {
    int n = combate ? iniciativas_get_tamanho(combate) : 0;
    if (!combate || n <= 0) return NULL;
    return iniciativas_get_persona(combate, turno_idx + 1);
}
static void turno_realocar(Persona* atual) {
    int n = combate ? iniciativas_get_tamanho(combate) : 0;
    if (!combate || n <= 0 || !atual) { turno_idx = 0; return; }
    for (int i = 1; i <= n; i++) {
        if (iniciativas_get_persona(combate, i) == atual) {
            turno_idx = i - 1;
            return;
        }
    }
    // se o atual foi removido, normaliza para inicio
    if (turno_idx >= n) turno_idx = 0;
}

/* ---------- CRUD de personagens ---------- */
int criar_personagem(void) {
    if (banco.length >= TAM_MAX_SALVOS) { printf("Nao ha como salvar mais. ENTUPIU!\n"); sp(); return FALHA; }
    char novo_nome[TAM_MAX_NOME]; int novo_nlv=0; int novo_dado = D0;

    section("Criar personagem");
    perguntar_nome(novo_nome);
    if (buscar_persona(novo_nome) != FALHA) { printf("Esse personagem ja existe e nao sera salvo.\n"); sp(); return FALHA; }

    do { read_int("Nivel do personagem (0-20): ", &novo_nlv); } while (novo_nlv < 0 || novo_nlv > 20);
    do {
        read_int("Dado de iniciativa (4/6/8/10/12/20): ", &novo_dado);
        switch (novo_dado) { case 4: case 6: case 8: case 10: case 12: case 20: break; default: novo_dado = D0; }
    } while (novo_dado == D0);

    Persona* novo = persona_create(novo_nome, novo_nlv, (Dado)novo_dado);
    if (!novo) { printf("Falha ao criar personagem!\n"); sp(); return FALHA; }

    banco.salvos[banco.length] = novo;
    banco.em_uso[banco.length] = 0;
    banco.length++;

    sp(); printf("Personagem salvo:\n");
    persona_print(novo, 3);
    sp(); hr(); sp();
    return SUCESSO;
}
char* apagar_personagem(void) {
    static char nome[TAM_MAX_NOME];
    section("Apagar personagem");
    perguntar_nome(nome);
    int end = buscar_persona(nome);
    if (end == FALHA) { printf("Personagem '%s' nao esta salvo.\n", nome); sp(); return NULL; }
    if (banco.em_uso[end] == 1) { printf("Personagem '%s' esta em combate e nao pode ser removido.\n", nome); sp(); return NULL; }

    for (int i = end; i < banco.length - 1; i++) {
        banco.salvos[i] = banco.salvos[i + 1]; banco.em_uso[i] = banco.em_uso[i + 1];
    }
    banco.salvos[banco.length - 1] = NULL; banco.em_uso[banco.length - 1] = 0; banco.length--;

    printf("Personagem '%s' foi removido do banco de dados.\n", nome);
    sp(); hr(); sp();
    return nome;
}

/* ---------- Combate util ---------- */
static void listar_iniciativa_com_turno(void) {
    int n = iniciativas_get_tamanho(combate);
    if (n <= 0) { printf("Iniciativa vazia.\n"); return; }
    section("Iniciativa");
    for (int i = 1; i <= n; i++) {
        Persona *p = iniciativas_get_persona(combate, i);
        printf("%s %2d) %s\n", (i-1)==turno_idx ? "=>" : "  ", i, p->nome);
    }
    sp();
    Persona *atual = iniciativas_get_persona(combate, turno_idx + 1);
    printf("Jogando agora: [%d] %s\n", turno_idx + 1, atual ? atual->nome : "(nenhum)");
    sp();
}

/* inserir (ordenado) preservando quem esta jogando; mostra posicao 1-based */
static int adicionar_persona_combate_sorted(Iniciativas* g) {
    if (banco.length == 0) { printf("Nenhum personagem salvo.\n"); sp(); return FALHA; }

    char nome[TAM_MAX_NOME];
    section("Adicionar ao combate");
    perguntar_nome(nome);

    int end = buscar_persona(nome);
    if (end == FALHA) { printf("Personagem '%s' nao foi encontrado no banco.\n", nome); sp(); return FALHA; }
    if (banco.em_uso[end] != 0) { printf("Personagem '%s' ja esta na iniciativa.\n", nome); sp(); return FALHA; }

    Persona* atual = turno_guardar_atual();        // <-- guarda quem estava jogando
    Persona* p = banco.salvos[end];
    if (iniciativas_add_persona_sorted(g, p) == SUCESSO) {
        banco.em_uso[end] = 1;
        int n = iniciativas_get_tamanho(g), pos = -1;
        for (int i = 1; i <= n; i++) { if (iniciativas_get_persona(g, i) == p) { pos = i; break; } }
        if (pos < 0) pos = n;
        printf("'%s' foi adicionado a iniciativa na posicao %d.\n", nome, pos);
        turno_realocar(atual);                     // <-- realoca o indice do turno
        sp(); hr(); sp();
        return pos;
    }
    return FALHA;
}

/* remove por indice, atualiza em_uso e ajusta turno_idx de forma robusta */
static int remover_persona_combate_por_indice(Iniciativas* g, int indice1) {
    if (indice1 <= 0 || indice1 > iniciativas_get_tamanho(g)) return FALHA;

    Persona* atual = turno_guardar_atual();        // guarda quem estava jogando
    char *removido = iniciativa_remove_persona(g, indice1);
    if (!removido || !removido[0]) return FALHA;

    int end_banco = buscar_persona(removido);
    if (end_banco != FALHA) banco.em_uso[end_banco] = 0;

    turno_realocar(atual);                         // reancora o turno
    printf("Removido: %s\n", removido);
    sp(); hr(); sp();
    return SUCESSO;
}

static int iniciar_combate(void) {
    if (combate) return SUCESSO;
    combate = iniciativas_create();
    guerra_ativa = 0; turno_idx = 0;
    return combate ? SUCESSO : FALHA;
}
static int encerrar_combate(void) {
    int r = iniciativas_destroy(&combate);
    if (r == SUCESSO) {
        for (int i = 0; i < banco.length; i++) banco.em_uso[i] = 0;
        combate = NULL; guerra_ativa = 0; turno_idx = 0;
        printf("Combate encerrado.\n"); sp(); hr(); sp();
    }
    return r;
}

/* ---------- Menus ---------- */
typedef struct { int key; const char* label; } MenuItem;
static void menu_print(const char* titulo, const MenuItem* itens, size_t n) {
    if (titulo && *titulo) title(titulo);
    for (size_t i = 0; i < n; i++) printf("%d - %s\n", itens[i].key, itens[i].label);
    sp(); printf("Escolha: ");
}
static int menu_read_choice(const MenuItem* itens, size_t n) {
    int op;
    for (;;) {
        if (!read_int("", &op)) return -1;
        for (size_t i = 0; i < n; i++) if (itens[i].key == op) { sp(); return op; }
        printf("Opcao invalida. Tente novamente: ");
    }
}

/* ---------- Fluxo: Preparo ---------- */
static void loop_preparo(void) {
    for (;;) {
        int len = iniciativas_get_tamanho(combate);

        if (len == 0) {
            const MenuItem itens[] = {
                {1, "Inserir UM personagem no combate"},
                {2, "Inserir TODOS os personagens no combate"},
                {0, "Cancelar combate"}
            };
            menu_print("Combate iniciado - Preparacao", itens, 3);
            int op = menu_read_choice(itens, 3);
            if (op == 0) { encerrar_combate(); return; }
            if (op == 1) { adicionar_persona_combate_sorted(combate); continue; }
            if (op == 2) {
                section("Inserir TODOS");
                Persona* atual = turno_guardar_atual();
                for (int i = 0; i < banco.length; i++) {
                    if (banco.em_uso[i] == 0) {
                        if (iniciativas_add_persona_sorted(combate, banco.salvos[i]) == SUCESSO)
                            banco.em_uso[i] = 1;
                    }
                }
                turno_realocar(atual);
                int after = iniciativas_get_tamanho(combate);
                if (after >= 2) { printf("Todos inseridos. Iniciando GUERRA!\n"); sp(); hr(); sp(); guerra_ativa = 1; return; }
                else { printf("E necessario pelo menos 2 personagens para comecar a guerra.\n"); sp(); continue; }
            }
        } else {
            MenuItem itens[4]; size_t n = 0;
            itens[n++] = (MenuItem){1, "Inserir UM personagem no combate"};
            itens[n++] = (MenuItem){3, "Remover personagem pelo indice"};
            if (len >= 2) itens[n++] = (MenuItem){4, "reAlizar GUEERAAAA"};
            itens[n++] = (MenuItem){0, "Cancelar combate"};

            menu_print("Combate iniciado - Preparacao", itens, n);
            int op = menu_read_choice(itens, n);
            if (op == 0) { encerrar_combate(); return; }
            if (op == 1) { adicionar_persona_combate_sorted(combate); continue; }
            if (op == 3) {
                listar_iniciativa_com_turno();
                int idx; read_int("Indice para remover: ", &idx);
                remover_persona_combate_por_indice(combate, idx);
                continue;
            }
            if (op == 4) { guerra_ativa = 1; return; } // len>=2
        }
    }
}

/* ---------- Fluxo: Turnos ---------- */
static void listar_iniciativa_com_turno(void); // forward (ja definido acima)

static void loop_turnos(void) {
    for (;;) {
        int n = iniciativas_get_tamanho(combate);
        if (n < 2) { printf("Guerra requer pelo menos 2 personagens. Voltando a preparacao.\n"); sp(); guerra_ativa = 0; return; }

        title("GUERRA");
        listar_iniciativa_com_turno();

        const MenuItem itens[] = {
            {1, "Proximo turno"},
            {2, "Adicionar UM personagem"},
            {3, "Remover personagem pelo indice"},
            {0, "Encerrar combate"}
        };
        menu_print(NULL, itens, 4);
        int op = menu_read_choice(itens, 4);

        if (op == 0) { encerrar_combate(); return; }
        if (op == 1) { turno_idx = (turno_idx + 1) % iniciativas_get_tamanho(combate); continue; }
        if (op == 2) { adicionar_persona_combate_sorted(combate); continue; }
        if (op == 3) {
            listar_iniciativa_com_turno();
            int idx; read_int("Indice para remover: ", &idx);
            remover_persona_combate_por_indice(combate, idx);
            continue;
        }
    }
}

/* ---------- Loop Principal ---------- */
static void loop_principal(void) {
    for (;;) {
        if (combate) { if (!guerra_ativa) loop_preparo(); else loop_turnos(); continue; }

        if (banco.length == 0) {
            const MenuItem itens[] = { {1,"Criar personagem"}, {0,"Sair"} };
            menu_print("SISTEMA DE INICIATIVA RPG", itens, 2);
            int op = menu_read_choice(itens, 2);
            if (op == 0) break; if (op == 1) criar_personagem(); continue;
        }
        if (banco.length == 1) {
            const MenuItem itens[] = { {1,"Criar personagem"}, {2,"Apagar personagem"}, {0,"Sair"} };
            menu_print("SISTEMA DE INICIATIVA RPG", itens, 3);
            int op = menu_read_choice(itens, 3);
            if (op == 0) break; if (op == 1) criar_personagem(); if (op == 2) apagar_personagem(); continue;
        }
        {
            const MenuItem itens[] = { {1,"Criar personagem"}, {2,"Apagar personagem"}, {3,"Iniciar combate"}, {0,"Sair"} };
            menu_print("SISTEMA DE INICIATIVA RPG", itens, 4);
            int op = menu_read_choice(itens, 4);
            if (op == 0) break;
            if (op == 1) criar_personagem();
            if (op == 2) apagar_personagem();
            if (op == 3) {
                if (iniciar_combate() == SUCESSO) { sp(); printf("Combate iniciado!\n"); sp(); hr(); sp(); }
                else { sp(); printf("Falha ao iniciar combate.\n"); sp(); }
            }
        }
    }
}

int main(void) {
    config();
    loop_principal();
    title("Encerrado");
    return 0;
}