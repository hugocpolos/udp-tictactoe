#include <Player.h>

//Muda o caractere do jogador atual
void ChangePlayer(char playerCharacter)
{
    if(playerCharacter == 'X')
    {
      playerCharacter = 'O';
    }
    else
    {
     playerCharacter = 'X';
    }
}

//Retorna o numero de jogadores escolhido pelo usuario (1 ou 2)
int GetNumberOfPlayers()
{	
	int numberOfPlayers;

	system("clear");
	
	printf("Quantos jogadores irao participar? ");
	scanf("%d", &numberOfPlayers);

	switch (numberOfPlayers)
	{
	case 1:
		return numberOfPlayers;
		break;
	case 2:
		return numberOfPlayers;
		break;
	default:
		return 0;
		break;
	}

}

//Define o nome do jogador (O par�metro de n�mero de jogadores define qual a mensagem mostrada no console e informa qual jogador deve informar o nome)
char* GetPlayerName(int playerNumber)	//Caso seja contra o computador, playerNumber � 0, caso seja contra outro jogador, playerNumber � 1 ou 2
{
	char *temp = calloc(100, sizeof(char)); //Vari�vel tempor�ria para receber o nome do jogador
	char *playerName;	//Vari�vel que ir� retornar
	if(playerNumber == 0)
	{
		printf("Digite o seu nome: ");
	}
	else if(playerNumber == 1 || playerNumber == 2)
	{
		printf("Digite o nome do jogador %d: ", playerNumber);
	}
	if(scanf("%[^\n]s", temp) == 1)
	{
		playerName = calloc(strlen(temp), sizeof(char));	//Aloca apenas a mem�ria necess�ria para suportar o nome do jogador
		strcpy(playerName, temp);	//Copia o conte�do de temp para a vari�vel que ir� retornar
		free(temp);	//Libera a mem�ria alocada na linha 35
	}
	
	return playerName;
}
