#include <menu.h>
#include <player.h>
#include <game.h>
#include <client.h>

typedef enum opcao
{
    jogo_local = 1, jogo_rede = 2, alterar = 3, sair = 4
}Opcao;

int main(){

    Opcao op;	//Variavel que define a escolha do usu�rio
	int numberOfPlayers;
    int socket;
	int login_server_port = 8080;
	int game_server_port;

    do
    {
        ShowMenu();
        if(scanf("%d", (int*)&op) != 1)
        {
            printf("Erro ao ler entrada do teclado. \n");
        }
        
        clear_stdin();

        switch(op)
        {
            case jogo_local:
                numberOfPlayers = GetNumberOfPlayers();
                StartGame(numberOfPlayers);
                break;
            case jogo_rede:
                srand(time(0));
                socket = create_client_socket();
                game_server_port = login(socket, "127.0.0.1", login_server_port);
                if (game_server_port == 0)
                {
                    printf("Erro ao conectar ao servidor.\n");
                }
                else
                {
                    printf("porta de jogo: %d\n", game_server_port);
                    play_tictactoe(socket, "127.0.0.1", game_server_port);
                }                
                break;
            case alterar:
                numberOfPlayers = GetNumberOfPlayers();
                break;                
            case sair:
                printf("Encerrando...\n");
                break;
        }
    }while(op != sair);
    
    return 0;
}