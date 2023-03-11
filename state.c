#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t* state, unsigned int snum);
static char next_square(game_state_t* state, unsigned int snum);
static void update_tail(game_state_t* state, unsigned int snum);
static void update_head(game_state_t* state, unsigned int snum);

/* Task 1 */
game_state_t* create_default_state() {
  // TODO: Implement this function.
  game_state_t * state_t = (game_state_t*) malloc(sizeof(game_state_t));
  state_t->num_rows = 18;
  unsigned int num_columns;
  num_columns = 20;
  //char **board = (char **)malloc(sizeof(char *) * state->num_rows);
  state_t->board = (char**) malloc(sizeof(char*) * state_t->num_rows);
  char *line1 = "####################";
  char *line2 = "#                  #";
  for (int i = 0; i < state_t->num_rows ; i++) {
    state_t->board[i] = (char*) malloc(sizeof(char*) * num_columns + 1);
    switch(i)
		{
      case 0 :
              strcpy(state_t->board[i], line1);
				      break;
      case 17 :
              strcpy(state_t->board[i], line1);
				      break;
      default : 
              strcpy(state_t->board[i], line2);
				      break;
    }
  }
  set_board_at(state_t, 2, 2, 'd');
  set_board_at(state_t, 2, 3,'>');
  set_board_at(state_t, 2, 4, 'D');
  set_board_at(state_t, 2, 9, '*');

  state_t->num_snakes = 1;
	state_t->snakes = (snake_t*) malloc(sizeof(snake_t));
  state_t->snakes[0].tail_row = 2;
	state_t->snakes[0].tail_col = 2;
	state_t->snakes[0].head_row = 2;
	state_t->snakes[0].head_col = 4;
	state_t->snakes[0].live = true;
  return state_t;
}

/* Task 2 */
void free_state(game_state_t* state) {
  // TODO: Implement this function.
  for(int i = 0; i < state->num_rows; i++){
    free(state->board[i]);
  }
  free(state->board);
  free(state->snakes);
  free(state);
  return;
}

/* Task 3 */
void print_board(game_state_t* state, FILE* fp) {
  // TODO: Implement this function.
  printf("%d", state->num_rows);
  for (int i = 0; i < state->num_rows; i++) {
    fprintf(fp,"%s\n", state->board[i]);
  }
  return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t* state, unsigned int row, unsigned int col) {
  return state->board[row][col];
}

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  if (c == 'w') {
    return true;
  }
  else if (c == 'a') {
    return true;
  }
  else if (c == 's') {
    return true;
  }
  else if (c == 'd') {
    return true;
  }
  else {
    return false;
  }
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  if (c == 'W') {
    return true;
  }
  else if (c == 'A') {
    return true;
  }
  else if (c == 'S') {
    return true;
  }
  else if (c == 'D') {
    return true;
  }
  else if (c == 'x') {
    return true;
  }
  else {
    return false;
  }
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  if (is_head(c) == true) {
    return true;
  }
  else if (is_tail(c) == true) {
    return true;
  }
  else if (c == '^') {
    return true;
  }
  else if (c == '<') {
    return true;
  }
  else if (c == '>') {
    return true;
  }
  else if (c == 'v') {
    return true;
  }
  else {
    return false;
  }
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implement this function.
  if (c == '^') {
    return 'w';
  }
  else if (c == '<') {
    return 'a';
  }
  else if (c == 'v') {
    return 's';
  }
  else if (c == '>') {
    return 'd';
  }
  else {
    return;
  }
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implement this function.
  if (c == 'W') {
    return '^';
  }
  else if (c == 'A') {
    return '<';
  }
  else if (c == 'S') {
    return 'v';
  }
  else if (c == 'D') {
    return '>';
  }
  else {
    return;
  }
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implement this function.
  if (c == 'v' || c == 's' || c == 'S') {
    return cur_row + 1;
  }
  else if (c == '^' || c == 'w' || c == 'W') {
    return cur_row - 1;
  }
  return cur_row;
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implement this function.
  if (c == '>' || c == 'd' || c == 'D') {
    return cur_col + 1;
  }
  else if (c == '<' || c == 'a' || c == 'A') {
    return cur_col - 1;
  }
  return cur_col;
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  unsigned int head_row = state->snakes[snum].head_row;
  unsigned int head_col = state->snakes[snum].head_col;
  char head = get_board_at(state, head_row, head_col);
  return get_board_at(state, get_next_row(head_row, head), get_next_col(head_col, head));
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t* state, unsigned int snum) {
  unsigned int head_row = state->snakes[snum].head_row;
  unsigned int head_col = state->snakes[snum].head_col;
  char prev_head = get_board_at(state, head_row, head_col);
  set_board_at(state, head_row, head_col, head_to_body(prev_head));
  set_board_at(state, get_next_row(head_row, prev_head), get_next_col(head_col, prev_head), prev_head);
  state->snakes[snum].head_row = get_next_row(head_row, prev_head);
  state->snakes[snum].head_col = get_next_col(head_col, prev_head);
  return;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  unsigned int tail_row = state->snakes[snum].tail_row;
  unsigned int tail_col = state->snakes[snum].tail_col;
  char prev_tail = get_board_at(state, tail_row, tail_col);
  set_board_at(state, tail_row, tail_col, ' ');
  set_board_at(state, get_next_row(tail_row, prev_tail), get_next_col(tail_col, prev_tail), body_to_tail(get_board_at(state, get_next_row(tail_row, prev_tail), get_next_col(tail_col, prev_tail))));
  state->snakes[snum].tail_row = get_next_row(tail_row, prev_tail);
  state->snakes[snum].tail_col = get_next_col(tail_col, prev_tail);
  return;
}

/* Task 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
  // TODO: Implement this function.
  for(int i = 0; i < state->num_snakes; i++) {
    //printf("%d\n", state->snakes[i].head_col);
    //printf("%c\n", get_board_at(state, state->snakes[i].head_row, state->snakes[i].head_col));
    if(state->snakes[i].live) {
      unsigned int head_row = state->snakes[i].head_row;
      unsigned int head_col = state->snakes[i].head_col;
      char prev_head = get_board_at(state, head_row, head_col);
      if(next_square(state, i) == '#' || is_snake(next_square(state, i)) == true) {
        state->snakes[i].live = false;
        set_board_at(state, head_row, head_col, 'x');
      }
      else if(next_square(state, i) == '*') {
        update_head(state, i);
        add_food(state);
      }
      else {
        update_head(state, i);
        update_tail(state, i);
      }
      //printf("%d\n", state->snakes[i].head_col);
      //printf("%c\n", get_board_at(state, state->snakes[i].head_row, state->snakes[i].head_col));
      //printf("%d\n", state->snakes[i].head_row);
      //printf("%c\n", get_board_at(state, state->snakes[i].head_row, state->snakes[i].head_col));
    }
  }
  return;
}

/* Task 5 */
game_state_t* load_board(char* filename) {
  // TODO: Implement this function.
  FILE *readFile = fopen(filename, "r");
  if (!readFile) {
    return NULL;
  }
  unsigned int num_rows = 0;
  size_t num_columns = 0;
  game_state_t * state_t = (game_state_t*) malloc(sizeof(game_state_t)); //enough memory for game state
  state_t->board = (char*) malloc(sizeof(char*) * num_rows); //access memory that belongs to me
  char buffer[2000];
  while (fgets(buffer, 2000, readFile)) {
      size_t strlength = strlen(buffer);
      buffer[--strlength] = 0; 
      //fgets uses /n -> get rid of
      num_rows = num_rows + 1;
      num_columns = strlength + 1; //columns is length of string of row
      state_t->board = (char**) realloc(state_t->board, sizeof(char*) * num_rows); //use realloc to keep track
      state_t->board[num_rows - 1] = (char*) malloc(sizeof(char) * num_columns + 1); //reallocate with 0th index
	    strcpy(state_t->board[num_rows - 1], buffer); //copy over on 0th index
      //printf("%d/n", strlen(buffer));
  }
  state_t->num_rows = num_rows;
  return state_t;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  unsigned int tail_to_head_row = state->snakes[snum].tail_row;
	unsigned int tail_to_head_col = state->snakes[snum].tail_col;
  //printf("%d\n", tail_to_head_col);
  char current_position = get_board_at(state, tail_to_head_row, tail_to_head_col);
  while (is_snake(get_board_at(state, get_next_row(tail_to_head_row, current_position), get_next_col(tail_to_head_col, current_position))) == true) {
    tail_to_head_row = get_next_row(tail_to_head_row, current_position);
    tail_to_head_col = get_next_col(tail_to_head_col, current_position);
    current_position = get_board_at(state, tail_to_head_row, tail_to_head_col);
    if(is_head(current_position)) {
      break;
    }
  }
  state->snakes[snum].head_row = tail_to_head_row;
  state->snakes[snum].head_col = tail_to_head_col;
  return;
}

/* Task 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
  // TODO: Implement this function.
  state->num_snakes = 0;
  for (int i = 0; i < state->num_rows; i++) {
    for (int j = 0; j < (strlen(state->board[i])); j++) {
      if (is_tail(state->board[i][j]) == true) {
        state->num_snakes += 1;
        state->snakes = (snake_t*) realloc(state->snakes, sizeof(snake_t) * state->num_snakes);
        //state->snakes = (snake_t*) malloc(sizeof(snake_t) * state->num_snakes);
        state->snakes[state->num_snakes - 1].tail_row = i;
				state->snakes[state->num_snakes - 1].tail_col = j;
        state->snakes[state->num_snakes - 1].live = true;
        find_head(state, state-> num_snakes - 1);
      }
    }
  }
  return state;
}
