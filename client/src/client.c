#include <stdio.h>
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

int main()
{
	int socket;
	char msg[1024];
	int server_port = 8080;
	Mensagem m;

	srand(time(0));	
	gets(msg, "LOGIN");

	socket = create_client_socket();
	envia_mensagem(socket, msg, "hugo", server_port);
	m = receive_message(socket);
	printf("porta recebida: %s\n", m.data);

	close(socket);


}
