#include <server.h>

int rand_range(int min, int max)
{
	return min + rand()%(max - min + 1);	
}

void *client_connection(void *client_address)
{
    Mensagem m;
    int socket;
    Player p;
    char msg[1024];
    int client_port;
    Sockaddr *address_rcv = (Sockaddr *)client_address;

    /* cria novo socket para comunicação dedicada com esse cliente */
    client_port = rand_range(20000,64000);
    socket = create_socket(client_port);

    /* armazena em msg a porta como string, para ser enviada ao cliente */
    sprintf(msg,"%d", client_port);
    
    send_message(socket, msg, *address_rcv);

    /* Espera pelo nome do cliente */
    m = receive_message(socket);

    printf("%s", m.data);

    strcpy(p.name, m.data);

    printf("client connected.\n");
    do
    {
        m=receive_message(socket);
        printf("%s: %s\n", p.name, m.data);
    }while(strcmp(m.data, "exit"));
    printf("client disconneted.\n");

    close(socket);
    return NULL;
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
    int conn_clients = 0;
    pthread_t *client_threads;
    Mensagem m;

    // aloca memória para as threads
    client_threads = malloc(sizeof(pthread_t));

    // inicia o socket de login
    socket = create_socket(8080);

    while (42)
    {
        m = receive_message(socket);
        if(strcmp(m.data, "LOGIN") == 0)
        {
            /* novo login */
            if(pthread_create(client_threads, NULL, client_connection, &m.client_addr))
            {
                printf("erro criando thread.\n");
            }
            conn_clients++;
        }
    }
    close(socket);
}

int main()
{
    srand(time(0));
    wait_for_login();
}