#include <stdio.h>
#include <stdlib.h>

//Limpa o console e desenha o tabuleiro
void DrawBoard(char gameBoard[9]);

//Vers�o alternativa da fun��o anterior, para caso seja necess�rio informar de qual jogador � a vez
void DrawBoardWithNames(char gameBoard[9], char *player1Name, char *player2Name, char playerCharacter);

//Verifica se houve uma vitoria (1), empate (2) ou se o jogo continua (0)
int CheckPlayerWin(char gameBoard[9]);
