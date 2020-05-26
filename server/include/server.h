#define TAM_MSG 1024
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <memory.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <stdarg.h>
#include <time.h>
/* the next two includes probably aren't relevant for you, but I typically use them all anyway */
#include <math.h>
#include <sys/termios.h>

typedef struct messagem
{
    char data[TAM_MSG];
    struct sockaddr_in client_addr;
}Mensagem;

typedef struct player
{
    char name[64];
}Player;

int create_socket(int port);
Mensagem receive_message(int sockfd);

