#include <server.h>
Game tictactoe;
pthread_mutex_t lock;

int rand_range(int min, int max)
{
	return min + rand()%(max - min + 1);	
}

void *client_connection(void *client_address)
{
    Mensagem m;
    int socket;
    Player p;
    char msg[2056];
    int client_port;
    Sockaddr *address_rcv = (Sockaddr *)client_address;

    p.addr = *address_rcv;

    /* cria novo socket para comunicação dedicada com esse cliente */
    client_port = rand_range(20000,64000);
    socket = create_socket(client_port);

    /* armazena em msg a porta como string, para ser enviada ao cliente */
    sprintf(msg,"%d", client_port);
    
    send_message(socket, msg, p.addr);

    /* Espera pelo nome do cliente */
    m = receive_message(socket);

    strcpy(p.name, m.data);

    /* Nesse ponto há 3 possibilidades:
    *   - ESPERANDO: há apenas um jogador, e estamos esperando o outro
    *   - PRONTO: 2 jogadores conectados, partida vai começar.
    *   - CHEIO: Há 2 jogadores jogando.
    */

    /* Verifica qual dos estados estamos */

    if (tictactoe.number_of_players == 2)
    {
        /* envia o estado CHEIO para o jogador */
        strcpy(msg, "CHEIO");
        send_message(socket, msg, p.addr);
        close(socket);
        return NULL;
    }
    else
    {
        /* insere o jogador na partida */
        pthread_mutex_lock(&lock);
        tictactoe.players[tictactoe.number_of_players] = p;
        p.id = tictactoe.number_of_players++;
        pthread_mutex_unlock(&lock);

        /* envia o estado atual para o jogador */
        strcpy(msg, (tictactoe.number_of_players == 1) ? "ESPERANDO" : "PRONTO");
        send_message(socket, msg, p.addr);

        /* se o estado for PRONTO, faz o sorteio dos turnos */
        if (strcmp(msg, "PRONTO") == 0)
        {
            pthread_mutex_lock(&lock);
            tictactoe.first_player = rand_range(0,1);
            pthread_mutex_unlock(&lock);
        }
    }

    /* Se houver apenas um jogador conectado, espera o outro conectar */
    printf("jogador conectado, aguardando.\n");
    while (tictactoe.number_of_players < 2)
    {
        sleep(1);
    }

    /* Jogo começou - envia FIRST para o primeiro jogador, e SECOND para o segundo.*/
    if (p.id == tictactoe.first_player)
    {
        strcpy(msg, "FIRST");
    }
    else
    {
        strcpy(msg, "SECOND");
    }
    send_message(socket, msg, p.addr);

    while ( 1 )
    {
        /* Espera jogada */
        m = receive_message(socket);
        sprintf(msg, "%s: %s", p.name, m.data);

        /* Envia jogada para o outro jogador */
        send_message(socket, msg, tictactoe.players[1-p.id].addr);
    }

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
    client_threads = malloc(MAX_LOGINS * sizeof(pthread_t));

    // inicia o socket de login
    socket = create_socket(8080);

    printf("Aguardando a conexão de jogadores.\n");
    while (42)
    {
        m = receive_message(socket);
        if(strcmp(m.data, "LOGIN") == 0)
        {
            if(conn_clients > MAX_LOGINS)
            {
                printf("falha no login, numero maximo de clientes conectados.\n");
            }
            else
            {
                /* novo login */
                if(pthread_create(&client_threads[conn_clients], NULL, client_connection, &m.client_addr))
                {
                    printf("erro criando thread.\n");
                    printf("falha no login,\n");
                }
                conn_clients++;
            }
        }
    }
    close(socket);
}

int init_server(void)
{
    srand(time(0));
    tictactoe.number_of_players = 0;
    if (pthread_mutex_init(&lock, NULL) != 0) { 
        printf("Inicialização do mutex falhou.\n"); 
        return 1;
    }
    return 0;
}

int main()
{
    if(init_server())
    {
        printf("falha ao inicializar o servidor.\n");
        return 1;
    }
    printf("servidor inicializado.\n");
    wait_for_login();
    return 0;
}