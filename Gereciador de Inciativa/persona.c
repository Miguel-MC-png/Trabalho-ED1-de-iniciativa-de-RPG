#include "persona.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void persona_config(void) 
	{
    	srand((unsigned)time(NULL));
	}

int rolar_dado(Dado dado)
	{
		if (dado == D0)
		{
			return CHAVE_INVALIDA;
		}
		return rand() % dado + 1;
	}

Persona vazio = {"", 0, D0, 0};
	
	Persona* persona_create(char *nome, int nvl, Dado dado_iniciativa)
		{
			int invalido = 0;
			
			if (nome[0] == '\0')
			{
				printf("Nome vazio.\n");
				invalido = 1;
			}
			
			int tam = strlen(nome);
			if ( tam > TAM_MAX_NOME)
			{
				printf("Nome muito grande - %d.\n", tam);
				invalido = 1;
			}
				
			if (nvl < 0 || nvl > 20)
			{
				printf("Alem do limite no nivel.\n");
				invalido = 1;
			}
				
			if (dado_iniciativa == D0)
			{
				printf("Dado invalido.\n");
				invalido = 1;
			}
			
			if (invalido)
				return NULL;
					
			
			Persona * personagem = (Persona*) malloc(sizeof(Persona));
			
			if (personagem != NULL)
			{
				strncpy(personagem->nome, nome, TAM_MAX_NOME - 1);
        		personagem->nome[TAM_MAX_NOME - 1] = '\0';

				personagem->nvl = nvl;
				personagem->dado_iniciativa = dado_iniciativa;
				personagem->iniciativa_atual = 0;
			}
			else
			{
				printf("Error: sem espaco na memoria!!!\n");
			}
			
			return personagem;
		}

	int persona_destroy(Persona **personagem)
		{
			if (personagem == NULL || *personagem == NULL)
			{
				printf("Error: ponteiro nulo\n");
				return CHAVE_INVALIDA;
			}

			free(*personagem);
			*personagem = NULL; 

			return SUCESSO;
		}

	int persona_print(Persona *personagem, int opcao)
	// Opção 1: ler só o nome e a iniciativa_atual do personagem.
	// Opção 2: lê, tbm, o nível.
	// Opção 3: lê tudo do personagem incluindo o seu "dado_iniciativa".
	// Qlr outra opção vai ler só o nome.
		{
			if (personagem == NULL)
			{
				printf("Error: ponteiro nulo\n");
				return CHAVE_INVALIDA;
			}

			if(personagem->dado_iniciativa == D0)
			{
				printf("Falha: não é possível ler personagem vazio.\n");
				return FALHA;
			}

			char nome[TAM_MAX_NOME + 11] = "";
			char iniciativa[30] = "";
			char nivel[30] = "";
			char dado[29] = "";
			switch (opcao)
			{
				case 3:
					sprintf(dado, "   Dado de iniciativa: D%d\n", personagem->dado_iniciativa);
					/* fallthrough */

				case 2:
					sprintf(nivel, "   Nivel: %d\n", personagem->nvl);
					/* fallthrough */

				case 1:
					sprintf(iniciativa, "   Iniciativa: %d\n", personagem->iniciativa_atual);
					/* fallthrough */

				default:
					sprintf(nome, "Nome: %s\n", personagem->nome);
			}
			printf("%s%s%s%s", nome, iniciativa, nivel, dado);
			return SUCESSO;
		}

	int persona_rolar_iniciativa(Persona *personagem)
		{
			if (personagem == NULL)
			{
				printf("Error: ponteiro nulo\n");
				return CHAVE_INVALIDA;
			}

			personagem->iniciativa_atual = rolar_dado(personagem->dado_iniciativa) + personagem->nvl;
			return personagem->iniciativa_atual;
		}


typedef struct iniciativas
{
	Persona * personagens[TAM_MAX_INICIATIVAS];
	unsigned int length;
}Iniciativas;

	int verificar_validade(Iniciativas *gerenciador)
		{
			if (gerenciador == NULL)
			{
				printf("Error: gerenciador de iniciativas nulo.\n");
				return CHAVE_INVALIDA;
			}

			return SUCESSO;
		}

	int verificar_endereco(Iniciativas *gerenciador, int endereco)
		{
			if (endereco <= 0)
			{
			    printf("Endereco negativo ou nulo.\n");
			    return FALHA;
			}

			if (endereco > gerenciador->length)
			{
				printf("Endereco muito grande.\n");
			    return FALHA;
			}

			return SUCESSO;
		}

	Iniciativas* iniciativas_create()
		{
			Iniciativas * gerenciador = calloc(1, sizeof(Iniciativas));
			
			if (gerenciador == NULL)
			{
				printf("Sem espaço de memória!!!\n");
				return NULL;
			}

			gerenciador->length = 0;
			
			return gerenciador;
		}

	int iniciativas_destroy(Iniciativas **gerenciador)
		{
		    if (gerenciador == NULL || *gerenciador == NULL)
		    {
		        printf("Error: ponteiro nulo.\n");
		        return CHAVE_INVALIDA;
		    }


		    // Destruição rasa: não liberar Personas (pertencem ao banco).
		    (*gerenciador)->length = 0;
		    for (int i = 0; i < TAM_MAX_INICIATIVAS; i++) {
		        (*gerenciador)->personagens[i] = NULL;
		    }
		    free(*gerenciador);
		    *gerenciador = NULL;
		    return SUCESSO;
		}

	int iniciativas_get_tamanho(Iniciativas * gerenciador)
		{
			if (verificar_validade(gerenciador) != SUCESSO)
				return CHAVE_INVALIDA;

			return gerenciador->length;
		}

	Persona* iniciativas_get_persona(Iniciativas * gerenciador, int endereco)
		{
			if (verificar_validade(gerenciador) != SUCESSO)
				return NULL;

			if(verificar_endereco(gerenciador, endereco) != SUCESSO)
				return NULL;

			return gerenciador->personagens[endereco - 1];
		}

	int iniciativas_print(Iniciativas *gerenciador)
		{
			if (verificar_validade(gerenciador) != SUCESSO)
				return CHAVE_INVALIDA;

			if (gerenciador->length == 0)
			{
				printf("Nao existe personagens no gerenciador de iniciativas.");
				return CHAVE_INVALIDA;
			}

			int contador = 1;
			for (int i = 0; i < gerenciador->length; i++)
			{
				printf("%d) ", contador);
				persona_print(gerenciador->personagens[i], 1);
				contador++;
			}

			return SUCESSO;
		}

	int iniciativas_print_persona(Iniciativas *gerenciador, int endereco)
		{
			if (verificar_validade(gerenciador) != SUCESSO)
				return CHAVE_INVALIDA;

			int endereco_valido = verificar_endereco(gerenciador, endereco);
			if(endereco_valido != SUCESSO)
				return endereco_valido;

			persona_print(gerenciador->personagens[endereco - 1], 3);
			return SUCESSO;
		}

	int iniciativas_add_persona_unsorted(Iniciativas *gerenciador, Persona* novo)
		{
		    if (verificar_validade(gerenciador) != SUCESSO)
				return CHAVE_INVALIDA;
		    
		    if (novo == NULL)
		    {
		        printf("Error: personagem nulo.\n");
		        return CHAVE_INVALIDA;
		    }

		    if (novo->dado_iniciativa == D0)
			{
				printf("Impossível adicionar personagem vazio.\n");
				return FALHA;
			}

		    if (gerenciador->length >= TAM_MAX_INICIATIVAS)
		    {
		        printf("Gerenciador de iniciativas cheio.\n");
		        return CHAVE_INVALIDA;
		    }

		    gerenciador->personagens[gerenciador->length] = novo;
		    gerenciador->length++;
		    
		    return SUCESSO;
		}

	int iniciativas_add_persona_sorted(Iniciativas *gerenciador, Persona* novo)
		{
			if (verificar_validade(gerenciador) != SUCESSO)
				return CHAVE_INVALIDA;

			if (novo == NULL)
			{
				printf("Error: personagem nulo.\n");
				return CHAVE_INVALIDA;
			}

			if (novo->dado_iniciativa == D0)
			{
				printf("Impossível adicionar personagem vazio.\n");
				return FALHA;
			}

			if (gerenciador->length >= TAM_MAX_INICIATIVAS)
		    {
		        printf("Gerenciador de iniciativas cheio.\n");
		        return CHAVE_INVALIDA;
		    }

		    persona_rolar_iniciativa(novo);

			int inicio, fim, meio;
			inicio = 0;
			fim = gerenciador->length - 1;

			//Ordem decrescente
			while (inicio <= fim)
			{
				meio = (inicio + fim)/2;
				if (novo->iniciativa_atual < gerenciador->personagens[meio]->iniciativa_atual)
				{
					inicio = meio + 1;
				}
				else if (novo->iniciativa_atual > gerenciador->personagens[meio]->iniciativa_atual)
				{
					fim = meio - 1;
				}
				else
				{
					inicio = meio + 1;
					break;
				}

			}

			for (int i = gerenciador->length - 1; i >= inicio; i--)
				gerenciador->personagens[i + 1] = gerenciador->personagens[i];

			gerenciador->personagens[inicio] = novo;
			gerenciador->length++;
			return SUCESSO;
		}

	char* iniciativa_remove_persona(Iniciativas * gerenciador, int endereco)
		{
			static char buffer[TAM_MAX_NOME];

			if (verificar_validade(gerenciador) != SUCESSO)
				return NULL;

			int endereco_valido = verificar_endereco(gerenciador, endereco);
			if(endereco_valido != SUCESSO)
			{
				strcpy(buffer, "");
			}
			else
			{
				strcpy(buffer,gerenciador->personagens[endereco - 1]->nome);

				int antepelutimo = gerenciador->length - 1;
				for(int i = endereco - 1; i < antepelutimo; i++)
				{
					gerenciador->personagens[i] =  gerenciador->personagens[i + 1];
				}
				gerenciador->personagens[gerenciador->length - 1] = NULL;
				gerenciador->length--;
			}

			return buffer;
		}


	/* ===== Ordenação ===== */
static void iniciativas_recalcular(Iniciativas* g) {
    if (!g) return;
    for (unsigned int i = 0; i < g->length; i++) persona_rolar_iniciativa(g->personagens[i]);
}

/* Merge estável decrescente */
static void merge_por_ptr(Persona** arr, Persona** tmp, int l, int m, int r) {
    int i=l, j=m+1, k=l;
    while (i<=m && j<=r) {
        if (arr[i]->iniciativa_atual >= arr[j]->iniciativa_atual) tmp[k++] = arr[i++];
        else                                                      tmp[k++] = arr[j++];
    }
    while (i<=m) tmp[k++] = arr[i++];
    while (j<=r) tmp[k++] = arr[j++];
    for (int t=l; t<=r; t++) arr[t] = tmp[t];
}
static void msort_ptr(Persona** arr, Persona** tmp, int l, int r) {
    if (l>=r) return;
    int m = l + (r-l)/2;
    msort_ptr(arr,tmp,l,m);
    msort_ptr(arr,tmp,m+1,r);
    if (arr[m]->iniciativa_atual >= arr[m+1]->iniciativa_atual) return;
    merge_por_ptr(arr,tmp,l,m,r);
}
int iniciativas_ordenar_merge(Iniciativas* g) {
    if (verificar_validade(g) != SUCESSO) return CHAVE_INVALIDA;
    if (g->length <= 1) return SUCESSO;
    Persona** tmp = (Persona**)malloc(g->length * sizeof(Persona*));
    if (!tmp) return FALHA;
    msort_ptr(g->personagens, tmp, 0, (int)g->length - 1);
    free(tmp);
    return SUCESSO;
}

/* Durmam (counting) estável decrescente */
int iniciativas_ordenar_durmam(Iniciativas* g) {
    if (verificar_validade(g) != SUCESSO) return CHAVE_INVALIDA;
    if (g->length <= 1) return SUCESSO;

    int minv = g->personagens[0]->iniciativa_atual;
    int maxv = minv;
    for (unsigned int i=1;i<g->length;i++){
        if (g->personagens[i]->iniciativa_atual < minv) minv = g->personagens[i]->iniciativa_atual;
        if (g->personagens[i]->iniciativa_atual > maxv) maxv = g->personagens[i]->iniciativa_atual;
    }
    int R = maxv - minv + 1;
    if (R <= 0) return SUCESSO;

    int* count = (int*)calloc((size_t)R, sizeof(int));
    int* start = (int*)malloc((size_t)R * sizeof(int));
    Persona** out = (Persona**)malloc(g->length * sizeof(Persona*));
    if (!count || !start || !out) { free(count); free(start); free(out); return FALHA; }

    for (unsigned int i=0;i<g->length;i++) count[g->personagens[i]->iniciativa_atual - minv]++;

    int total = 0;
    for (int v = maxv; v >= minv; --v) {
        int idx = v - minv;
        start[idx] = total;
        total += count[idx];
    }
    for (unsigned int i=0;i<g->length;i++) {
        int val = g->personagens[i]->iniciativa_atual - minv;
        int pos = start[val]++;
        out[pos] = g->personagens[i];
    }
    for (unsigned int i=0;i<g->length;i++) g->personagens[i] = out[i];

    free(count); free(start); free(out);
    return SUCESSO;
}

int iniciativas_recalcular_e_ordenar(Iniciativas* g, SortMethod metodo) {
    if (verificar_validade(g) != SUCESSO) return CHAVE_INVALIDA;
    iniciativas_recalcular(g);
    if (metodo == SORT_MERGE) return iniciativas_ordenar_merge(g);
    else                      return iniciativas_ordenar_durmam(g);
}