#include <menu.h>

int main(){

    int choice = 0;	//Vari�vel que define a escolha do usu�rio
	int numberOfPlayers;
    int socket;
	int login_server_port = 8080;
	int game_server_port;

    ShowMenu();

    do
    {
        if(scanf("%d", &choice) != 1)
        {
            printf("Erro ao ler entrada do teclado. \n");
        }
        
        clear_stdin();

        switch(choice)
        {
            case 1:
                numberOfPlayers = GetNumberOfPlayers();
                StartGame(numberOfPlayers);
            case 2:
                srand(time(0));
                socket = create_client_socket();
                game_server_port = login(socket, "127.0.0.1", login_server_port);

                printf("porta de jogo: %d\n", game_server_port);

                play_tictactoe(socket, "127.0.0.1", game_server_port);
                break;
            case 3:
                numberOfPlayers = GetNumberOfPlayers();
                break;                
            case 4:
                printf("Encerrando...\n");
                break;
        }
    }while(choice != 3);
    
    return 0;
}