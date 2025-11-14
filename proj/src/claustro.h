#ifndef __CLAUSTRO_H
#define __CLAUSTRO_H

#include "action.h"
#include "img/black_pawn.xpm"
#include "img/blue_pawn.xpm"
#include "img/board.xpm"
#include "img/board_dark_mode.xpm"
#include "img/current_player.xpm"
#include "img/game_over_player1.xpm"
#include "img/game_over_player2.xpm"
#include "img/green_pawn.xpm"
#include "img/grey_pawn.xpm"
#include "img/menu.xpm"
#include "img/mouse_pointer.xpm"
#include "img/play_button.xpm"
#include "img/player1.xpm"
#include "img/player2.xpm"
#include "img/quit_button.xpm"
#include "img/white_pawn.xpm"
#include "sprite.h"
#include <lcom/lcf.h>
#include <stdbool.h>
#include <stdint.h>

// #define BOARD_LENGTH 351
#define MARGIN_LENGTH 7
#define DIVISION_LENGTH 3
#define CELL_LENGTH 65 // ((board_length - (2 * margin_length) - (4 * divison_length)) / BOARD_SIZE)
// #define PAWN_LENGTH 45
// #define PAWN_OFFSET 10 // ((cell_length - pawn_length) / 2)
// #define HELPER_PAWN_LENGTH 20
#define MOUSE_POINTER_OFFSET 32 // (cell_length / 2)

#define DARK_MODE_HOUR 20

#define PLAYER1 1
#define PLAYER2 2
#define BOARD_SIZE 5
#define EMPTY 0
#define NON_OCCUPIABLE_SQUARE 3
#define GOAL1 4
#define GOAL2 5

Sprite *menu;
Sprite *play_button;
Sprite *quit_button;
Sprite *game_over_player1;
Sprite *game_over_player2;
Sprite *player1_sp;
Sprite *player2_sp;
Sprite *current_player_sp;
Sprite *mouse_pointer;
Sprite *board_sp;
Sprite *board_sp_dark_mode;
Sprite *player1_pawn;
Sprite *player2_pawn;
Sprite *player1_dark_mode_pawn;
Sprite *player2_dark_mode_pawn;
Sprite *helper_pawn;

typedef struct
{
  int xi;
  int yi;
  int xf;
  int yf;
} Move;

/**
 * @brief Inicializa a matriz board (representação interna do tabuleiro) 
 * com os valores que reprensentam o disposição das peças no início do jogo.
*/
void(setup_board)();

/**
 * @brief Imprime o jogador atual e o tabuleiro na consola.
*/
void(print_board)();

/**
 * @brief Incrementa/decrementa uma unidade à váriável passada por argumento
 * que representa um campo da struct Move. 
 * Esta função é invocada quando o utilizador prime as teclas W,A,S,D.
 * @param add_or_sub se verdadeiro incrementa, caso contrário decrementa
*/
void(update_pawn_move)(int *pawn_move_pos, bool add_or_sub);

/**
 * @brief Dada a posição x ou y do rato no ecrã, calcula a abscissa/ordenada no tabuleiro.
 * @return valor entre 0 e BOARD_SIZE. BOARD_SIZE representa um valor fora do tabuleiro
*/
int(find_board_cell)(int16_t position);

/**
 * @brief Valida e executa uma jogada, movendo a peça de (xi, yi) para (xf, yf).
*/
int(move)(uint8_t xi, uint8_t yi, uint8_t xf, uint8_t yf);

/**
 * @brief Valida uma jogada de acordo com as regras do Claustro.
*/
int(can_move)(uint8_t player, uint8_t xi, uint8_t yi, uint8_t xf, uint8_t yf);

/**
 * @brief Verifica se a peça em (x,y) pertence ao jogador atual.
*/
int(can_pick)(uint8_t x, uint8_t y);

/**
 * @brief Coloca/retira uma peça do tabuleiro em (x,y).
 * @param pawn PLAYER1, PLAYER2 ou EMPTY
*/
void(set_pawn)(uint8_t pawn, uint8_t x, uint8_t y);

/**
 * @brief Coloca a peça capturada no tabuleiro em (x,y),
 * verificando se essa posição está EMPTY.
*/
int(place_back)(uint8_t x, uint8_t y);

/**
 * @brief Avalia um tabuleiro e verifica se há um vencedor.
 * @return PLAYER 1, PLAYER2 ou 0 casa não haja vencedor
*/
int(game_over)();

/**
 * @brief Verifica se o jogador passado como argumento tem pelo menos uma jogada válida.
*/
int(valid_move)(uint8_t player);

/**
 * @brief Troca os jogadores.
*/
void(swap_player)();

/**
 * @brief Atualiza o estado do jogo de acordo com os eventos do rato/teclado.
*/
void(update_state_machine)();



/**
 * @brief Cria todos os sprites.
*/
void(create_sprites)();

/**
 * @brief Destroi todos os sprites.
*/
void(destroy_sprites)();

/**
 * @brief Desenha o tabuleiro (e as peças) de acordo com o modo atual (escuro ou claro).
*/
int(draw_board)();

/**
 * @brief Desenha as peças.
*/
int(draw_pawns)();

/**
 * @brief Desenha os destinos/jogadas possíveis para uma peça quando o cursor está por cima da mesma.
*/
void(draw_valid_moves)();

/**
 * @brief Desenha informação complementar ao tabuleiro: jogador atual 
 * e associação entre as peças e o jogador.
*/
int(draw_side_info)();

/**
 * @brief Desenha o cursor.
*/
int(draw_mouse_pointer)();

/**
 * @brief Desenha o menu principal, incluido os botões PLAY e QUIT.
*/
void(draw_main_menu)();

/**
 * @brief Desenha o menu final, incluido os botões PLAY e QUIT.
*/
void(draw_game_over_menu)();

/**
 * @brief Desenha uma frame de acordo com o estado do jogo.
*/
void(draw_frame)();

/**
 * @brief Verifica se o botão PLAY foi clicado pelo rato.
*/
int(clicked_play_button)();

/**
 * @brief Verifica se o botão QUIT foi clicado pelo rato.
*/
int(clicked_quit_button)();

#endif
