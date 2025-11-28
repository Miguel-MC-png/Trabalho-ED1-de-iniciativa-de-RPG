#ifndef PERSONA_H
#define PERSONA_H

#define SUCESSO 1
#define FALHA -1
#define CHAVE_INVALIDA 0


#define TAM_MAX_INICIATIVAS 50

typedef enum dado
{
	D0 = 0,
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
	char nome[20];
	int nivel;
	Dado dado_iniciativa;
	int iniciativa_atual; 
}Persona;

	extern Persona vazio;

	Persona* persona_create(char *nome, int nivel, Dado dado_iniciativa);
	int persona_print(Persona *personagem);
	int persona_destroy(Persona **personagem);

typedef struct iniciativas Inciativas;
	
	Inciativas* iniciativas_create();
	int iniciativas_get_tamanho(Inciativas * gerenciador);
	Persona* iniciativas_get_persona(Inciativas * gerenciador, int endereco);
	int iniciativas_print(Inciativas *gerenciador);
	int iniciativas_destroy(Inciativas **gerenciador);

#endif