#include "persona.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int rolar_dado(Dado dado)
{
	return 0;
}

Persona vazio = {"", 0, D0, 0};
	
	Persona* persona_create(char *nome, int nvl, Dado dado_iniciativa)
	{
		int sucesso = 0;
		
		if (nome[0] == '\0')
		{
			printf("Error: nome vazio.\n");
			sucesso = 1;
		}
		
		int tam = strlen(nome);
		if ( tam > TAM_MAX_NOME )
		{
			printf("Error: nome muito grande - %d.\n", tam);
			sucesso = 1;
		}
			
		if (nvl < 0 || nvl > 20)
		{
			printf("Error: alem do limite no nivel.\n");
			sucesso = 1;
		}
			
		if (dado_iniciativa == D0)
		{
			printf("Error: dado invalido.\n");
			sucesso = 1;
		}
		
		if (sucesso)
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
			perror("Error: sem espaco na memoria!!!");
		}
		
		return personagem;
	}

	int persona_destroy(Persona **personagem)
	{
		if (personagem || *personagem)
		{
			printf("Error: ponteiro nulo");
			return CHAVE_INVALIDA;
		}
	}

	int persona_print(Persona *personagem)
	{
		return CHAVE_INVALIDA;
	}

typedef struct iniciativas
{
	Persona * personagens[TAM_MAX_INICIATIVAS];
	int length;
}Iniciativas;

	Iniciativas* Iniciativas_create()
	{
		Iniciativas * gerenciador = calloc(1, sizeof(Iniciativas));
		
		if (gerenciador == NULL)
		{
			printf("Sem espaço de memória!!!");
			return NULL;
		}

		gerenciador->length = 0;
		
		return gerenciador;
	}

	int iniciativas_get_tamanho(Inciativas * gerenciador)
	{
		if (gerenciador == NULL)
		{
			printf("Iniciativas nulo.");
			return CHAVE_INVALIDA;
		}

		return gerenciador->length;
	}

	Persona* iniciativas_get_persona(Inciativas * gerenciador, int endereco)
	{
		if (gerenciador == NULL)
		{
			printf("Iniciativas nulo.");
			return NULL;
		}

		if (endereco >= gerenciador->length)
		{
			printf("Endereco maior que o gerenciador de iniciativas.");
			return NULL;
		}

		return gerenciador->personagens[endereco];
	}

	/*int iniciativas_print(Iniciativas *gerenciador)
	{
		if (gerenciador == NULL)
		{
			printf("Iniciativas nulo.");
			return CHAVE_INVALIDA;
		}

		for (int i = 0; i < gerenciador->length; i++)
		{
			printf("%d) ", i );
			site_print(gerenciador->personagens[i]);
		}
	}*/

	/* Isto só serve caso queira copiar um método antigo meu...
		int iniciativas_push(Inciativas *gerenciador, Persona* personagem)
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

		int iniciativas_pop(Inciativas *gerenciador);
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

		int historico_print_site(Historico *historico, int endereco)
		{
			if (historico == NULL)
			{
				printf("Historico nulo.");
				return CHAVE_INVALIDA;
			}

			if (endereco >= historico->length)
			{
				printf("Endereco maior que o historico.");
				return FALHA;
			}

			site_print(historico->sites[endereco]);
		}
	*/
	
