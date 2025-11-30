#include "persona.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


int rolar_dado(Dado dado)
	{
		if (dado == D0)
		{
			return CHAVE_INVALIDA;
		}
		srand(time(NULL));
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
			if ( tam > TAM_MAX_NOME )
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
				*personagem = vazio;
				strcpy(personagem->nome, nome);
				personagem->nvl = nvl;
				personagem->dado_iniciativa = dado_iniciativa;
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

				case 2:
					sprintf(nivel, "   Nivel: %d\n", personagem->nvl);

				case 1:
					sprintf(iniciativa, "   Iniciativa: %d\n", personagem->iniciativa_atual);

				default:
					sprintf(nome, "Nome: %s\n", personagem->nome);
			}
			printf("%s%s%s%s", nome, iniciativa, nivel, dado);
			return SUCESSO;
		}

typedef struct iniciativas
{
	Persona * personagens[TAM_MAX_INICIATIVAS];
	int length;
}Iniciativas;

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
			if (gerenciador == NULL)
			{
				printf("Iniciativas nulo.\n");
				return CHAVE_INVALIDA;
			}

			return gerenciador->length;
		}

	Persona* iniciativas_get_persona(Iniciativas * gerenciador, int endereco)
		{
			if (gerenciador == NULL)
			{
				printf("Iniciativas nulo.\n");
				return NULL;
			}

			if (endereco >= gerenciador->length)
			{
				printf("Endereco maior que o gerenciador de iniciativas.\n");
				return NULL;
			}

			if (endereco < 0)
			{
				printf("Endereco negativo.\n");
				return NULL;
			}

			return gerenciador->personagens[endereco];
		}

	int iniciativas_print(Iniciativas *gerenciador)
		{
			if (gerenciador == NULL)
			{
				printf("Error: gerencioador de iniciativas nulo.\n");
				return CHAVE_INVALIDA;
			}

			for (int i = 0; i < gerenciador->length; i++)
			{
				printf("%d) ", i + 1);
				persona_print(gerenciador->personagens[i], 1);
			}

			return SUCESSO;
		}

	int iniciativas_print_persona(Iniciativas *gerenciador, int endereco)
		{
			if (gerenciador == NULL)
			{
				printf("Error: gerencioador de iniciativas nulo.\n");
				return CHAVE_INVALIDA;
			}

			if (endereco >= gerenciador->length)
			{
				printf("Endereco maior que o historico.\n");
				return FALHA;
			}

			if (endereco < 0)
			{
				printf("Endereco negativo.\n");
				return FALHA;
			}

			persona_print(gerenciador->personagens[endereco], 3);
			return SUCESSO;
		}

	int iniciativas_inserir_persona_unsorted(Iniciativas *gerenciador, Persona* novo)
		{
		    if (gerenciador == NULL)
		    {
		        printf("Error: gerenciador nulo.\n");
		        return CHAVE_INVALIDA;
		    }
		    
		    if (novo == NULL)
		    {
		        printf("Error: personagem nulo.\n");
		        return CHAVE_INVALIDA;
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


		
	/* Isto só serve caso queira copiar um método antigo meu...
		int iniciativas_push(Iniciativas *gerenciador, Persona* personagem)
		{
			if (gerenciador == NULL)
			{
				return CHAVE_INVALIDA;
			}

			if (gerenciador->length < 0)
			{
				printf("Tamanho do gerenciador de iniciativas negativo.");
				return CHAVE_INVALIDA;
			}

			for ( int i = (gerenciador->length - 1); i >= 0; i-- )
			{
				if (i + 1 == TAM_MAX_INICIATIVAS)
					continue;

				gerenciador->personagens[i + 1] = gerenciador->personagens[i];
			}

			gerenciador->personagens[0] = personagem;
			gerenciador->length++;
			return SUCESSO;
		}

		int iniciativas_pop(Iniciativas *gerenciador);
		//N está pronto
		{
			if (gerenciador == NULL)
			{
				return CHAVE_INVALIDA;
			}

			if (gerenciador->length < 0)
			{
				printf("Tamanho do gerenciador de iniciativas negativo.");
				return CHAVE_INVALIDA;
			}

			for ( int i = 0; i < gerenciador->length; i++)
			{

				gerenciador->personagens[i] = gerenciador->personagens[i + 1];
			}

			gerenciador->length--;
			return SUCESSO;
		}
	*/