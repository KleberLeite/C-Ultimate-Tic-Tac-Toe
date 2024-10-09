/*
Resumo:
O código abaixo cria um jogo da velha NxN (variável: 3 <= BOARD_SIZE <= 26,
a limitação ao 26 é conveniente tanto porque em poucas linhas o tabuleiro já
é maior que o número de linhas disponíveis na janela do terminal, como também
é o número de letras do alfabeto ASCII.) com um menu inicial simples, caso o
jogador decida jogar, ele poderá digitar o nome do jogador 1 e o nome do jogador 2,
em seguida o jogo começa (obs: o primeiro jogador pode ser aleatório caso defina
USE_RAND como 1, ou pode ser fixamente o primeiro jogador, caso defina USE_RAND
como qualquer número diferente de 1). Um exemplo para o visual do jogo é:
"
Current Player: {current_player_name} ({current_player_symbol})


  1   2   3   4   5   6   7   8   9  10  11  12
+---+---+---+---+---+---+---+---+---+---+---+---+
| X | O | X |   | X | X | O | X |   | X | X | X | A
+---+---+---+---+---+---+---+---+---+---+---+---+
| X | O | X |   | X | X | O | X |   | X | X | X | B
+---+---+---+---+---+---+---+---+---+---+---+---+
{ ... }

Insert your play in format 'A1':
"
Nesse momento o jogador deve fazer alguma jogada, como "A4", se A4 estiver
vazio, a jogada é validada, se estiver preenchido ou estiver fora do tamanho
do tabuleiro a jogada é inválida e irá pedir novamente a jogada do jogador
atual. Em seguida é feito a verificação de vitória, que só pode ocorrer, nesse
caso, na 5º jogada em diante, o código resumido é basicamente:
1. Contar quantas casas A (até 2) na direção D.
2. Contar quantas casas B (até 2) na direção oposta à D.
3. Teste isso para vertical, horizontal, diagonal crescente e diagonal decrescente.
4. Verifique se A+B+1 >= 3 em qualquer caso. Se for verdade, então pare a verificação
e diga que o player atual ganhou.
5. Se em nenhum caso A+B+1 >= 3 e é o último turno do jogo, defina empate.
6. Se nada disso acontecer, o jogo continua.

Assim que o jogo termina (por empate ou por vitória), é mostrado o resultado e o
tabuleiro. É dado também a chance do jogador apenas reiniciar o jogo ou voltar ao menu
principal.

OBS: foi utilizado um return -999 no código apenas para evitar de ter mais uma identação
o que poderia prejudicar mais ainda a compreensão já complexa do código, apenas para
validar a configuração de 3 <= BOARD_SIZE <= 26. Caso não possa usar, basta fazer:
if(BOARD_SIZE >= 3 && BOARD_SIZE <= 26)
{
    // Todo o código restante dentro de main aqui!
}

OBS: blocos de comentário multi-linha serão utilizados para explicar o código.
*/


// Set USE_RAND 1 to randomize the first player
//set another number to define the first player
// as player one.
#define USE_RAND 1

#include <stdio.h>
#include <stdlib.h>

// Necessário caso se vá usar rand() para inicializar
// uma seed aleatória o suficiente.
#if USE_RAND == 1
#include <time.h>
#endif // USE_RAND

// Visuals
/*
NAIL = o ponto de encontro entre barras verticais e horizontais
HOR_LINE = linha horizontal
VERT_LINE = linha vertical
PLAYER_ONE_SYMBOL = símbolo p/ identificar no tabuleiro o jogador 1
PLAYER_TWO_SYMBOL = símbolo p/ identificar no tabuleiro o jogador 2
BLANK_SYMBOL = símbolo p/ identificar no tabuleiro que a casa está vazia
*/
#define BOARD_VISUALS_NAIL '+'
#define BOARD_VISUALS_HOR_LINE '-'
#define BOARD_VISUALS_VERT_LINE '|'
#define PLAYER_ONE_SYMBOL 'X'
#define PLAYER_TWO_SYMBOL 'O'
#define BLANK_SYMBOL ' '

// Main Menus
/*
Os sub-menus disponíveis no menu principal
*/
#define M_MAIN_MENU 0
#define M_HOW_TO_PLAY_MENU 1
#define M_QUIT_CONFIRM 2

// Board
/*
Configurações para o tabuleiro:
BOARD_SIZE = tamanho do tabuleiro que deve ser 3 <= BOARD_SIZE <= 26.
EMPTY_VALUE = valor que uma casa vazia possui no tabuleiro.
PLAYER_ONE_VALUE = valor que uma casa preenchida pelo jogador 1 possui
                    no tabuleiro.
 PLAYER_TWO_VALUE = valor que uma casa preenchida pelo jogador 2 possui
                    no tabuleiro.
*/
#define BOARD_SIZE 15
#define EMPTY_VALUE 0
#define PLAYER_ONE_VALUE 1
#define PLAYER_TWO_VALUE 2

// Rules
/*
O número mínimo de símbolos em sequência para um jogador ganhar,
no jogo da velha clássico, 3.
*/
#define MIN_SYMBOLS_TO_WIN 3

// Generals
/*
Configurações para facilitar o desenvolvimento.
*/
#define PLAYER_NICKNAME_MAX_SIZE 16
#define PLAYER_ONE_NAME "Player One"
#define PLAYER_TWO_NAME "Player Two"
#define PLAYER_INPUT_BUFFER_SIZE 15
#define IS_NOT_GAME_OVER 0
#define DRAW -1
#define PLAYER_WINS 1

int main()
{
    /* Valida a configuração do tamanho do tabuleiro mínimo e máximo. */
    if(BOARD_SIZE < 3 || BOARD_SIZE > 26)
    {
        printf("BOARD_SIZE NEED TO BE A VALUE BETWEEN 3 AND 26!");
        return -999;
    }

// Region: SETUP
    /*
    Carrega os símbolos em um array na ordem "VAZIO", "PLAYER 1", "PLAYER 2";
    Inicia o array board, dos nicknames;
    Carrega os nomes padrões p/ o player 1 e player 2;
    Carrega um array que transforma o index do player no seu valor no tabuleiro.
    */
    char symbols[] = { BLANK_SYMBOL, PLAYER_ONE_SYMBOL, PLAYER_TWO_SYMBOL };
    int board[BOARD_SIZE][BOARD_SIZE];
    char player_nicknames[2][PLAYER_NICKNAME_MAX_SIZE];
    char player_names[2][15] = { PLAYER_ONE_NAME, PLAYER_TWO_NAME };
    int player_index_to_value[] = { PLAYER_ONE_VALUE, PLAYER_TWO_VALUE };
    int board_size_sqr = BOARD_SIZE * BOARD_SIZE;

    /* Se configurado para usar rand(), configura a seed */
#if USE_RAND == 1
    srand(time(NULL));
#endif // USE_RAND;
// EndRegion: SETUP

    // Main loop
    /* Loop onde todo o jogo vai rodar, seja menu, ou a própria partida. */
    while(1)
    {
        // Region: MAIN MENU
        /*
            Um loop que mostra, atualiza e faz as interações com o menu principal
            e seus sub-menus.
        */
        int m_current_menu = M_MAIN_MENU;
        while(1)
        {
            system("cls");
            int go_to_gameplay = 0;

            switch(m_current_menu)
            {
            case M_MAIN_MENU:
                printf("         +-------------------------------------+\n");
                printf("         |                                     |\n");
                printf("         |               Welcome               |\n");
                printf("         |                 to                  |\n");
                printf("         |          Ultimate TicTacToe         |\n");
                printf("         |                                     |\n");
                printf("         +-------------------------------------+\n\n\n");
                printf("         +-------------------------------------+\n");
                printf("         |     1 - Play                        |\n");
                printf("         |     2 - How to Play                 |\n");
                printf("         |     3 - Quit                        |\n");
                printf("         +-------------------------------------+\n");

                char m_mm_char_option = getchar();

                /* Limpa o buffer de input */
                while(m_mm_char_option != '\n' && getchar() != '\n');

                int m_mm_option = m_mm_char_option - '0';
                if(m_mm_option <= 3 && m_mm_option >= 2)
                    m_current_menu = m_mm_option - 1; // It's starts in 0, not in 1
                else if(m_mm_option == 1)
                    go_to_gameplay = 1;

                break;
            case M_HOW_TO_PLAY_MENU:
                 printf("HOW TO PLAY:\n\n");
                printf("Ultimate TicTacToe is a variation of the famous tictactoe\n");
                printf("game in which you play on an %dx%d board with the\n", BOARD_SIZE, BOARD_SIZE);
                printf("conventional objective of getting 3 consecutive pieces\n");
                printf("diagonally, vertically or horizontally.");
                printf("\n\nPress RETURN to return to main menu...");

                char m_hp_press_key = getchar();
                /* Limpa o buffer de input */
                while(m_hp_press_key != '\n' && getchar() != '\n');

                m_current_menu = M_MAIN_MENU;

                break;
            case M_QUIT_CONFIRM:
                printf("Do you really want to Quit?\nPress Y to confirm or any other to cancel.\n");
                char input = getchar();
                if(input == 'Y' || input == 'y')
                    return 0;

                /* Limpa o buffer de input */
                while(input != '\n' && getchar() != '\n');

                m_current_menu = M_MAIN_MENU;

                break;
            }

            if(go_to_gameplay == 1)
                break;
        }
        // EndRegion: MAIN MENU

        // Region: PREPARE TO GAMEPLAY
        /* Região onde os jogadores irão inserir seus nicknames. */
        // Clear player_nicknames
        for(int i = 0; i < 2; i++)
            for(int j = 0; j < PLAYER_NICKNAME_MAX_SIZE; j++)
                player_nicknames[i][j] = ' ';

        // Read Player nicknames
        for(int i = 0; i < 2; i++)
        {
            while(1)
            {
                system("cls");
                // Show player one nickname if it's exists
                if(i == 1)
                    printf("%s nickname: %s\n", player_names[0], player_nicknames[0]);

                printf("Insert %s nickname (max %d characters): ", player_names[i], PLAYER_NICKNAME_MAX_SIZE - 1);
                gets(player_nicknames[i]);

                // Trim left
                while(isspace(player_nicknames[i][0]))
                {
                    for(int j = 1; j < PLAYER_NICKNAME_MAX_SIZE; j++)
                        player_nicknames[i][j-1] = player_nicknames[i][j];
                }

                // Trim right
                int finded_end = 0;
                for(int j = PLAYER_NICKNAME_MAX_SIZE - 1; j >= 0; j--)
                {
                    if(!finded_end && player_nicknames[i][j] == '\0')
                        finded_end = 1;
                    else if(finded_end && !isspace(player_nicknames[i][j]))
                    {
                        player_nicknames[i][j+1] = '\0';
                        break;
                    }
                }

                // Check if it's valid
                if(player_nicknames[i][0] == '\0')
                {
                    printf("\n\nInvalid nickname! Press RETURN to insert again.");
                    int key = getchar();
                    while(key != '\n' && getchar() != '\n');
                }
                else
                    break;
            }
        }
        // EndRegion: PREPARE TO GAMEPLAY

        // Replay loop
        /*
        Região onde o jogo de fato acontece na seguinte ordem:
        1. Desenha o tabuleiro e valida a jogada do jogador atual
        2. Aplica a jogada no tabuleiro
        3. Verifica se o jogo empatou, teve um vencedor ou deve continuar
        4. Se o jogo acabou, imprime o resultado, se não, repete 1,2 e 3.
        */
        while(1)
        {
            int replay = 0;

            // Clear board
            for(int x = 0; x < BOARD_SIZE; x++)
                for(int y = 0; y < BOARD_SIZE; y++)
                    board[x][y] = EMPTY_VALUE;

            // Region: GAMEPLAY
            // O index 0 representa o player 1, o index 1 o player 2.
            int current_player_index = 0;
            #if USE_RAND == 1
            current_player_index = rand() % 2;
            #endif // USE_RAND

            for(int turn = 1; turn <= board_size_sqr; turn++)
            {
                int input_x = -1, input_y = -1;

                // Region: INPUT
                while(1)
                {
                    system("cls");

                    printf(
                        "Current Player: %s (%c)\n\n",
                        player_nicknames[current_player_index],
                        symbols[ player_index_to_value[current_player_index] ]
                    );

                    // Draw board
                    // Draw collumn identifier
                    for(int i = 0; i < BOARD_SIZE; i++)
                        printf(i < 9 ? "  %d " : " %d ", i+1);
                    printf("\n");
                    // Draw line
                    printf("%c", BOARD_VISUALS_NAIL);
                    for(int i = 0; i < BOARD_SIZE; i++)
                        printf("%c%c%c%c", BOARD_VISUALS_HOR_LINE, BOARD_VISUALS_HOR_LINE, BOARD_VISUALS_HOR_LINE, BOARD_VISUALS_NAIL);
                    printf("\n");
                    for(int y = 0; y < BOARD_SIZE; y++)
                    {
                        // Draw cells
                        printf("%c", BOARD_VISUALS_VERT_LINE);
                            for(int i = 0; i < BOARD_SIZE; i++)
                                printf(" %c |", symbols[ board[i][y] ]);

                        // Draw row identifier
                        printf(" %c\n", y + 'A');

                        // Draw line
                        printf("%c", BOARD_VISUALS_NAIL);
                        for(int i = 0; i < BOARD_SIZE; i++)
                            printf("%c%c%c%c", BOARD_VISUALS_HOR_LINE, BOARD_VISUALS_HOR_LINE, BOARD_VISUALS_HOR_LINE, BOARD_VISUALS_NAIL);
                        printf("\n");
                    }

                    printf("\n\nInsert your play in format 'A1': ");

                    // Cria e limpa o buffer de input
                    char input_buffer[PLAYER_INPUT_BUFFER_SIZE];
                    for(int i = 0; i < PLAYER_INPUT_BUFFER_SIZE; i++)
                        input_buffer[i] = ' ';

                    gets(input_buffer);
                    for(int i = 0; i < PLAYER_INPUT_BUFFER_SIZE; i++)
                    {
                        int finded = 0;
                        if(input_buffer[i] >= 'A' && input_buffer[i] <= 'Z')
                        {
                            input_y = input_buffer[i] - 'A';
                            finded = 1;
                        }
                        else if(input_buffer[i] >= 'a' && input_buffer[i] <= 'z')
                        {
                            input_y = input_buffer[i] - 'a';
                            finded = 1;
                        }

                        if(finded)
                        {
                            input_x = atoi(input_buffer + i + 1) - 1; // It's started in 0, not in 1
                            break;
                        }
                    }

                    // Valid input
                    if(
                       input_y < BOARD_SIZE
                       && input_x >= 0
                       && input_x < BOARD_SIZE
                       && board[input_x][input_y] == EMPTY_VALUE
                    )
                        break;
                }
                // EndRegion: INPUT

                board[input_x][input_y] = player_index_to_value[ current_player_index ];

                // Region: CHECK VICTORY
                int victory_result = IS_NOT_GAME_OVER;
                if(turn >= 5)
                {
                    int this_player_value = player_index_to_value[ current_player_index ];

                    // Check Horizontal
                    int h_count_right = 0, h_count_left = 0;
                    for(int k = 1; k < MIN_SYMBOLS_TO_WIN && input_x + k < BOARD_SIZE; k++)
                    {
                        if(board[input_x + k][input_y] == this_player_value)
                            h_count_right++;
                        else
                            break;
                    }
                    for(int k = 1; k < MIN_SYMBOLS_TO_WIN && input_x - k >= 0; k++)
                    {
                        if(board[input_x - k][input_y] == this_player_value)
                            h_count_left++;
                        else
                            break;
                    }

                    if(h_count_left + h_count_right + 1 >= MIN_SYMBOLS_TO_WIN)
                        victory_result = PLAYER_WINS;

                    // Check Vertical
                    if(victory_result != PLAYER_WINS)
                    {
                        int v_count_top = 0, v_count_bot = 0;
                        for(int k = 1; k < MIN_SYMBOLS_TO_WIN && input_y + k < BOARD_SIZE; k++)
                        {
                            if(board[input_x][input_y + k] == this_player_value)
                                v_count_top++;
                            else
                                break;
                        }
                        for(int k = 1; k < MIN_SYMBOLS_TO_WIN && input_y - k >= 0; k++)
                        {
                            if(board[input_x][input_y - k] == this_player_value)
                                v_count_bot++;
                            else
                                break;
                        }

                        if(v_count_bot + v_count_top + 1 >= MIN_SYMBOLS_TO_WIN)
                            victory_result = PLAYER_WINS;
                    }

                    // Check Diagonal(/)
                    if(victory_result != PLAYER_WINS)
                    {
                        int d_count_right = 0, d_count_left = 0;
                        for(int k = 1; k < MIN_SYMBOLS_TO_WIN && input_y + k < BOARD_SIZE && input_x + k < BOARD_SIZE; k++)
                        {
                            if(board[input_x + k][input_y + k] == this_player_value)
                                d_count_right++;
                            else
                                break;
                        }
                        for(int k = 1; k < MIN_SYMBOLS_TO_WIN && input_y - k >= 0 && input_x - k >= 0; k++)
                        {
                            if(board[input_x - k][input_y - k] == this_player_value)
                                d_count_left++;
                            else
                                break;
                        }

                        if(d_count_left + d_count_right + 1 >= MIN_SYMBOLS_TO_WIN)
                            victory_result = PLAYER_WINS;
                    }

                    // Check Diagonal(\)
                    if(victory_result != PLAYER_WINS)
                    {
                        int d_count_right = 0, d_count_left = 0;
                        for(int k = 1; k < MIN_SYMBOLS_TO_WIN && input_y - k >= 0 && input_x + k < BOARD_SIZE; k++)
                        {
                            if(board[input_x + k][input_y - k] == this_player_value)
                                d_count_right++;
                            else
                                break;
                        }
                        for(int k = 1; k < MIN_SYMBOLS_TO_WIN && input_y + k < BOARD_SIZE && input_x - k >= 0; k++)
                        {
                            if(board[input_x - k][input_y + k] == this_player_value)
                                d_count_left++;
                            else
                                break;
                        }

                        if(d_count_left + d_count_right + 1 >= MIN_SYMBOLS_TO_WIN)
                            victory_result = PLAYER_WINS;
                    }

                    if(victory_result != PLAYER_WINS && turn == board_size_sqr)
                        victory_result = DRAW;
                }
                // EndRegion: CHECK VICTORY

                if(victory_result == IS_NOT_GAME_OVER)
                {
                    current_player_index = (current_player_index + 1) % 2;
                    continue;
                }

                system("cls");
                // Region: SHOW GAME OVER
                if(victory_result == PLAYER_WINS)
                    printf("%s (%c) WINS!!!\n\n", player_nicknames[current_player_index], symbols[ player_index_to_value[current_player_index] ]);
                else
                    printf("DRAW!!!\n\n");

                // Draw board
                // Draw collumn identifier
                for(int i = 0; i < BOARD_SIZE; i++)
                    printf(i < 9 ? "  %d " : " %d ", i+1);
                printf("\n");
                // Draw line
                printf("%c", BOARD_VISUALS_NAIL);
                for(int i = 0; i < BOARD_SIZE; i++)
                    printf("%c%c%c%c", BOARD_VISUALS_HOR_LINE, BOARD_VISUALS_HOR_LINE, BOARD_VISUALS_HOR_LINE, BOARD_VISUALS_NAIL);
                printf("\n");
                for(int y = 0; y < BOARD_SIZE; y++)
                {
                    // Draw cells
                    printf("%c", BOARD_VISUALS_VERT_LINE);
                        for(int i = 0; i < BOARD_SIZE; i++)
                            printf(" %c |", symbols[ board[i][y] ]);

                    // Draw row identifier
                    printf(" %c\n", y + 'A');

                    // Draw line
                    printf("%c", BOARD_VISUALS_NAIL);
                    for(int i = 0; i < BOARD_SIZE; i++)
                        printf("%c%c%c%c", BOARD_VISUALS_HOR_LINE, BOARD_VISUALS_HOR_LINE, BOARD_VISUALS_HOR_LINE, BOARD_VISUALS_NAIL);
                    printf("\n");
                }

                printf("\n\n\nPress RETURN to return to main menu or R to play again!\n");
                int play_again_input = getchar();
                if(play_again_input == 'R' || play_again_input == 'r')
                    replay = 1;
                else
                    while(play_again_input != '\n' && getchar() != '\n');

                break;
                // EndRegion: SHOW GAME OVER
            }

            if(replay == 0)
                break;
        }
        // EndRegion: GAMEPLAY
    }

    return 0;
}
