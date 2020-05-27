#include <client.h>

int create_client_socket(void)
{
	int porta;
	struct sockaddr_in clientaddr;
	int socketfd;

	porta = rand_range(20000, 64000);

	// Inicia estrutura de socket e servidor
	bzero((char *) &clientaddr, sizeof(clientaddr));
	clientaddr.sin_family = AF_INET; 
	clientaddr.sin_port = htons(porta); 
	clientaddr.sin_addr.s_addr = INADDR_ANY;

	// Cria socket
	socketfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (socketfd < 0)
	{
		printf("Error opening socket\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		// Faz o bind
		if(bind(socketfd, (struct sockaddr *) &clientaddr, sizeof(clientaddr)) < 0)
		{ 
				//perror("Falha na nomeação do socket\n");
				exit(EXIT_FAILURE);
		}
	}

	return socketfd;
}

Mensagem receive_message(int sockfd)
{
    Mensagem m;
    unsigned int addr_len;

    addr_len = sizeof(m.client_addr);
    memset(&(m.data), '\0', TAM_MSG);
    recvfrom(sockfd,m.data,TAM_MSG,0,(struct sockaddr *)&(m.client_addr), &addr_len);

    return m;
}

int envia_mensagem(int socketfd, char *msg, char *host, int port)
{
	struct sockaddr_in hostaddr;
	hostaddr.sin_family = AF_INET; 
    hostaddr.sin_port = htons(port); 
    hostaddr.sin_addr.s_addr = INADDR_ANY;

	sendto(socketfd, (const char *)msg, strlen(msg), 
        MSG_CONFIRM, (const struct sockaddr *) &hostaddr,  
            sizeof(hostaddr));
	
	return 0;
}


int rand_range(int min, int max)
{
	return min + rand()%(max - min + 1);	
}

int login(int socket, char *host, int port)
/* Realiza login no servidor
 * retorna número da porta de comunicação dedicada com o servidor.	 
*/
{
	Mensagem m;
	char msg[6];
	strcpy(msg, "LOGIN");

	envia_mensagem(socket, msg, host, port);
	m = receive_message(socket);

	return atoi(m.data);
}

int play_tictactoe(int socket, char *host, int port)
{
	char msg[1024];
	Mensagem m;

	/* envia o nome do jogador para o servidor: */
	printf("Nome do jogador: ");
	/* fgets é alternativa ao gets. fonte: https://stackoverflow.com/questions/43174201/alternative-to-gets */
	if (fgets(msg, sizeof msg, stdin)) {
		msg[strcspn(msg, "\n")] = '\0';
	}
	envia_mensagem(socket, msg, host, port);

	/* Espera pelo estado da partida */
	m = receive_message(socket);

	if (strcmp(m.data, "CHEIO") == 0)
	{
		printf("Já existe uma partida acontecendo. Tente mais tarde.\n");
		return 0;
	}

	if (strcmp(m.data, "ESPERANDO") == 0 )
	{
		printf("Conectado. Aguardando outro jogador.\n");
	}

	if (strcmp(m.data, "PRONTO") == 0)
	{
		printf("Partida pronta. O jogo vai começar em breve.\n");
	}

	/* espera pela indicação de que o jogo começou, assim como a ordem dos turnos*/
	m = receive_message(socket);
	printf("%s\n", m.data);

	if(strcmp(m.data, "FIRST") == 0)
	{
		while (1)
		{
			printf("qual sua jogada?");
			if (fgets(msg, sizeof msg, stdin)) {
				msg[strcspn(msg, "\n")] = '\0';
			}
			envia_mensagem(socket, msg, host, port);

			/* espera jogada */
			m = receive_message(socket);
			printf("jogada: %s\n", m.data);
		}
	}
	else if (strcmp(m.data, "SECOND") == 0)
	{
		while (1)
		{
			/* espera jogada */
			m = receive_message(socket);
			printf("jogada: %s\n", m.data);

			printf("qual sua jogada?");
			if (fgets(msg, sizeof msg, stdin)) {
				msg[strcspn(msg, "\n")] = '\0';
			}
			envia_mensagem(socket, msg, host, port);
		}
	}

	return 0;
}

int main(int argc, char* argv[])
{
	int socket;
	int login_server_port = 8080;
	int game_server_port;

	srand(time(0));
	socket = create_client_socket();
	game_server_port = login(socket, "127.0.0.1", login_server_port);

	printf("porta de jogo: %d\n", game_server_port);

	ShowMenu();

	play_tictactoe(socket, "127.0.0.1", game_server_port);

	close(socket);

}
