#ifndef PERSONA_H
#define PERSONA_H

#define SUCESSO 1
#define FALHA -1
#define CHAVE_INVALIDA 0


#define TAM_MAX_NOME 50
#define TAM_MAX_INICIATIVAS 50



void persona_cofig();
	//Cofigurar qlr coisa que executar esse código.

typedef enum dado
{
	D0,
	D4 = 4,
	D6 = 6,
	D8 = 8,
	D10 = 10,
	D12 = 12,
	D20 = 20
} Dado;

	int rolar_dado(Dado dado);

typedef struct personagem
{
	char nome[TAM_MAX_NOME];
	int nvl;
	Dado dado_iniciativa;
	int iniciativa_atual; 
}Persona;


	extern Persona vazio;

	Persona* persona_create(char *nome, int nvl, Dado dado_iniciativa);

	int persona_print(Persona *personagem,  int opcao);
		// Opção 1: ler só o nome e a iniciativa_atual do personagem.
		// Opção 2: lê, tbm, o nível.
		// Opção 3: lê tudo do personagem incluindo o seu "dado_iniciativa".
		// Qlr outra opção vai ler só o nome.

	int persona_destroy(Persona **personagem);

typedef struct iniciativas Iniciativas;
	

	Iniciativas* iniciativas_create();

	int iniciativas_get_tamanho(Iniciativas * gerenciador);

	Persona* iniciativas_get_persona(Iniciativas * gerenciador, int endereco);

	int iniciativas_inserir_persona_unsorted(Iniciativas *gerenciador, Persona* novo);
		//Partir do pressuposto que está desordenado

	//int iniciativas_inserir_persona_sorted(Iniciativas *gerenciador, Persona* novo);
		//Partir do pressuposto que está ordenado, buscado pela busca binária

	int iniciativas_print(Iniciativas *gerenciador);

	int iniciativas_print_persona(Iniciativas *gerenciador, int endereco);

	int iniciativas_destroy(Iniciativas **gerenciador);

#endif