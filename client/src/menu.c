#include <Menu.h>
#include <client.h>

//Mostra o menu
void ShowMenu()
{
	if(system("clear")==0)
	{
		printf("Menu");
		printf("\n1 - Jogar");
		printf("\n2 - Jogar em lan");
		printf("\n3 - Alterar numero de jogadores");
		printf("\n4 - Sair\n");
	}
}

//Chama a fun��o escolhida pelo jogador
void MenuChoice(int choice, int *numberOfPlayers)
{
	int socket;
	int login_server_port = 8080;
	int game_server_port;

	switch(choice)
	{
		case 1:
			StartGame(*numberOfPlayers);
			break;
		case 2:
			srand(time(0));
			socket = create_client_socket();
			game_server_port = login(socket, "127.0.0.1", login_server_port);

			printf("porta de jogo: %d\n", game_server_port);

			play_tictactoe(socket, "127.0.0.1", game_server_port);
			break;
		case 3:
			*numberOfPlayers = GetNumberOfPlayers();
			break;
		case 4:
			exit(0);
			break;
	}
}