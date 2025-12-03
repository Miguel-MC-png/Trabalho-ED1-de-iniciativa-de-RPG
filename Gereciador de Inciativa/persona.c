#include "persona.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void persona_config()
	{
		srand(time(NULL));
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
				printf("Error: nome vazio.\n");
				invalido = 1;
			}
			
			int tam = strlen(nome);
			if ( tam > TAM_MAX_NOME)
			{
				printf("Error: nome muito grande - %d.\n", tam);
				invalido = 1;
			}
				
			if (nvl < 0 || nvl > 20)
			{
				printf("Error: alem do limite no nivel.\n");
				invalido = 1;
			}
				
			if (dado_iniciativa == D0)
			{
				printf("Error: dado invalido.\n");
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
	int length;
}Iniciativas;

	int verificar_validade(Iniciativas *gerenciador)
		{
			if (gerenciador == NULL)
			{
				printf("Error: gerenciador de iniciativas nulo.\n");
				return CHAVE_INVALIDA;
			}

			if (gerenciador->length < 0)
			{
				printf("Error: gerenciador de iniciativas com tamanho negativo.\n");
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

		    // Libera todos os personagens
		    for (int i = 0; i < (*gerenciador)->length; i++)
		    {
		        if ((*gerenciador)->personagens[i] != NULL)
		        {
		            persona_destroy(&((*gerenciador)->personagens[i]));
		        }
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
				return FALHA;
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
		        return FALHA;
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
		        return FALHA;
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

	int iniciativa_remove_persona(Iniciativas * gerenciador, int endereco)
		{
			if (verificar_validade(gerenciador) != SUCESSO)
				return CHAVE_INVALIDA;

			int endereco_valido = verificar_endereco(gerenciador, endereco);
			if(endereco_valido != SUCESSO)
				return endereco_valido;

			int antepelutimo = gerenciador->length - 1;
			for(int i = endereco - 1; i < antepelutimo; i++)
			{
				gerenciador->personagens[i] =  gerenciador->personagens[i + 1];
			}

			gerenciador->personagens[gerenciador->length - 1] = NULL;
			gerenciador->length--;

			 return SUCESSO;
		}