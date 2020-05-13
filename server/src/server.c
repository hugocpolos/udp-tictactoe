#include <server.h>

int rand_range(int min, int max)
{
	return min + rand()%(max - min + 1);	
}

int create_socket(int port)
{
	int sockfd;
    struct sockaddr_in servaddr;

    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
      
    memset(&servaddr, 0, sizeof(servaddr)); 
      
    // Filling server information
    servaddr.sin_family = AF_INET; // IPv4 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(port);

    // Bind the socket with the server address 
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,  
            sizeof(servaddr)) < 0 ) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    }
	return sockfd;
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

void send_message(int socket, char *msg, struct sockaddr_in client_addr)
{
    sendto(socket, (const char *)msg, strlen(msg), 
        MSG_CONFIRM, (const struct sockaddr *) &client_addr,  
            sizeof(client_addr));
}

int wait_for_login( void )
{
    int socket;
    int clientes_sockets[10];
    int clientes_conectados = 0;
    Mensagem m;
    char msg[TAM_MSG];
    int porta;
    socket = create_socket(8080);

    while (42)
    {
        m = receive_message(socket);
        if(!strcmp(m.data, "LOGIN"))
        {
            /* novo login */
            /* cria novo socket para comunicação dedicada com esse cliente */
            porta = rand_range(20000,64000);
            clientes_sockets[clientes_conectados] = create_socket(porta);
            
            /* armazena em msg a porta como string, para ser enviada ao cliente */
            sprintf(msg,"%d", porta);
            send_message(clientes_sockets[clientes_conectados], msg, m.client_addr);
            close(clientes_sockets[clientes_conectados]);
        }
    }
    close(socket);
}

int main()
{
    srand(time(0));
    wait_for_login();
}