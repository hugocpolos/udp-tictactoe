#define TAM_MSG 1024
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

typedef struct messagem
{
    char data[TAM_MSG];
    struct sockaddr_in client_addr;
}Mensagem;

int create_client_socket(void);
int rand_range(int min, int max);
Mensagem receive_message(int sockfd);
int envia_mensagem(int socketfd, char *msg, char *host, int port);
int login(int socket, char *host, int port);
int play_tictactoe(int socket, char *host, int port);
void clear_stdin(void);
void disconnect(int socket, char *host, int port);