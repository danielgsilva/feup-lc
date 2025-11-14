#include "claustro.h"

static int board[BOARD_SIZE][BOARD_SIZE];
static uint8_t player = PLAYER1;
static uint8_t winner;
bool dark_mode = false;
Move pawn_move = {2, 2, 0, 0}; // {xi, yi, xf, yf}
extern GameState game_state;
extern MouseState mouse_state;

void(setup_board)() {
  int initial_board[BOARD_SIZE][BOARD_SIZE] = {
    {NON_OCCUPIABLE_SQUARE, EMPTY, PLAYER1, PLAYER1, GOAL2},
    {EMPTY, EMPTY, EMPTY, EMPTY, PLAYER1},
    {PLAYER2, EMPTY, EMPTY, EMPTY, PLAYER1},
    {PLAYER2, EMPTY, EMPTY, EMPTY, EMPTY},
    {GOAL1, PLAYER2, PLAYER2, EMPTY, NON_OCCUPIABLE_SQUARE}};

  for (int i = 0; i < BOARD_SIZE; i++) {
    for (int j = 0; j < BOARD_SIZE; j++) {
      board[i][j] = initial_board[i][j];
    }
  }
}

void(print_board)() {
  printf("Current player: %d", player);
  printf("\n   0 1 2 3 4\n");
  printf("  -----------\n");
  for (int i = 0; i < BOARD_SIZE; i++) {
    printf("%d| ", i);
    for (int j = 0; j < BOARD_SIZE; j++) {
      switch (board[i][j]) {
        case EMPTY:
          printf("  ");
          break;
        case PLAYER1:
          printf("1 ");
          break;
        case PLAYER2:
          printf("2 ");
          break;
        case GOAL1:
          printf("G ");
          break;
        case GOAL2:
          printf("G ");
          break;
        case NON_OCCUPIABLE_SQUARE:
          printf("- ");
          break;
      }
    }
    printf("|\n");
  }
  printf("  -----------\n");
}

void(update_pawn_move)(int *pawn_move_pos, bool add_or_sub) {
  if (add_or_sub) {
    (*pawn_move_pos)++;
    if (*pawn_move_pos > (BOARD_SIZE - 1))
      *pawn_move_pos = (BOARD_SIZE - 1);
  }
  else {
    (*pawn_move_pos)--;
    if (*pawn_move_pos < 0)
      *pawn_move_pos = 0;
  }
}

int(find_board_cell)(int16_t position) {
  int16_t cell_position;
  for (int i = 1; i <= BOARD_SIZE; i++) {
    cell_position = MARGIN_LENGTH + i * (CELL_LENGTH + DIVISION_LENGTH);
    if (position <= cell_position)
      return (i - 1);
  }
  return BOARD_SIZE;
}

int(move)(uint8_t xi, uint8_t yi, uint8_t xf, uint8_t yf) {
  if (!can_move(player, xi, yi, xf, yf))
    return 0;
  set_pawn(EMPTY, xi, yi);
  set_pawn(player, xf, yf);
  return 1;
}

int(can_move)(uint8_t player, uint8_t xi, uint8_t yi, uint8_t xf, uint8_t yf) {
  if (xi < 0 || yi < 0 || xi >= BOARD_SIZE || yi >= BOARD_SIZE)
    return 0;
  if (board[xi][yi] != player) // Check if the pawn belongs to the player
    return 0;
  if (player == PLAYER1) {
    // Check if the destination is empty or goal
    if (board[xf][yf] == EMPTY || board[xf][yf] == GOAL1) {
      // Check if the movement is one square orthogonally towards the goal
      if ((yi == yf) && (xf == (xi + 1))) // One square down
        return 1;
      if ((xi == xf) && (yf == (yi - 1))) // One square to the left
        return 1;
    }
    // Check if the destination is with an opponent's pawn
    else if (board[xf][yf] == PLAYER2) {
      // Check if it's a diagonal (one-step) move
      if (abs(xi - xf) == 1 && abs(yi - yf) == 1)
        return 1;
    }
  }
  else if (player == PLAYER2) {
    // Check if the destination is empty or goal
    if (board[xf][yf] == EMPTY || board[xf][yf] == GOAL2) {
      // Check if the movement is one square orthogonally towards the goal
      if ((yi == yf) && (xf == (xi - 1))) // One square up
        return 1;
      if ((xi == xf) && (yf == (yi + 1))) // One square to the rigth
        return 1;
    }
    // Check if the destination is with an opponent's pawn
    else if (board[xf][yf] == PLAYER1) {
      // Check if it's a diagonal (one-step) move
      if (abs(xi - xf) == 1 && abs(yi - yf) == 1)
        return 1;
    }
  }
  return 0;
}

int(can_pick)(uint8_t x, uint8_t y) {
  // Check if the pawn belongs to the player
  return board[x][y] == player;
}

void(set_pawn)(uint8_t pawn, uint8_t x, uint8_t y) {
  board[x][y] = pawn;
}

int(place_back)(uint8_t x, uint8_t y) {
  if (x < 0 || y < 0 || x >= BOARD_SIZE || y >= BOARD_SIZE)
    return 0;
  if (board[x][y] == EMPTY) {
    uint8_t opponent = (player == PLAYER1) ? PLAYER2 : PLAYER1;
    set_pawn(opponent, x, y);
    return 1;
  }
  return 0;
}

int(game_over)() {
  // Check if player 1 walked a pawn in the goal 1
  if (board[BOARD_SIZE - 1][0] == PLAYER1)
    return PLAYER1;
  // Check if player 2 walked a pawn in the goal 2
  else if (board[0][BOARD_SIZE - 1] == PLAYER2)
    return PLAYER2;
  // Check if player 1 is not able to move or capture
  if (!valid_move(PLAYER1))
    return PLAYER1;
  // Check if player 2 is not able to move or capture
  if (!valid_move(PLAYER2))
    return PLAYER2;
  return 0;
}

int(valid_move)(uint8_t player) {
  for (int i = 0; i < BOARD_SIZE; i++) {
    for (int j = 0; j < BOARD_SIZE; j++) {
      if (board[i][j] == player) {
        for (int k = 0; k < BOARD_SIZE; k++) {
          for (int l = 0; l < BOARD_SIZE; l++) {
            if (can_move(player, i, j, k, l))
              return 1;
          }
        }
      }
    }
  }
  return 0;
}

void(swap_player)() {
  pawn_move.xi = 2;
  pawn_move.yi = 2;
  player = (player == PLAYER1) ? PLAYER2 : PLAYER1;
}

void(update_state_machine)() {
  switch (game_state) {
    case NEW_GAME:
      // rato
      if (mouse_state.mouse_mv) {
        if (clicked_play_button())
          game_state = CHOOSE_PAWN;
        else if (clicked_quit_button())
          game_state = QUIT;
      }
      else
        // teclado
        game_state = CHOOSE_PAWN;
      break;
    case CHOOSE_PAWN:
      if (can_pick(pawn_move.xi, pawn_move.yi))
        game_state = MOVEMENT;
      pawn_move.xf = pawn_move.xi;
      pawn_move.yf = pawn_move.yi;
      break;
    case MOVEMENT:
      game_state = CHOOSE_PAWN;
      if (move(pawn_move.xi, pawn_move.yi, pawn_move.xf, pawn_move.yf)) {
        print_board();
        printf("pawn was moved from xi=%d yi=%d to xf=%d yf=%d\n\n", pawn_move.xi, pawn_move.yi, pawn_move.xf, pawn_move.yf);
        if (abs(pawn_move.xi - pawn_move.xf) == 1 && abs(pawn_move.yi - pawn_move.yf) == 1) {
          game_state = CAPTURE;
        }
        else {
          winner = game_over();
          if (winner) {
            game_state = GAME_OVER;
            printf("GAME OVER!\nCongratulations Player %d\n", winner);
          }
          else
            swap_player();
        }
      }
      else
        printf("Invalid move!\n");
      break;
    case CAPTURE:
      if (place_back(pawn_move.xi, pawn_move.yi)) {
        print_board();
        printf("placed back the captured pawn on x=%d y=%d\n\n", pawn_move.xi, pawn_move.yi);
        game_state = CHOOSE_PAWN;
        winner = game_over();
        if (winner) {
          game_state = GAME_OVER;
          printf("GAME OVER!\nCongratulations Player %d\n", winner);
        }
        else
          swap_player();
      }
      else
        printf("Square not empty!\n");
      break;
    case GAME_OVER:
      // rato
      if (mouse_state.mouse_mv) {
        if (clicked_play_button())
          game_state = CHOOSE_PAWN;
        else if (clicked_quit_button())
          game_state = QUIT;
      }
      else
        // teclado
        game_state = CHOOSE_PAWN;
      pawn_move.xi = 2;
      pawn_move.yi = 2;
      setup_board();
      player = PLAYER1;
      break;
    default:
      break;
  }
}

void(create_sprites)() {
  menu = create_sprite((xpm_map_t) menu_xpm, 0, 0);
  play_button = create_sprite((xpm_map_t) play_button_xpm, 280, 263);
  quit_button = create_sprite((xpm_map_t) quit_button_xpm, 280, 416);
  game_over_player1 = create_sprite((xpm_map_t) game_over_player1_xpm, 0, 0);
  game_over_player2 = create_sprite((xpm_map_t) game_over_player2_xpm, 0, 0);
  player1_sp = create_sprite((xpm_map_t) player1_xpm, 400, 27);
  player2_sp = create_sprite((xpm_map_t) player2_xpm, 400, 95);
  current_player_sp = create_sprite((xpm_map_t) current_player_xpm, 368, 163);
  board_sp = create_sprite((xpm_map_t) board_xpm, 0, 0);
  board_sp_dark_mode = create_sprite((xpm_map_t) board_dark_mode_xpm, 0, 0);
  player1_pawn = create_sprite((xpm_map_t) blue_pawn_xpm, 0, 0);
  player2_pawn = create_sprite((xpm_map_t) green_pawn_xpm, 0, 0);
  player1_dark_mode_pawn = create_sprite((xpm_map_t) black_pawn_xpm, 0, 0);
  player2_dark_mode_pawn = create_sprite((xpm_map_t) white_pawn_xpm, 0, 0);
  helper_pawn = create_sprite((xpm_map_t) grey_pawn_xpm, 0, 0);
  mouse_pointer = create_sprite((xpm_map_t) mouse_pointer_xpm, 0, 0);
}

void(destroy_sprites)() {
  destroy_sprite(menu);
  destroy_sprite(play_button);
  destroy_sprite(quit_button);
  destroy_sprite(game_over_player1);
  destroy_sprite(game_over_player2);
  destroy_sprite(player1_sp);
  destroy_sprite(player2_sp);
  destroy_sprite(current_player_sp);
  destroy_sprite(board_sp);
  destroy_sprite(board_sp_dark_mode);
  destroy_sprite(player1_pawn);
  destroy_sprite(player2_pawn);
  destroy_sprite(player1_dark_mode_pawn);
  destroy_sprite(player2_dark_mode_pawn);
  destroy_sprite(helper_pawn);
  destroy_sprite(mouse_pointer);
}

int(draw_board)() {
  if (dark_mode) {
    if (draw_sprite(board_sp_dark_mode))
      return 1;
  }
  else {
    if (draw_sprite(board_sp))
      return 1;
  }
  if (draw_pawns())
    return 1;
  if (draw_side_info())
    return 1;

  return 0;
}

int(draw_pawns)() {
  for (int i = 0; i < BOARD_SIZE; i++) {
    for (int j = 0; j < BOARD_SIZE; j++) {
      switch (board[i][j]) {
        case PLAYER1:
          if (dark_mode) {
            player1_dark_mode_pawn->x = MARGIN_LENGTH + j * (CELL_LENGTH + DIVISION_LENGTH);
            player1_dark_mode_pawn->y = MARGIN_LENGTH + i * (CELL_LENGTH + DIVISION_LENGTH);
            if (draw_sprite(player1_dark_mode_pawn))
              return 1;
          }
          else {
            player1_pawn->x = MARGIN_LENGTH + j * (CELL_LENGTH + DIVISION_LENGTH);
            player1_pawn->y = MARGIN_LENGTH + i * (CELL_LENGTH + DIVISION_LENGTH);
            if (draw_sprite(player1_pawn))
              return 1;
          }
          break;
        case PLAYER2:
          if (dark_mode) {
            player2_dark_mode_pawn->x = MARGIN_LENGTH + j * (CELL_LENGTH + DIVISION_LENGTH);
            player2_dark_mode_pawn->y = MARGIN_LENGTH + i * (CELL_LENGTH + DIVISION_LENGTH);
            if (draw_sprite(player2_dark_mode_pawn))
              return 1;
          }
          else {
            player2_pawn->x = MARGIN_LENGTH + j * (CELL_LENGTH + DIVISION_LENGTH);
            player2_pawn->y = MARGIN_LENGTH + i * (CELL_LENGTH + DIVISION_LENGTH);
            if (draw_sprite(player2_pawn))
              return 1;
          }
          break;
        default:
          break;
      }
    }
  }
  return 0;
}

void(draw_valid_moves)() {
  for (int i = 0; i < BOARD_SIZE; i++) {
    for (int j = 0; j < BOARD_SIZE; j++) {
      if (can_move(player, pawn_move.xi, pawn_move.yi, i, j)) {
        helper_pawn->x = MARGIN_LENGTH + j * (CELL_LENGTH + DIVISION_LENGTH);
        helper_pawn->y = MARGIN_LENGTH + i * (CELL_LENGTH + DIVISION_LENGTH);
        draw_sprite(helper_pawn);
      }
    }
  }
}

int(draw_side_info)() {
  draw_sprite(player1_sp);
  draw_sprite(player2_sp);
  draw_sprite(current_player_sp);
  if (dark_mode) {
    player1_dark_mode_pawn->x = 690;
    player1_dark_mode_pawn->y = MARGIN_LENGTH;
    if (draw_sprite(player1_dark_mode_pawn))
      return 1;
    player2_dark_mode_pawn->x = 690;
    player2_dark_mode_pawn->y = MARGIN_LENGTH + (CELL_LENGTH + DIVISION_LENGTH);
    if (draw_sprite(player2_dark_mode_pawn))
      return 1;
    if (player == PLAYER1) {
      player1_dark_mode_pawn->x = 544;
      player1_dark_mode_pawn->y = MARGIN_LENGTH + 3 * (CELL_LENGTH + DIVISION_LENGTH);
      if (draw_sprite(player1_dark_mode_pawn))
        return 1;
    }
    else {
      player2_dark_mode_pawn->x = 544;
      player2_dark_mode_pawn->y = MARGIN_LENGTH + 3 * (CELL_LENGTH + DIVISION_LENGTH);
      if (draw_sprite(player2_dark_mode_pawn))
        return 1;
    }
  }
  else {
    player1_pawn->x = 690;
    player1_pawn->y = MARGIN_LENGTH;
    if (draw_sprite(player1_pawn))
      return 1;
    player2_pawn->x = 690;
    player2_pawn->y = MARGIN_LENGTH + (CELL_LENGTH + DIVISION_LENGTH);
    if (draw_sprite(player2_pawn))
      return 1;
    if (player == PLAYER1) {
      player1_pawn->x = 544;
      player1_pawn->y = MARGIN_LENGTH + 3 * (CELL_LENGTH + DIVISION_LENGTH);
      if (draw_sprite(player1_pawn))
        return 1;
    }
    else {
      player2_pawn->x = 544;
      player2_pawn->y = MARGIN_LENGTH + 3 * (CELL_LENGTH + DIVISION_LENGTH);
      if (draw_sprite(player2_pawn))
        return 1;
    }
  }
  return 0;
}

int(draw_mouse_pointer)() {
  return draw_sprite(mouse_pointer);
}

void(draw_main_menu)() {
  draw_sprite(menu);
  draw_sprite(play_button);
  draw_sprite(quit_button);
}

void(draw_game_over_menu)() {
  (winner == PLAYER1) ? draw_sprite(game_over_player1) : draw_sprite(game_over_player2);
  draw_sprite(play_button);
  draw_sprite(quit_button);
}

void(draw_frame)() {
  if (mouse_state.mouse_mv) {
    mouse_pointer->x = mouse_state.x;
    mouse_pointer->y = mouse_state.y;
  }
  else {
    mouse_pointer->x = MARGIN_LENGTH + pawn_move.yi * (CELL_LENGTH + DIVISION_LENGTH) + MOUSE_POINTER_OFFSET;
    mouse_pointer->y = MARGIN_LENGTH + pawn_move.xi * (CELL_LENGTH + DIVISION_LENGTH) + MOUSE_POINTER_OFFSET;
  }
  switch (game_state) {
    case NEW_GAME:
      draw_main_menu();
      break;
    case CHOOSE_PAWN:
      vg_fill(BACKGROUND_COLOR);
      draw_board();
      draw_valid_moves();
      break;
    case CAPTURE:
      vg_fill(BACKGROUND_COLOR);
      draw_board();
      if (player == PLAYER1) {
        if (dark_mode) {
          player2_dark_mode_pawn->x = mouse_pointer->x - MOUSE_POINTER_OFFSET;
          player2_dark_mode_pawn->y = mouse_pointer->y - MOUSE_POINTER_OFFSET;
          draw_sprite(player2_dark_mode_pawn);
        }
        else {
          player2_pawn->x = mouse_pointer->x - MOUSE_POINTER_OFFSET;
          player2_pawn->y = mouse_pointer->y - MOUSE_POINTER_OFFSET;
          draw_sprite(player2_pawn);
        }
      }
      else {
        if (dark_mode) {
          player1_dark_mode_pawn->x = mouse_pointer->x - MOUSE_POINTER_OFFSET;
          player1_dark_mode_pawn->y = mouse_pointer->y - MOUSE_POINTER_OFFSET;
          draw_sprite(player1_dark_mode_pawn);
        }
        else {
          player1_pawn->x = mouse_pointer->x - MOUSE_POINTER_OFFSET;
          player1_pawn->y = mouse_pointer->y - MOUSE_POINTER_OFFSET;
          draw_sprite(player1_pawn);
        }
      }
      break;
    case MOVEMENT:
      vg_fill(BACKGROUND_COLOR);
      draw_board();
      draw_valid_moves();
      if (!mouse_state.mouse_mv) {
        mouse_pointer->x = MARGIN_LENGTH + pawn_move.yf * (CELL_LENGTH + DIVISION_LENGTH) + MOUSE_POINTER_OFFSET;
        mouse_pointer->y = MARGIN_LENGTH + pawn_move.xf * (CELL_LENGTH + DIVISION_LENGTH) + MOUSE_POINTER_OFFSET;
      }
      if (player == PLAYER1) {
        if (dark_mode) {
          player1_dark_mode_pawn->x = mouse_pointer->x - MOUSE_POINTER_OFFSET;
          player1_dark_mode_pawn->y = mouse_pointer->y - MOUSE_POINTER_OFFSET;
          draw_sprite(player1_dark_mode_pawn);
        }
        else {
          player1_pawn->x = mouse_pointer->x - MOUSE_POINTER_OFFSET;
          player1_pawn->y = mouse_pointer->y - MOUSE_POINTER_OFFSET;
          draw_sprite(player1_pawn);
        }
      }
      else {
        if (dark_mode) {
          player2_dark_mode_pawn->x = mouse_pointer->x - MOUSE_POINTER_OFFSET;
          player2_dark_mode_pawn->y = mouse_pointer->y - MOUSE_POINTER_OFFSET;
          draw_sprite(player2_dark_mode_pawn);
        }
        else {
          player2_pawn->x = mouse_pointer->x - MOUSE_POINTER_OFFSET;
          player2_pawn->y = mouse_pointer->y - MOUSE_POINTER_OFFSET;
          draw_sprite(player2_pawn);
        }
      }
      break;
    case GAME_OVER:
      vg_fill(BACKGROUND_COLOR);
      draw_game_over_menu();
      break;
    default:
      break;
  }
  draw_mouse_pointer();
}

int(clicked_play_button)() {
  return (mouse_state.x >= play_button->x && mouse_state.x <= (play_button->x + play_button->width) && mouse_state.y >= play_button->y && mouse_state.y <= (play_button->y + play_button->height));
}

int(clicked_quit_button)() {
  return (mouse_state.x >= quit_button->x && mouse_state.x <= (quit_button->x + quit_button->width) && mouse_state.y >= quit_button->y && mouse_state.y <= (quit_button->y + quit_button->height));
}
