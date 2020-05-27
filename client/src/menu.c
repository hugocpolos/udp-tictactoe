#include <Menu.h>

//Mostra o menu
void ShowMenu()
{
	int choice = 0;	//Variavel que define a escolha do usuario
	int numberOfPlayers = GetNumberOfPlayers();	//Define o numero de jogadores participantes
	if(numberOfPlayers == 0)
	{
		printf("Por favor insira um numero valido de jogadores (1 ou 2).\n");
		ShowMenu();
	}

	getchar();
	printf("Menu");
	printf("\n1 - Jogar");
	printf("\n2 - Alterar numero de jogadores");
	printf("\n3 - Sair\n");
	
	if(scanf("%d", &choice)==1)
	{
		switch(choice)
		{
			case 1:
				StartGame(numberOfPlayers);
				break;
			case 2:
				numberOfPlayers = GetNumberOfPlayers();
				break;
			case 3:
				exit(0);
				break;
			default:
				printf("Escolha uma opcao valida.\n");
				ShowMenu();
				break;
		}
	}
}
