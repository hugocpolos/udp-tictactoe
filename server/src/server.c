#include <server.h>
#include <client_thread.h>

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
        return -1;
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

int wait_for_login( int port )
{
    int socket;
    int conn_clients = 0;
    pthread_t client_thread;
    Mensagem m;

    // inicia o socket de login
    socket = create_socket(port);

    if (socket == -1)
    {
        exit(EXIT_FAILURE);
    }

    printf("Aguardando a conexão de jogadores.\n");
    while (42)
    {
        m = receive_message(socket);
        if(strcmp(m.data, "LOGIN") == 0)
        {
            printf("Nova conexão.\n");

            /* novo login */
            if(pthread_create(&client_thread, NULL, client_connection_thread, &m.client_addr))
            {
                printf("erro criando thread.\n");
                printf("falha no login,\n");
            }
            conn_clients++;
        }
    }
    close(socket);
}

int init_server(void)
{
    srand(time(0));
    return 0;
}

void show_usage (char *bin_name)
{
    printf("usage: %s <int: port>\n", bin_name);
}

int main(int argc, char **argv)
{
    int server_port;
    if (argc != 2)
    {
        show_usage(argv[0]);
        return 1;
    }
    
    server_port = atoi(argv[1]);
    if (server_port == 0)
    {
        show_usage(argv[0]);
        return 1;
    }

    if(init_server())
    {
        printf("falha ao inicializar o servidor.\n");
        return 1;
    }
    if (init_shared_variables())
    {
        printf("falha ao inicializar o servidor.\n");
        return 1;
    }
    printf("servidor inicializado.\n");
    wait_for_login(server_port);
    return 0;
}