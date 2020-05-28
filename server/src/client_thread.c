#include <server.h>
#include <client_thread.h>

/* Recursos que serão compartilhados entre threads */
Game tictactoe[NRO_PARTIDAS_SIMULTANEAS];

/* Mutex para fazer a proteção dos recursos */
pthread_mutex_t lock;

int init_shared_variables(void)
/*
    Função para inicializar todos os recursos compartilhados.
*/
{
    int i;
    if (pthread_mutex_init(&lock, NULL) != 0) { 
        printf("Inicialização do mutex falhou.\n"); 
        return 1;
    }
    pthread_mutex_lock(&lock);
    for (i = 0; i < NRO_PARTIDAS_SIMULTANEAS; i++)
    {
        tictactoe[i].number_of_players = 0;
    }
    pthread_mutex_unlock(&lock);
    return 0;
}

void start_remote_tictactoe_game(int socket, Player p, unsigned short game_id)
{
    Mensagem m;
    char msg[TAM_JOGADA];
    int outro_jogador = 1 - p.id;

    while ( strcmp(m.data, "VENCEDOR") && strcmp(m.data, "PERDEDOR") )
    {
        /* Espera jogada */
        m = receive_message(socket);
        sprintf(msg, "%s: %s", p.name, m.data);

        /* Envia jogada para o outro jogador */
        send_message(socket, msg, tictactoe[game_id].players[outro_jogador].addr);
    }

    pthread_mutex_lock(&lock);
    tictactoe[game_id].number_of_players = 0;
    pthread_mutex_unlock(&lock);
    return;
}

void *client_connection_thread(void *client_address)
{
    Mensagem m;
    int socket;
    unsigned short i;
    Player p;
    char msg[2056];
    int client_port;
    Sockaddr *address_rcv = (Sockaddr *)client_address;

    /* Associa o endereço recebido por endereço ao endereço desse jogador */
    p.addr = *address_rcv;

    /* cria novo socket para comunicação dedicada com esse cliente */
    /* Sorteia uma porta e abre um socket com bind nessa porta */
    client_port = rand_range(20000,64000);
    socket = create_socket(client_port);

    /* armazena a porta como string, para ser enviada ao cliente */
    sprintf(msg,"%d", client_port);
    
    /* envia a porta para o cliente */
    send_message(socket, msg, p.addr);

    /* Espera pelo nome do cliente */
    m = receive_message(socket);

    /* O nome recebido é associado como nome do jogador */
    strcpy(p.name, m.data);

    /*
    *   Verifica o número de jogadores conectados até o momento na partida atual. 
    *   Nesse ponto há 3 possibilidades:
    *   - ESPERANDO: há apenas um jogador, e estamos esperando o outro
    *   - PRONTO: 2 jogadores conectados, partida vai começar.
    *   - CHEIO: Há 2 jogadores jogando.
    */

    /* Verifica qual dos estados estamos */
    i = 0;
    while(tictactoe[i].number_of_players == 2)
    /* Percorre o array de partidas procurando um com menos de 2 jogadores */
    {
        i++;
        if (i == NRO_PARTIDAS_SIMULTANEAS)
        {
            /* Todas partidas estão cheias, informa o jogador */
            strcpy(msg, "CHEIO");
            send_message(socket, msg, p.addr);
            close(socket);
            return NULL;
        }
    }
    
    /* Encontrou uma partida com espaço livre, a partida tem indice igual a i */
    /* insere o jogador na partida */
    pthread_mutex_lock(&lock);
    p.id = tictactoe[i].number_of_players;
    tictactoe[i].players[tictactoe[i].number_of_players] = p;
    tictactoe[i].number_of_players++;
    pthread_mutex_unlock(&lock);

    /* envia o estado atual para o jogador */
    strcpy(msg, (tictactoe[i].number_of_players == 1) ? "ESPERANDO" : "PRONTO");
    send_message(socket, msg, p.addr);

    /* se o estado for PRONTO, faz o sorteio dos turnos */
    if (strcmp(msg, "PRONTO") == 0)
    {
        pthread_mutex_lock(&lock);
        tictactoe[i].first_player = rand_range(0,1);
        pthread_mutex_unlock(&lock);
    }


    /* Se houver apenas um jogador conectado, espera o outro conectar */
    printf("jogador conectado, aguardando.\n");
    while (tictactoe[i].number_of_players < 2)
    {
        sleep(1);
    }

    /* Jogo começou - envia FIRST para o primeiro jogador, e SECOND para o segundo.*/
    if (p.id == tictactoe[i].first_player)
    {
        strcpy(msg, "FIRST");
    }
    else
    {
        strcpy(msg, "SECOND");
    }
    send_message(socket, msg, p.addr);

    /* Feito isso, o jogo irá começar entre os dois jogadores conectados.*/
    start_remote_tictactoe_game(socket, p, i);

    close(socket);
    return NULL;
}