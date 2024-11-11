// Froggie
//
// This program was written by Kelompok 2:
// - FRANCISCUS FEBY ETDOLO - 01085240013
// - LEONARDUS KEVIN TANJUNG - 01085240011
// - RINA RIDWAN MAMAHIT - 01087240002
// - SERAFINA ALAMSYAIR - 01087240001
// - TRIANTO - 01085240015
//
// on Semester Ganjil 2024

#include <stdio.h>
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////  CONSTANTS  /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Provided constants
#define SIZE          9
#define TRUE          1
#define FALSE         0
#define LILLYPAD_ROW  0
#define BANK_ROW      (SIZE - 1)
#define LEFT_COLUMN   0
#define RIGHT_COLUMN  (SIZE - 1)
#define MIN(a,b)      ((a) < (b) ? (a) : (b))
#define MAX(a,b)      ((a) > (b) ? (a) : (b))
#define X             0
#define Y             1
#define DEFAULT_LIVES 3
#define DEAD          0
#define GAME_WON      -1

// Provided Enums
enum tile_type {LILLYPAD, BANK, WATER, TURTLE, LOG};

////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////  STRUCTS  //////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Provided structs
struct board_tile {
    enum tile_type type;  // The type of piece it is (water, bank, etc.)
    int occupied;         // TRUE or FALSE based on if Frogger is there.
    struct bug_node* bug; // POINTER or NULL based on if Bug is there.
};

struct bug_node {
    int row;
    int col;
    struct bug_node* prev;
    struct bug_node* next;
    int moving_right;
};

////////////////////////////////////////////////////////////////////////////////
/////////////////////////////  FUNCTION PROTOTYPES  ////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void init_board(struct board_tile board[SIZE][SIZE]);
int is_placeable(int row, int col);
void place_turtles(struct board_tile board[SIZE][SIZE], int turtle_row[SIZE]);
void add_log(struct board_tile board[SIZE][SIZE], int turtle_row[SIZE]);
void add_bug(struct board_tile board[SIZE][SIZE], struct bug_node** bug_linked_list);
void clear_row(struct board_tile board[SIZE][SIZE], int last_coordinate[2], struct bug_node** bug_linked_list);
void remove_log(struct board_tile board[SIZE][SIZE], int last_coordinate[2], struct bug_node** bug_linked_list);
void move_frogger(struct board_tile board[SIZE][SIZE], char command, int last_coordinate[2]);
void move_bug(struct board_tile board[SIZE][SIZE], struct bug_node* bug_linked_list);
void occupy(struct board_tile board[SIZE][SIZE], int last_coordinate[2]);
void unoccupy(struct board_tile board[SIZE][SIZE], int last_coordinate[2]);
int check_winning_condition(struct board_tile board[SIZE][SIZE], int last_coordinate[2], int lives);
void reset_frogger(struct board_tile board[SIZE][SIZE], int last_coordinate[2]);
void print_board(struct board_tile board[SIZE][SIZE]);
char type_to_char(enum tile_type type);
struct bug_node* create_bug_node(struct bug_node** bug_linked_list, int x, int y);
void remove_bug_node(struct bug_node** bug_linked_list, struct bug_node* to_be_removed);

////////////////////////////////////////////////////////////////////////////////
//////////////////////////  FUNCTION IMPLEMENTATIONS  //////////////////////////
////////////////////////////////////////////////////////////////////////////////

int main(void) {
    printf("Welcome to Frogger Game!\n");
    struct board_tile game_board[SIZE][SIZE];
    struct bug_node* bug_linked_list = NULL; // linked list to help us add/remove bug for easy traversal and lookup
    int lives = DEFAULT_LIVES;
    int turtle_row[SIZE] = {FALSE}; // dictionary to help us indicate if certain row has a turtle for O(1) lookup
    int last_coordinate[2] = {BANK_ROW, SIZE / 2}; // (x, y)

    init_board(game_board);
    place_turtles(game_board, turtle_row);
    printf("Game Started\n");
    print_board(game_board);

    char command;
    while (lives > DEAD) {
        printf("|--------------------------------------------------------------------|\n");
        printf("| q = quit     |  l = add log    |  c = clear row  |  r = remove log |\n");
        printf("| b = add bug  |                 |                 |                 |\n");
        printf("| w = move up  |  a = move left  |  s = move down  |  d = move right |\n");
        printf("|--------------------------------------------------------------------|\n");
        printf("Enter command: ");
        int result = scanf(" %c", &command);
        if (result == EOF || command == 'q') {
            break;
        }

        switch (command) {
            case 'l':
                add_log(game_board, turtle_row);
                break;
            case 'c':
                clear_row(game_board, last_coordinate, &bug_linked_list);
                break;
            case 'r':
                remove_log(game_board, last_coordinate, &bug_linked_list);
                break;
            case 'b':
                add_bug(game_board, &bug_linked_list);
                break;
            case 'a':
            case 'w':
            case 's':
            case 'd':
                move_frogger(game_board, command, last_coordinate);
                move_bug(game_board, bug_linked_list);
                lives = check_winning_condition(game_board, last_coordinate, lives);
                break;
            default:
                printf("Invalid command!\n");
                continue;
        }
        print_board(game_board);
    }

    if (lives == DEAD) {
        printf("\n!! GAME OVER !!\n");
    } else if (lives == GAME_WON) {
        printf("Wahoo!! You Won!\n");
    }

    printf("\nThank you for playing Frogger Game!\n");
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////// ADDITIONAL FUNCTIONS /////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void init_board(struct board_tile board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            board[i][j].occupied = 0;
            board[i][j].bug = NULL;

            // first row
            if (i == 0 && j % 2 == 0) {
                board[i][j].type = LILLYPAD;
            }
            // last row print bank and frog occupied middle
            else if (i == SIZE - 1) {
                board[i][j].occupied = j == (SIZE / 2);
                board[i][j].type = BANK;
            }
            // print water
            else {
                board[i][j].type = WATER;
            }
        }
    }
}

void place_turtles(
    struct board_tile board[SIZE][SIZE],
    int turtle_row[SIZE]
) {
    // Read user input and place turtles.
    int num_turtles;
    printf("How many turtles? ");
    scanf("%d", &num_turtles);

    // Scan in the turtles, and place them on the map.
    int row, col;
    for (int i = 0; i < num_turtles; i++) {
        scanf("%d %d", &row, &col);

        if (is_placeable(row, col) == TRUE) {
            board[row][col].type = TURTLE;
            turtle_row[row] = TRUE;
        }
    }
}

/**
 * Helper function to allow us easily check if certain item is placeable in the board.
 * Assumption:
 *  1. Lilly Pad Row are prohibited (destination row)
 *  2. Bank Row are prohibited (starting row)
 */
int is_placeable(int row, int col) {
    return row > LILLYPAD_ROW
        && row < BANK_ROW
        && col >= LEFT_COLUMN
        && col <= RIGHT_COLUMN;
}

void add_log(
    struct board_tile board[SIZE][SIZE],
    int turtle_row[SIZE]
) {
    int row, start_col, end_col = -1;
    printf("Adding Log, input [row] [start_col] [end_col]: ");
    scanf("%d %d %d", &row, &start_col, &end_col);

    if (start_col > end_col) {
        printf("Invalid input! [start_col] must be less or equal than [end_col]!\n");
        return;
    }

    // since column range can be out of bounds, but log should still be placed
    // we ensure the input is within bound using MAX and MIN
    start_col = MAX(LEFT_COLUMN, start_col);
    end_col = MIN(RIGHT_COLUMN, end_col);

    // check if placable and is not occupied by turtle
    if (is_placeable(row, start_col) == TRUE
        && is_placeable(row, end_col) == TRUE
        && turtle_row[row] == FALSE
    ) {
        for (int col = start_col; col <= end_col; col++) {
            board[row][col].type = LOG;
        }
    }
}

void add_bug(
    struct board_tile board[SIZE][SIZE],
    struct bug_node** bug_linked_list
) {
    int row, col = -1;
    printf("Adding Bug, input [row] [col]: ");
    scanf("%d %d", &row, &col);

    if ((board[row][col].type != TURTLE && board[row][col].type != LOG)
        || board[row][col].bug != NULL
    ) {
        return;
    }
    board[row][col].bug = create_bug_node(bug_linked_list, row, col);
}

void clear_row(
    struct board_tile board[SIZE][SIZE],
    int last_coordinate[2],
    struct bug_node** bug_linked_list
) {
    int row;
    printf("Enter row to clear: ");
    scanf("%d", &row);

    // Check if row is out of bounds OR prohibited (LILLYPAD or BANK row)
    if (is_placeable(row, LEFT_COLUMN) == FALSE) {
        printf("Error: Row must be between %d and %d.\n", LILLYPAD_ROW + 1, BANK_ROW - 1);
        return;
    }

    // Check if Frogger is on the row
    if (last_coordinate[X] == row) {
        printf("Error: Cannot clear row %d because Frogger is present.\n", row);
        return;
    }

    // Clear the row by setting all tiles to WATER
    for (int col = 0; col < SIZE; col++) {
        if (board[row][col].type == TURTLE || board[row][col].type == LOG) {
            board[row][col].type = WATER;
        }
        if (board[row][col].bug != NULL) {
            remove_bug_node(bug_linked_list, board[row][col].bug);
            board[row][col].bug = NULL;
        }
    }
    printf("Row %d cleared.\n", row);
}

void remove_log(
    struct board_tile board[SIZE][SIZE],
    int last_coordinate[2],
    struct bug_node** bug_linked_list
) {
    int row, col;
    printf("To remove log(s), please input [row] [column]: ");
    scanf("%d %d", &row, &col);

    //Tile that is not log tile will not be changed.
    if (board[row][col].type != LOG)  {
        printf("There is no log to remove at row %d and column %d.\n", row, col);
        return;
    }

    //To check for Frogger.
    int frogger_present = FALSE;

    int z = col; //Looping to check if Frogger is present on the left.
    while ((z >= LEFT_COLUMN) && board[row][z].type == LOG) {
        if(board[row][z].occupied == TRUE) {
            frogger_present = TRUE; //if present, set frogger_present to TRUE.
        }
        z--;
    }

    int y = col; //Looping to check if Frogger is present on the right.
    while ((y <= RIGHT_COLUMN) && board[row][y].type == LOG) {
        if(board[row][y].occupied == TRUE) {
            frogger_present = TRUE; //if present, set frogger_present to TRUE.
        }
        y++;
    }

    //To remove logs if Frogger is not present on any adjacent logs.
    if (frogger_present == TRUE) {
        printf("Cannot remove logs as a Frogger is present on an adjacent log.\n");
    } else {
        int removal = z;
        while (removal <= y) {
            board[row][removal].type = WATER;
            if (board[row][removal].bug != NULL) {
                remove_bug_node(bug_linked_list, board[row][removal].bug);
                board[row][removal].bug = NULL;
            }
            removal++;
        }
        printf("Log(s) removed on row %d and column %d, and any logs adjacent to it.\n", row, col);
    }
}

void move_frogger(
    struct board_tile board[SIZE][SIZE],
    char command,
    int last_coordinate[2]
) {
    unoccupy(board, last_coordinate);
    switch (command) {
        case 'w':
            last_coordinate[X] = MAX(last_coordinate[X] - 1, LILLYPAD_ROW);
            break;
        case 's':
            last_coordinate[X] = MIN(last_coordinate[X] + 1, BANK_ROW);
            break;
        case 'a':
            last_coordinate[Y] = MAX(last_coordinate[Y] - 1, LEFT_COLUMN);
            break;
        case 'd':
            last_coordinate[Y] = MIN(last_coordinate[Y] + 1, RIGHT_COLUMN);
            break;
    }
    occupy(board, last_coordinate);
}

int is_bug_moveable(
    struct board_tile board[SIZE][SIZE],
    int row,
    int col
) {
    return row > LILLYPAD_ROW && row < BANK_ROW
        && col <= RIGHT_COLUMN && col >= LEFT_COLUMN
        && (board[row][col].type == TURTLE || board[row][col].type == LOG)
        && (board[row][col].bug == NULL);
}

void move_bug_direction(
    struct board_tile board[SIZE][SIZE],
    struct bug_node* bug_linked_list,
    int direction
) {
    struct bug_node* current_bug = bug_linked_list;
    int row = current_bug->row;
    int col = current_bug->col;
    int next_col = col + direction;
    if (is_bug_moveable(board, row, next_col) == FALSE) {
        if (is_bug_moveable(board, row, col - direction) == TRUE) {
            current_bug->moving_right = !current_bug->moving_right;
            move_bug_direction(board, current_bug, -direction);
        }
    } else {
        board[row][col].bug = NULL;
        board[row][next_col].bug = current_bug;
        current_bug->col = next_col;
    }
}

void move_bug(
    struct board_tile board[SIZE][SIZE],
    struct bug_node* bug_linked_list
) {
    struct bug_node* current_bug = bug_linked_list;
    while (current_bug != NULL) {
        move_bug_direction(board, current_bug, current_bug->moving_right ? 1 : -1);
        current_bug = current_bug->next;
    }
}

void occupy(
    struct board_tile board[SIZE][SIZE],
    int last_coordinate[2]
) {
    board[last_coordinate[X]][last_coordinate[Y]].occupied = TRUE;
}

void unoccupy(
    struct board_tile board[SIZE][SIZE],
    int last_coordinate[2]
) {
    board[last_coordinate[X]][last_coordinate[Y]].occupied = FALSE;
}

int check_winning_condition(
    struct board_tile board[SIZE][SIZE],
    int last_coordinate[2],
    int lives
) {
    //To simplify current Frogger tile.
    struct board_tile tile = board[last_coordinate[X]][last_coordinate[Y]];

    //If the Frogger lands on water, decrease lives.
    if (tile.type == WATER) {
        lives--;
        printf("Frogger has fallen into the water! Remaining lives: %d\n", lives);

        if (lives == 0) //Should lives reach 0, player has lost the game (losing condition).
        {
            printf("\n!! Game Over !!\n");
            return 0;
        }
        else //If lives have not reached 0, reset Frogger position and print the game board to start again.
        {
            reset_frogger(board, last_coordinate);
            return lives;
        }
    }

    //If Frogger hits a bug, decrease lives.
    if (tile.bug != NULL) { //Conditional check for bugs.
        lives--;
        printf("Oh no! Frogger hit a bug! Remaining lives: %d\n", lives);

        if (lives == 0) //Should lives reach 0, player has lost the game (losing condition).
        {
            printf("\n!! Game Over !!\n");
            return 0;
        }
        else //If lives have not reached 0, reset Frogger position and print the game board to start again.
        {
            reset_frogger(board, last_coordinate);
            return lives;
        }
    }

    //If Frogger lands on a lillypad, player has won the game (winning condition).
    if (tile.type == LILLYPAD) {
        printf("\nWahoo!! You Won!\n");
        return -1;
    }
    return lives;
}

void reset_frogger(
    struct board_tile board[SIZE][SIZE],
    int last_coordinate[2]
) {
    board[last_coordinate[X]][last_coordinate[Y]].occupied = FALSE;
    last_coordinate[X] = BANK_ROW;
    last_coordinate[Y] = SIZE / 2;
    board[last_coordinate[X]][last_coordinate[Y]].occupied = TRUE;
}

/**
 * This function acts as a helper to hide the complexity of how we can add a
 * bug into the first item of the linked list.
 *
 * Linked List is a list data structure that allows easier addition/removal of item
 * into the list, since array needs to have a predetermined length, and removal might
 * introduce gap in the list.
 */
struct bug_node* create_bug_node(
    struct bug_node** bug_linked_list,
    int row,
    int col
) {
    struct bug_node* bug = (struct bug_node*) malloc(sizeof(struct bug_node));
    if (bug == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    bug->row = row;
    bug->col = col;
    bug->prev = NULL;
    bug->next = NULL;
    bug->moving_right = TRUE;

    // list is empty, therefore set as HEAD
    if (*bug_linked_list == NULL) {
        *bug_linked_list = bug;
        return bug;
    }

    // insert in the list based on the row/column order
    struct bug_node* current = *bug_linked_list;
    struct bug_node* previous = NULL;
    while (current != NULL && (
        current->row < bug->row                                     // exit if current item has row smaller than inserted bug
        || (current->row == bug->row && current->col < bug->col)    // exit if current item has column smaller than inserted bug
    )) {
        // keep reference to the previous node, since we have doubly linked list
        previous = current;
        // iterate to the next node
        current = current->next;
    }

    // if no previous, then we need to insert at the beginning
    if (previous == NULL) {
        bug->next = *bug_linked_list;
        (*bug_linked_list)->prev = bug;
        *bug_linked_list = bug;
    }
    // else it's inserting in the middle or end
    else {
        bug->next = current;
        bug->prev = previous;
        previous->next = bug;
        // if it's not the end, then we need to re-point the next node to current node
        if (current != NULL) {
            current->prev = bug;
        }
    }

    return bug;
}

/**
 * This function acts as a helper to hide the complexity of how we can remove
 * a certain bug from the linked list.
 */
void remove_bug_node(
    struct bug_node** bug_linked_list,
    struct bug_node* removed
) {
    if (removed == *bug_linked_list) {
        *bug_linked_list = removed->next;
    }
    if (removed->prev != NULL) {
        removed->prev->next = removed->next;
    }
    if (removed->next != NULL) {
        removed->next->prev = removed->prev;
    }
    free(removed);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////// PROVIDED FUNCTIONS //////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void print_board(struct board_tile board[SIZE][SIZE]) {
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            char type_char = '\0';
            if (board[row][col].occupied) {
                type_char = 'F';
            } else if (board[row][col].bug != NULL) {
                type_char = 'B';
            } else {
                type_char = type_to_char(board[row][col].type);
            }
            printf("%c ", type_char);
        }
        printf("\n");
    }
}

char type_to_char(enum tile_type type) {
    char type_char = ' ';
    if (type == LILLYPAD) {
        type_char = 'o';
    } else if (type == BANK) {
        type_char = 'x';
    } else if (type == WATER) {
        type_char = '~';
    } else if (type == TURTLE) {
        type_char = 'T';
    } else if (type == LOG) {
        type_char = 'L';
    }
    return type_char;
}
