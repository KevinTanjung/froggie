// Froggie
//
// This program was written by Kelompok 2:
// - FRANCISCUS FEBY ETDOLO - 01085240013
// - LEONARDUS KEVIN TANJUNG - 01085240011
// - RINA RIDWAN MAMAHIT - 01087240002
// - SERAFINA ALMASYAIR - 01087240001
// - TRIANTO - 01085240015
//
// on Semester Ganjil 2024

#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////  CONSTANTS  /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Provided constants
#define SIZE         9
#define TRUE         1
#define FALSE        0
#define LILLYPAD_ROW 0
#define BANK_ROW     (SIZE - 1)
#define LEFT_COLUMN  0
#define RIGHT_COLUMN (SIZE - 1)
#define MIN(a,b)     ((a) < (b) ? (a) : (b))
#define MAX(a,b)     ((a) > (b) ? (a) : (b))
#define X            0
#define Y            1

// Provided Enums
enum tile_type {LILLYPAD, BANK, WATER, TURTLE, LOG};

////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////  STRUCTS  //////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Provided structs
struct board_tile {
    enum tile_type type; // The type of piece it is (water, bank, etc.)
    int occupied;        // TRUE or FALSE based on if Frogger is there.
};

////////////////////////////////////////////////////////////////////////////////
/////////////////////////////  FUNCTION PROTOTYPES  ////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void init_board(struct board_tile board[SIZE][SIZE]);
int is_placeable(int row, int col);
void place_turtles(struct board_tile board[SIZE][SIZE], int turtle_row[SIZE]);
void add_log(struct board_tile board[SIZE][SIZE], int turtle_row[SIZE]);
void clear_row(struct board_tile board[SIZE][SIZE], int last_coordinate[2]);
void remove_log(struct board_tile board[SIZE][SIZE], int last_coordinate[2]);
void move_frogger(struct board_tile board[SIZE][SIZE], char command, int last_coordinate[2]);
void print_board(struct board_tile board[SIZE][SIZE]);
char type_to_char(enum tile_type type);

////////////////////////////////////////////////////////////////////////////////
//////////////////////////  FUNCTION IMPLEMENTATIONS  //////////////////////////
////////////////////////////////////////////////////////////////////////////////

int main(void) {

    printf("Welcome to Frogger Game!\n");
    struct board_tile game_board[SIZE][SIZE];
    int turtle_row[SIZE] = {FALSE}; // dictionary to help us indicate if certain row has a turtle for O(1) lookup
    int last_coordinate[2]; // (x, y)
    last_coordinate[X] = BANK_ROW;
    last_coordinate[Y] = SIZE / 2;

    init_board(game_board);
    place_turtles(game_board, turtle_row);
    printf("Game Started\n");
    print_board(game_board);

    char command;
    while (1) {
        printf("|--------------------------------------------------------------------|\n");
        printf("| q = quit     |  l = add log    |  c = clear row  |  r = remove log |\n");
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
                clear_row(game_board, last_coordinate);
                break;
            case 'r':
                remove_log(game_board, last_coordinate);
                break;
            case 'a':
            case 'w':
            case 's':
            case 'd':
                move_frogger(game_board, command, last_coordinate);
                break;
            default:
                printf("Invalid command!\n");
                continue;
        }
        print_board(game_board);
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
            // first row
            if (i == 0 && j % 2 == 0) {
                board[i][j].occupied = 0;
                board[i][j].type = LILLYPAD;
            }
            // last row print bank and frog occupied middle
            else if (i == SIZE - 1) {
                board[i][j].occupied = j == (SIZE / 2);
                board[i][j].type = BANK;
            }
            // print water
            else {
                board[i][j].occupied = 0;
                board[i][j].type = WATER;
            }
        }
    }
}

void place_turtles(struct board_tile board[SIZE][SIZE], int turtle_row[SIZE]) {
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

void add_log(struct board_tile board[SIZE][SIZE], int turtle_row[SIZE]) {
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

void clear_row(
    struct board_tile board[SIZE][SIZE],
    int last_coordinate[2]
) {
    printf("// TODO clear_row [row]\n");
}

void remove_log(
    struct board_tile board[SIZE][SIZE],
    int last_coordinate[2]
) {
    printf("// TODO remove_log [row] [column]\n");
}

void move_frogger(
    struct board_tile board[SIZE][SIZE],
    char command,
    int last_coordinate[2]
) {
    printf("// TODO move_frogger [%c]\n", command);
    // TODO
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
